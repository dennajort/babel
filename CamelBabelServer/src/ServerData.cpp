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
	  _clients.insert(clients_type::value_type(tools::OIDToUint(obj["_id"].OID()), client));
	  client->sendResp(200, "Connected");
	}
      else
	client->sendResp(403, "User or password do not match");
    }
}

void	ServerData::handleParserCreateAccount(TcpClient::Ptr client, const std::string &username, const std::string &password)
{
  if (!_mongo.findOne(MONGODB_COL_USERS, BSON("username" << username)).isEmpty())
    client->sendResp(403, "Username already taken");
  else
    {
      _mongo.insert(MONGODB_COL_USERS, BSON(mongo::GENOID << "username" << username << "password" << tools::lower(password)));
      client->sendResp(200, "Created");
    }
}

void	ServerData::handleParserError(TcpClient::Ptr client)
{
  std::cerr << "Error Parser" << std::endl;
}
