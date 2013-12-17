#include	<boost/asio.hpp>
#include	"TcpServer.hh"

int	main()
{
  boost::asio::io_service	io;
  TcpServer			server(io);

  io.run();
  return 0;
}
