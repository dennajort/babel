#include	<boost/bind.hpp>
#include	"TcpServer.hh"

using namespace boost::asio;

TcpServer::TcpServer(io_service &io) :
  _acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 4242))
{
  startAccept();
}

void	TcpServer::startAccept()
{
  TcpClient *newClient = new TcpClient(_acceptor.get_io_service(), &_data, 42);

  _acceptor.async_accept(newClient->getSocket(), boost::bind(&TcpServer::handleAccept, this, newClient, placeholders::error));
}

void	TcpServer::handleAccept(TcpClient *newClient, const boost::system::error_code &error)
{
  if (!error)
    newClient->start();
  startAccept();
}
