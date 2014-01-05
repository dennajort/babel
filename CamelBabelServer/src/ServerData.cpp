#include	<boost/foreach.hpp>
#include	<ctime>
#include	"ServerData.hh"
#include	"config.hh"
#include	"tools.hh"

ServerData	ServerData::_instance;

ServerData	&ServerData::getInstance()
{
  return _instance;
}

ServerData::ServerData() :
  _mongo(true)
{
}

void	ServerData::connectToMongo()
{
  try
    {
      _mongo.connect(MONGODB_URI);
      std::cout << "Sucessfully connected to MongoDB" << std::endl;
    }
  catch (const mongo::ConnectException &e)
    {
      std::cerr << "Error: can't connect to MongoDB" << std::endl;
      exit(1);
    }
}

unsigned int	ServerData::generateId()
{
  unsigned int	id;

  if (_availableId.empty())
    return _maxId++;
  id = _availableId.front();
  _availableId.pop();
  return id;
}

void	ServerData::releaseId(unsigned int id)
{
  _availableId.push(id);
}

void	ServerData::removeClient(TcpClient::Ptr client)
{
  if (client->isAuthenticated())
    {
      mongo::auto_ptr<mongo::DBClientCursor>	cursor = _mongo.query(MONGODB_COL,
								      BSON(FIELD_CONTACTS << BSON("$in" << BSON_ARRAY(client->getOID()))));

      while (cursor->more())
	{
	  mongo::BSONObj	tmp = cursor->next();
	  unsigned int		id = tools::OIDToUint(tmp[FIELD_ID].OID());
	  
	  if (_clients.left.count(id) != 0)
	    {
	      TcpClient::Ptr	contact = _clients.left.at(id);
	      
	      contact->sendContact(client->getID(), client->getUsername(), BABEL_LOGOUT_STATE, "");
	    }
	}
      client->setAuthenticated(false);
    }
  _clients.right.erase(client);
}

void	ServerData::sendContacts(TcpClient::Ptr client)
{
  if (client->isAuthenticated())
    {
      mongo::BSONObj	obj = _mongo.findOne(MONGODB_COL,
					     BSON(FIELD_ID << client->getOID()));

      if (!obj.isEmpty())
	{
	  mongo::auto_ptr<mongo::DBClientCursor>	cursor;
	  mongo::BSONArrayBuilder			builder;

	  BOOST_FOREACH(mongo::BSONElement elem, obj[FIELD_CONTACTS].Array())
	    {
	      builder.append(elem.OID());
	    }
	  cursor = _mongo.query(MONGODB_COL,
				BSON(FIELD_ID << BSON("$in" << builder.arr())));
	  while (cursor->more())
	    {
	      mongo::BSONObj	tmp = cursor->next();
	      unsigned int	id = tools::OIDToUint(tmp[FIELD_ID].OID());

	      if (_clients.left.count(id) == 0)
		client->sendContact(id, tmp[FIELD_USERNAME].String(), BABEL_LOGOUT_STATE, "");
	      else
		{
		  TcpClient::Ptr	contact = _clients.left.at(id);

		  client->sendContact(id, contact->getUsername(), contact->getState(), contact->getMood());
		}
	    }
	}
    }
}

void	ServerData::handleParserCreateAccount(TcpClient::Ptr client, const std::string &username, const std::string &password)
{
  if (!_mongo.findOne(MONGODB_COL, BSON(FIELD_USERNAME << username)).isEmpty())
    client->sendResp(403, "Username already taken");
  else
    {
      mongo::BSONArrayBuilder	arb;
      mongo::BSONArrayBuilder	arbb;

      _mongo.insert(MONGODB_COL,
		    BSON(mongo::GENOID << FIELD_USERNAME << username << FIELD_PASSWORD << tools::lower(password) << FIELD_CONTACTS << arb.arr() << FIELD_MESSAGES << arbb.arr()));
      client->sendResp(200, "Created");
    }
}

void	ServerData::handleParserConnect(TcpClient::Ptr client, const std::string &username, const std::string &password, unsigned int port)
{
  if (client->isAuthenticated())
    client->sendResp(405, "Already connected");
  else
    {
      mongo::BSONObj	obj = _mongo.findOne(MONGODB_COL,
					     BSON(FIELD_USERNAME << username));
      
      if (!obj.isEmpty() && tools::sha1(obj[FIELD_PASSWORD].String() + client->getSalt()) == tools::lower(password))
	{
	  client->setAuthenticated(true);
	  client->setOID(obj[FIELD_ID].OID());
	  client->setUsername(username);
	  client->setPort(port);
	  _clients.insert(clients_type::value_type(client->getID(), client));
	  client->sendResp(200, "Connected");
	  sendContacts(client);
	}
      else
	client->sendResp(403, "User or password do not match");
    }
}

void	ServerData::handleParserSetStatus(TcpClient::Ptr client, unsigned int state, const std::string &mood)
{
  if (!client->isAuthenticated())
    client->sendResp(403, "Unauthorized to change status");
  else
    {
      mongo::auto_ptr<mongo::DBClientCursor>	cursor = _mongo.query(MONGODB_COL,
								      BSON(FIELD_CONTACTS << BSON("$in" << BSON_ARRAY(client->getOID()))));

      client->setMood(mood);
      client->setState(state);
      client->sendResp(200, "Status changed");
      while (cursor->more())
	{
	  mongo::BSONObj	tmp = cursor->next();
	  unsigned int		id = tools::OIDToUint(tmp[FIELD_ID].OID());
	  
	  if (_clients.left.count(id) != 0)
	    {
	      TcpClient::Ptr	contact = _clients.left.at(id);
	      
	      contact->sendContact(client->getID(), client->getUsername(), state, mood);
	    }
	}
    }
}

void	ServerData::handleParserListContacts(TcpClient::Ptr client)
{
  sendContacts(client);
}

void	ServerData::handleParserAddContact(TcpClient::Ptr client, const std::string &username)
{
  if (!client->isAuthenticated())
    client->sendResp(403, "Unauthorized to add contact");
  else
    {
      mongo::BSONObj	obj = _mongo.findOne(MONGODB_COL,
					     BSON(FIELD_USERNAME << username));

      if (obj.isEmpty())
	client->sendResp(403, "User does not exists");
      else
	{
	  unsigned int		id = tools::OIDToUint(obj[FIELD_ID].OID());

	  _mongo.update(MONGODB_COL,
			BSON(FIELD_ID << client->getOID()),
			BSON("$addToSet" << BSON(FIELD_CONTACTS << obj[FIELD_ID].OID())));
	  client->sendResp(200, "Contact added");
	  if (_clients.left.count(id) == 0)
	    client->sendContact(id, obj[FIELD_USERNAME].String(), BABEL_LOGOUT_STATE, "");
	  else
	    {
	      TcpClient::Ptr	contact = _clients.left.at(id);
	      
	      client->sendContact(id, contact->getUsername(), contact->getState(), contact->getMood());
	    }
	}
    }
}

void	ServerData::handleParserDeleteContact(TcpClient::Ptr client, unsigned int id)
{
  if (!client->isAuthenticated())
    client->sendResp(403, "Unauthorized to delete contact");
  else
    {
      _mongo.update(MONGODB_COL,
		    BSON(FIELD_ID << client->getOID()),
		    BSON("$pull" << BSON(FIELD_CONTACTS << tools::uintToOID(id))));
      client->sendResp(200, "Contact deleted");
    }
}

void	ServerData::handleParserCallId(TcpClient::Ptr client, unsigned int id)
{
  if (!client->isAuthenticated())
    client->sendResp(403, "Unauthorized to call");
  else
    {
      if (_clients.left.count(id) == 0)
	client->sendResp(404, "User unavailable or does not exist");
      else
	{
	  TcpClient::Ptr	contact = _clients.left.at(id);
	  
	  client->sendResp(200, "Waiting for answer");
	  contact->sendCall(client->getID());
	}      
    }
}

void	ServerData::handleParserAcceptCall(TcpClient::Ptr client, unsigned int id)
{
  if (!client->isAuthenticated())
    client->sendResp(403, "You must be authenticated");
  else
    {
      if (_clients.left.count(id) != 0)
	{
	  TcpClient::Ptr	contact = _clients.left.at(id);
	  
	  contact->sendContactIp(client);
	  client->sendContactIp(contact);
	}      
    }
}

void	ServerData::handleParserDeclineCall(TcpClient::Ptr client, unsigned int id)
{
  if (!client->isAuthenticated())
    client->sendResp(403, "You must be authenticated");
  else
    {
      if (_clients.left.count(id) != 0)
	{
	  TcpClient::Ptr	contact = _clients.left.at(id);
	  
	  contact->sendDeclinedCall(client->getID());
	}      
    }
}

void	ServerData::handleParserSendMessage(TcpClient::Ptr client, unsigned int dest, const std::string &msg)
{
  if (!client->isAuthenticated())
    client->sendResp(403, "Unauthorized");
  else
    {
      if (_clients.left.count(dest) != 0)
	{
	  TcpClient::Ptr	contact = _clients.left.at(dest);

	  contact->sendMessage(client->getID(), msg, time(NULL));
	}
      else
	_mongo.update(MONGODB_COL,
		      BSON(FIELD_ID << tools::uintToOID(dest)),
		      BSON("$push" << BSON(FIELD_MESSAGES << BSON(FIELD_MESS_FROM << client->getOID() << FIELD_MESS_MESS << msg << FIELD_MESS_DATE << mongo::DATENOW))));
      client->sendResp(200, "Message sent");
    }
}

void	ServerData::handleParserGetMessages(TcpClient::Ptr client, unsigned int from)
{
  if (!client->isAuthenticated())
    client->sendResp(403, "Unauthorized");
  else
    {
      mongo::BSONObj	obj = _mongo.findOne(MONGODB_COL,
					     BSON(FIELD_ID << client->getOID()));

      if (!obj.isEmpty())
	{
	  mongo::OID	fromOID = tools::uintToOID(from);

	  BOOST_FOREACH(mongo::BSONElement elem, obj[FIELD_MESSAGES].Array())
	    {
	      mongo::BSONObj	message = elem.Obj();

	      if (message[FIELD_MESS_FROM].OID() == fromOID)
		client->sendMessage(from, message[FIELD_MESS_MESS].String(), message[FIELD_MESS_DATE].Date().millis / 1000);
	    }
	  _mongo.update(MONGODB_COL,
			BSON(FIELD_ID << client->getOID()),
			BSON("$pull" << BSON(FIELD_MESS_FROM << fromOID)));
	}
    }
}

void	ServerData::handleParserError(TcpClient::Ptr client)
{
  std::cerr << "Error Parser" << std::endl;
}
