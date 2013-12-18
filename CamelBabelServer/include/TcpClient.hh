#ifndef __TCP_CLIENT_HH__
#define __TCP_CLIENT_HH__

#include	<string>
#include	<queue>
#include	<boost/asio.hpp>
#include	<boost/array.hpp>
#include	"ServerData.hh"

class TcpClient
{
public:
  TcpClient(boost::asio::io_service &, ServerData *, unsigned int);

  void	start();
  void	send(const std::string &);

  void	handleLine(const boost::system::error_code &, std::size_t);
  void	handleWrite(const boost::system::error_code &);

  void	handleParserError();
  void	handleParserConnect(const std::string &, const std::string &) {}
  void	handleParserCreateAccount(const std::string &, const std::string &) {}
  void	handleParserCallId(unsigned int) {}
  void	handleParserSetStatus(const std::string &) {}
  void	handleParserSendMessage(unsigned int, const std::string &) {}
  void	handleParserListContacts() {}
  void	handleParserAddContact(const std::string &) {}
  void	handleParserDeleteContact(unsigned int) {}
  void	handleParserAcceptCall(unsigned int) {}
  void	handleParserDeclineCall(unsigned int) {}
  void	handleParserGetMessages(unsigned int) {}

  boost::asio::ip::tcp::socket &getSocket() { return _socket; }

private:
  boost::asio::ip::tcp::socket	_socket;
  ServerData			*_data;
  bool				_isAuthenticated;
  unsigned int			_id;
  boost::asio::streambuf	_inBuffer;
  std::queue<std::string>	_outBuffer;
};

#endif
