#ifndef __TCP_CLIENT_HH__
#define __TCP_CLIENT_HH__

#include	<string>
#include	<queue>
#include        <boost/shared_ptr.hpp>
#include        <boost/enable_shared_from_this.hpp>
#include	<boost/asio.hpp>
#include	<boost/array.hpp>
#include	<mongo/client/dbclient.h>

class Parser;

class TcpClient : public boost::enable_shared_from_this<TcpClient>
{
public:
  typedef boost::shared_ptr<TcpClient>	Ptr;

  static Ptr	create(boost::asio::io_service &);

  ~TcpClient();

  void	start();
  void	send(const std::string &);
  void	sendLine(const std::string &);

  // Protocol abstraction
  void	sendHello();
  void	sendResp(unsigned int, const std::string &);
  void	sendContact(unsigned int, const std::string &, unsigned int, const std::string &);
  void	sendCall(unsigned int);
  void	sendContactIp(Ptr);
  void	sendContactIp(unsigned int, const std::string &, unsigned int);
  void	sendDeclinedCall(unsigned int);
  void	sendMessage(unsigned int, const std::string &, unsigned long long);

  // Asio handlers
  void	handleLine(const boost::system::error_code &, std::size_t);
  void	handleWrite(const boost::system::error_code &);

  // Getters
  boost::asio::ip::tcp::socket	&getSocket();
  bool				isAuthenticated() const;
  const std::string		&getSalt() const;
  const mongo::OID		&getOID() const;
  unsigned int			getID() const;
  unsigned int			getState() const;
  const std::string		&getMood() const;
  const std::string		&getUsername() const;
  std::string			getIP() const;
  unsigned int			getPort() const;

  // Setters
  void				setAuthenticated(bool);
  void				setOID(const mongo::OID &);
  void				setState(unsigned int);
  void				setMood(const std::string &);
  void				setUsername(const std::string &);
  void				setPort(unsigned int);

private:
  TcpClient(boost::asio::io_service &);

  boost::asio::ip::tcp::socket	_socket;
  bool				_isAuthenticated;
  unsigned int			_id;
  std::string			_salt;
  std::string			_mood;
  unsigned int			_state;
  mongo::OID			_oid;
  boost::asio::streambuf	_inBuffer;
  std::queue<std::string>	_outBuffer;
  std::string			_username;
  unsigned int			_port;
};

#endif
