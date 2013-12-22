#ifndef __SERVER_DATA_HH__
#define __SERVER_DATA_HH__

#include	<boost/bimap.hpp>
#include	<mongo/client/dbclient.h>
#include	"TcpClient.hh"

class ServerData
{
public:
  typedef boost::bimap<uint16_t, TcpClient::Ptr> clients_type;

  static ServerData	&getInstance();

  void	connectToMongo();

  unsigned int	generateId();
  void		releaseId(unsigned int);

  void	removeClient(TcpClient::Ptr);

  // Parser handlers
  void	handleParserError(TcpClient::Ptr);
  void	handleParserConnect(TcpClient::Ptr, const std::string &, const std::string &);
  void	handleParserCreateAccount(TcpClient::Ptr, const std::string &, const std::string &);
  void	handleParserCallId(TcpClient::Ptr, unsigned int) {}
  void	handleParserSetStatus(TcpClient::Ptr, unsigned int, const std::string &) {}
  void	handleParserSendMessage(TcpClient::Ptr, unsigned int, const std::string &) {}
  void	handleParserListContacts(TcpClient::Ptr) {}
  void	handleParserAddContact(TcpClient::Ptr, const std::string &) {}
  void	handleParserDeleteContact(TcpClient::Ptr, unsigned int) {}
  void	handleParserAcceptCall(TcpClient::Ptr, unsigned int) {}
  void	handleParserDeclineCall(TcpClient::Ptr, unsigned int) {}
  void	handleParserGetMessages(TcpClient::Ptr, unsigned int) {}

private:
  ServerData();

  static ServerData		_instance;

  mongo::DBClientConnection	_mongo;
  unsigned int			_maxId;
  std::queue<unsigned int>	_availableId;
  clients_type			_clients;
};

#endif
