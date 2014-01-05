#ifndef __SERVER_DATA_HH__
#define __SERVER_DATA_HH__

#include	<boost/bimap.hpp>
#include	<mongo/client/dbclient.h>
#include	"TcpClient.hh"

class ServerData
{
public:
  typedef boost::bimap<unsigned int, TcpClient::Ptr> clients_type;

  static ServerData	&getInstance();

  void	connectToMongo();

  unsigned int	generateId();
  void		releaseId(unsigned int);

  void	removeClient(TcpClient::Ptr);

  void	sendContacts(TcpClient::Ptr);

  // Parser handlers
  void	handleParserError(TcpClient::Ptr);
  void	handleParserCreateAccount(TcpClient::Ptr, const std::string &, const std::string &);
  void	handleParserConnect(TcpClient::Ptr, const std::string &, const std::string &, unsigned int);
  void	handleParserSetStatus(TcpClient::Ptr, unsigned int, const std::string &);
  void	handleParserListContacts(TcpClient::Ptr);
  void	handleParserAddContact(TcpClient::Ptr, const std::string &);
  void	handleParserDeleteContact(TcpClient::Ptr, unsigned int);
  void	handleParserCallId(TcpClient::Ptr, unsigned int);
  void	handleParserAcceptCall(TcpClient::Ptr, unsigned int);
  void	handleParserDeclineCall(TcpClient::Ptr, unsigned int);
  void	handleParserEndCall(TcpClient::Ptr, unsigned int) {}
  void	handleParserSendMessage(TcpClient::Ptr, unsigned int, const std::string &);
  void	handleParserGetMessages(TcpClient::Ptr, unsigned int);

private:
  ServerData();

  static ServerData		_instance;

  mongo::DBClientConnection	_mongo;
  unsigned int			_maxId;
  std::queue<unsigned int>	_availableId;
  clients_type			_clients;
};

#endif
