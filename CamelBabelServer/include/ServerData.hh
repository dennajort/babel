#ifndef __SERVER_DATA_HH__
#define __SERVER_DATA_HH__

#include	<map>
#include	"TcpClient.hh"

class ServerData
{
public:
  static ServerData	&getInstance();

  void	removeClient(TcpClient::Ptr);

private:
  static ServerData				_instance;

  std::map<unsigned int, TcpClient::Ptr>	_clients;
};

#endif
