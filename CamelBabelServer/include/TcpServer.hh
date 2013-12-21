#ifndef __TCP_SERVER_HH__
#define __TCP_SERVER_HH__

#include	<boost/asio.hpp>
#include	"TcpClient.hh"

class TcpServer
{
public:
  TcpServer(boost::asio::io_service &);

private:
  void	startAccept();
  void	handleAccept(TcpClient::Ptr , const boost::system::error_code &);

  boost::asio::ip::tcp::acceptor	_acceptor;
};

#endif
