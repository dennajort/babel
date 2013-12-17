#include	<boost/bind.hpp>
#include	"TcpClient.hpp"

using namespace boost::asio;

static TcpClient::Ptr	create(io_service &io, ServerData::Ptr &serverData)
{
  return TcpClient::Ptr(new TcpClient(io, serverData));
}

TcpClient::TcpClient(io_service &io, ServerData::Ptr &serverData) :
  _data(serverData)
{
}

void	TcpClient::start()
{
  _data.join(shared_from_this());
  async_read_until(_socket, buffer(_inBuffer, _inBuffer.size()), '\n',
		   boost::bind(&TcpClient::handleLine, shared_from_this(), boost::asio::placeholders::error));
}

void	TcpClient::send(const std::string &data)
{
  bool	isWriting = !_outBuffer.empty();

  _outBuffer.push_back(data);
  if (isWriting)
    return;
  async_write(_socket, buffer(_outBuffer.front().data(), _outBuffer.front().length()),
	      boost::bind(&TcpClient::handleClient, shard_from_this(), boost::asio::placeholders::error));
}

void	TcpClient::handle
