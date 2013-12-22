#include	<boost/asio.hpp>
#include	<cstdlib>
#include	"TcpServer.hh"
#include	"ServerData.hh"

int	main(int ac, char **av)
{
  srand(time(NULL));
  ServerData::getInstance().connectToMongo();
  try
    {
      boost::asio::io_service	io;
      TcpServer			server(io);

      io.run();
    }
  catch (const std::exception &e)
    {
      std::cerr << "Catch: " << e.what() << std::endl;
      return 1;
    }
  return 0;
}
