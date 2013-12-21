#ifndef __TCP_CLIENT_HH__
#define __TCP_CLIENT_HH__

#include	<string>
#include	<queue>
#include        <boost/shared_ptr.hpp>
#include        <boost/enable_shared_from_this.hpp>
#include	<boost/asio.hpp>
#include	<boost/array.hpp>

class Parser;

class TcpClient : public boost::enable_shared_from_this<TcpClient>
{
public:
  typedef boost::shared_ptr<TcpClient>	Ptr;

  static Ptr	create(boost::asio::io_service &);

  ~TcpClient();

  void	start();
  void	send(const std::string &);

  // Protocol abstraction
  void	sendHello();

  // Asio handlers
  void	handleLine(const boost::system::error_code &, std::size_t);
  void	handleWrite(const boost::system::error_code &);

  // Parser handlers
  void	handleParserError();
  void	handleParserConnect(const std::string &, const std::string &) {}
  void	handleParserCreateAccount(const std::string &, const std::string &) {}
  void	handleParserCallId(unsigned int) {}
  void	handleParserSetStatus(unsigned int, const std::string &) {}
  void	handleParserSendMessage(unsigned int, const std::string &) {}
  void	handleParserListContacts() {}
  void	handleParserAddContact(const std::string &) {}
  void	handleParserDeleteContact(unsigned int) {}
  void	handleParserAcceptCall(unsigned int) {}
  void	handleParserDeclineCall(unsigned int) {}
  void	handleParserGetMessages(unsigned int) {}

  // Getters
  boost::asio::ip::tcp::socket &getSocket();

private:
  TcpClient(boost::asio::io_service &);

  boost::asio::ip::tcp::socket	_socket;
  bool				_isAuthenticated;
  unsigned int			_id;
  Parser			*_parser;
  boost::asio::streambuf	_inBuffer;
  std::queue<std::string>	_outBuffer;
};

#endif
