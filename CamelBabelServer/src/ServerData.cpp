#include	<boost/foreach.hpp>
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
  _clients.right.erase(client);
  client->setAuthenticated(false);
}

void	ServerData::sendContacts(TcpClient::Ptr client)
{
  if (client->isAuthenticated())
    {
      mongo::BSONObj	obj = _mongo.findOne(MONGODB_COL_USERS, BSON("_id" << client->getOID()));

      if (!obj.isEmpty())
	{
	  mongo::auto_ptr<mongo::DBClientCursor>	cursor;
	  mongo::BSONArrayBuilder			builder;

	  BOOST_FOREACH(mongo::BSONElement elem, obj["contacts"].Array())
	    {
	      builder.append(elem.OID());
	    }
	  cursor = _mongo.query(MONGODB_COL_USERS, BSON("_id" << BSON("$in" << builder.arr())));
	  while (cursor->more())
	    {
	      mongo::BSONObj	tmp = cursor->next();
	      unsigned int	id = tools::OIDToUint(tmp["_id"].OID());

	      if (_clients.left.count(id) == 0)
		client->sendContact(id, tmp["username"].String(), 0, "");
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
  if (!_mongo.findOne(MONGODB_COL_USERS, BSON("username" << username)).isEmpty())
    client->sendResp(403, "Username already taken");
  else
    {
      mongo::BSONArrayBuilder	arb;

      _mongo.insert(MONGODB_COL_USERS, BSON(mongo::GENOID << "username" << username << "password" << tools::lower(password) << "contacts" << arb.arr()));
      client->sendResp(200, "Created");
    }
}

void	ServerData::handleParserConnect(TcpClient::Ptr client, const std::string &username, const std::string &password)
{
  if (client->isAuthenticated())
    client->sendResp(405, "Already connected");
  else
    {
      mongo::BSONObj	obj = _mongo.findOne(MONGODB_COL_USERS, BSON("username" << username));
      
      if (!obj.isEmpty() && tools::sha1(obj["password"].String() + client->getSalt()) == tools::lower(password))
	{
	  client->setAuthenticated(true);
	  client->setOID(obj["_id"].OID());
	  client->setUsername(username);
	  _clients.insert(clients_type::value_type(tools::OIDToUint(client->getOID()), client));
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
      mongo::auto_ptr<mongo::DBClientCursor>	cursor = _mongo.query(MONGODB_COL_USERS, BSON("contacts" << BSON("$in" << BSON_ARRAY(client->getOID()))));

      client->setMood(mood);
      client->setState(state);
      client->sendResp(200, "Status changed");
      while (cursor->more())
	{
	  mongo::BSONObj	tmp = cursor->next();
	  unsigned int		id = tools::OIDToUint(tmp["_id"].OID());
	  
	  if (_clients.left.count(id) != 0)
	    {
	      TcpClient::Ptr	contact = _clients.left.at(id);
	      
	      contact->sendContact(tools::OIDToUint(client->getOID()), client->getUsername(), state, mood);
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
      mongo::BSONObj	obj = _mongo.findOne(MONGODB_COL_USERS, BSON("username" << username));

      if (obj.isEmpty())
	client->sendResp(403, "User does not exists");
      else
	{
	  unsigned int		id = tools::OIDToUint(obj["_id"].OID());

	  _mongo.update(MONGODB_COL_USERS, BSON("_id" << client->getOID()), BSON("$addToSet" << BSON("contacts" << obj["_id"].OID())));
	  client->sendResp(200, "Contact added");
	  if (_clients.left.count(id) == 0)
	    client->sendContact(id, obj["username"].String(), 0, "");
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
      _mongo.update(MONGODB_COL_USERS, BSON("_id" << client->getOID()), BSON("$pull" << BSON("contacts" << tools::uintToOID(id))));
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
	  
	  contact->sendCall(tools::OIDToUint(client->getOID()));
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
	  
	  contact->sendContactIp(tools::OIDToUint(client->getOID()), client->getSocket().remote_endpoint().address().to_string());
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
	  
	  contact->sendDeclinedCall(tools::OIDToUint(client->getOID()));
	}      
    }
}

void	ServerData::handleParserError(TcpClient::Ptr client)
{
  std::cerr << "Error Parser" << std::endl;
}
