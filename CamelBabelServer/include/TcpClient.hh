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
  void	sendResp(unsigned int, const std::string &);

  // Asio handlers
  void	handleLine(const boost::system::error_code &, std::size_t);
  void	handleWrite(const boost::system::error_code &);

  // Getters
  boost::asio::ip::tcp::socket	&getSocket();
  bool				isAuthenticated() const;
  const std::string		&getSalt() const;

  // Setters
  void				setAuthenticated(bool);

private:
  TcpClient(boost::asio::io_service &);

  boost::asio::ip::tcp::socket	_socket;
  bool				_isAuthenticated;
  unsigned int			_id;
  std::string			_salt;
  boost::asio::streambuf	_inBuffer;
  std::queue<std::string>	_outBuffer;
};

#endif
