#include	<boost/bind.hpp>
#include	<boost/format.hpp>
#include	"TcpClient.hh"
#include	"ServerData.hh"

using namespace boost::asio;

TcpClient::Ptr	TcpClient::create(io_service &io)
{
  return TcpClient::Ptr(new TcpClient(io));
}

TcpClient::TcpClient(io_service &io) :
  _socket(io),
  _isAuthenticated(false),
  _id(42)
{
}

void	TcpClient::start()
{
  async_read_until(_socket, _inBuffer, '\n',
		   boost::bind(&TcpClient::handleLine, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  sendHello();
}

void	TcpClient::send(const std::string &data)
{
  bool	isWriting = !_outBuffer.empty();

  _outBuffer.push(data);
  if (isWriting)
    return;
  async_write(_socket, buffer(_outBuffer.front().data(), _outBuffer.front().length()),
	      boost::bind(&TcpClient::handleWrite, shared_from_this(), boost::asio::placeholders::error));
}

void	TcpClient::sendHello()
{
  send((boost::format("HELLO\t%1%\t%2%\n") % _id % "hash").str());
}

void	TcpClient::handleLine(const boost::system::error_code& error, std::size_t size)
{
  if (!error)
    {
      std::string	line((std::istreambuf_iterator<char>(&_inBuffer)), std::istreambuf_iterator<char>());

      std::cout << "Get: " << line;
      //_parser.parse(line);
      async_read_until(_socket, _inBuffer, '\n',
		       boost::bind(&TcpClient::handleLine, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
  else if (_isAuthenticated)
    ServerData::getInstance().removeClient(shared_from_this());
}

void	TcpClient::handleWrite(const boost::system::error_code& error)
{
  if (!error)
    {
      _outBuffer.pop();
      if (_outBuffer.empty())
	return;
      async_write(_socket, buffer(_outBuffer.front().data(), _outBuffer.front().length()),
		  boost::bind(&TcpClient::handleWrite, shared_from_this(), boost::asio::placeholders::error));      
    }
  else if (_isAuthenticated)
    ServerData::getInstance().removeClient(shared_from_this());
}

void	TcpClient::handleParserError()
{
  std::cerr << "Error Parser" << std::endl;
}

ip::tcp::socket	&TcpClient::getSocket()
{
  return _socket;
}
