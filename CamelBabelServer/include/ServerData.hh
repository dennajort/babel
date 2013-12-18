#ifndef __SERVER_DATA_HH__
#define __SERVER_DATA_HH__

#include	<map>

class TcpClient;

class ServerData
{
public:
  void	removeClient(TcpClient *);

private:
  std::map<unsigned int, TcpClient *>	_clients;
};

#endif
