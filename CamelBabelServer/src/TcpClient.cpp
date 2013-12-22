#include	<boost/bind.hpp>
#include	<boost/format.hpp>
#include	<cstdlib>
#include	"TcpClient.hh"
#include	"ServerData.hh"
#include	"Parser.hh"

using namespace boost::asio;

TcpClient::Ptr	TcpClient::create(io_service &io)
{
  return TcpClient::Ptr(new TcpClient(io));
}

TcpClient::TcpClient(io_service &io) :
  _socket(io),
  _isAuthenticated(false),
  _id(ServerData::getInstance().generateId()),
  _salt("")
{
  static const char hexdigits[] = "0123456789abcdef";

  for (int n = 0; n < 40; n++)
    _salt += hexdigits[rand() % 16];
}

TcpClient::~TcpClient()
{
  ServerData::getInstance().releaseId(_id);
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
  send((boost::format("HELLO\t%1%\t%2%\n") % _id % _salt).str());
}

void	TcpClient::sendResp(unsigned int code, const std::string &msg)
{
  send((boost::format("RESP\t%1%\t%2%\n") % code % msg).str());
}

void	TcpClient::handleLine(const boost::system::error_code& error, std::size_t size)
{
  if (!error)
    {
      std::string	line((std::istreambuf_iterator<char>(&_inBuffer)), std::istreambuf_iterator<char>());
      Parser		parser(shared_from_this());

      std::cout << "Get: " << line;
      parser.parse(line);
      async_read_until(_socket, _inBuffer, '\n',
		       boost::bind(&TcpClient::handleLine, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
  else
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
  else
    ServerData::getInstance().removeClient(shared_from_this());
}

ip::tcp::socket	&TcpClient::getSocket()
{
  return _socket;
}

bool	TcpClient::isAuthenticated() const
{
  return _isAuthenticated;
}

const std::string	&TcpClient::getSalt() const
{
  return _salt;
}

void	TcpClient::setAuthenticated(bool authenticated)
{
  _isAuthenticated = authenticated;
}

