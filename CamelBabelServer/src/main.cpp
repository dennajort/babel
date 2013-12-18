#include	<boost/asio.hpp>
#include	<iostream>
#include	"TcpServer.hh"

int	main(int ac, char **av)
{
  try
    {
      boost::asio::io_service	io;
      TcpServer			server(io);

      io.run();
    }
  catch (std::exception &e)
    {
      std::cerr << "Exception: " << e.what() << std::endl;
    }
  return 0;
}
