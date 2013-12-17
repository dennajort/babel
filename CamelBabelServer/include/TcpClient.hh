#ifndef __TCP_CLIENT_HH__
#define __TCP_CLIENT_HH__

#include	<string>
#include	<boost/shared_ptr.hpp>
#include	<boost/enable_shared_from_this.hpp>
#include	<boost/asio.hpp>
#include	<boost/array.hpp>
#include	"ServerData.hh"

class TcpClient : public boost::enable_shared_from_this<TcpClient>
{
public:
  typedef boost::shared_ptr<TcpClient> Ptr;

  static Ptr	create(boost::asio::io_service &, ServerData::Ptr &);

  void	start();
  void	send(const std::string &);

  void	handleLine(const boost::system::error_code &);
  void	handleWrite(const boost::system::error_code &);

  boost::asio::ip::tcp::socket &getSocket() { return _socket; }

private:
  TcpClient(boost::asio::io_service &);

  ServerData::Ptr		_data;
  boost::asio::ip::tcp::socket	_socket;
  boost::array<char, 1024>	_inBuffer;
  std::queue<std::string>	_outBuffer;
}

#endif
