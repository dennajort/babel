#include	<boost/bind.hpp>
#include	"TcpClient.hh"

using namespace boost::asio;

TcpClient::TcpClient(io_service &io, ServerData *serverData, unsigned int id) :
  _socket(io),
  _data(serverData),
  _isAuthenticated(false),
  _id(id)
{
}

void	TcpClient::start()
{
  async_read_until(_socket, _inBuffer, '\n',
		   boost::bind(&TcpClient::handleLine, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void	TcpClient::send(const std::string &data)
{
  bool	isWriting = !_outBuffer.empty();

  _outBuffer.push(data);
  if (isWriting)
    return;
  async_write(_socket, buffer(_outBuffer.front().data(), _outBuffer.front().length()),
	      boost::bind(&TcpClient::handleWrite, this, boost::asio::placeholders::error));
}

void	TcpClient::handleLine(const boost::system::error_code& error, std::size_t size)
{
  if (!error)
    {
      std::string	line((std::istreambuf_iterator<char>(&_inBuffer)), std::istreambuf_iterator<char>());

      std::cout << "Get: " << line;
      //_parser.parse(line);
      async_read_until(_socket, _inBuffer, '\n',
		       boost::bind(&TcpClient::handleLine, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
  else if (_isAuthenticated)
    _data->removeClient(this);
  else
    delete this;
}

void	TcpClient::handleWrite(const boost::system::error_code& error)
{
  if (!error)
    {
      _outBuffer.pop();
      if (_outBuffer.empty())
	return;
      async_write(_socket, buffer(_outBuffer.front().data(), _outBuffer.front().length()),
		  boost::bind(&TcpClient::handleWrite, this, boost::asio::placeholders::error));      
    }
  else if (_isAuthenticated)
    _data->removeClient(this);
  else
    delete this;
}

void	TcpClient::handleParserError()
{
  std::cerr << "Error Parser" << std::endl;
}
