#ifndef __TCP_SERVER_HH__
#define __TCP_SERVER_HH__

#include	<boost/asio.hpp>

class TcpServer
{
public:
  TcpServer(boost::asio::io_service &);

private:
  void	startAccept();
  void	handleAccept(TcpClient::pointer, const boost::system::error_code &);

  boost::asio::ip::tcp::acceptor _acceptor;
};

#endif
