#include	"ServerData.hh"

ServerData	ServerData::_instance;

ServerData	&ServerData::getInstance()
{
  return _instance;
}

void	ServerData::removeClient(TcpClient::Ptr client)
{
}
