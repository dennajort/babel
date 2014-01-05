#ifndef	__PARSER_HH__
#define	__PARSER_HH__

#include <string>
#include "TcpClient.hh"

class	Parser
{
public:
  Parser(TcpClient::Ptr);
  ~Parser();

  void	parse(const std::string &);
private:
  void	caseFirstC();
  void	caseFirstS();
  void	caseFirstA();
  void	caseSecondE();
  void	caseFirstD();
  void	caseFirstDE();

  void	caseEndCall();
  void	caseEndCallContactId();

  void	caseConnect();
  void	caseConnectUsername();
  void	caseConnectPassword();
  void	caseConnectUdpPort();

  void	caseCreateAccount();
  void	caseCreateAccountUsername();
  void	caseCreateAccountPassword();

  void	caseCall();
  void	caseCallId();

  void	caseSetStatus();
  void	caseSetStatusNewStatusId();
  void	caseSetStatusNewStatusMood();
  void	caseSendMessage();
  void	caseSendMessageToId();
  void	caseSendMessageMessage();

  void	caseListContacts();

  void	caseAddContact();
  void	caseAddContactUsername();

  void	caseAcceptCall();
  void	caseAcceptCallId();

  void	caseDeleteContact();
  void	caseDeleteContactId();

  void	caseDeclineCall();
  void	caseDeclineCallId();

  void	caseGetMessages();
  void	caseGetMessagesId();

  std::string			_line;
  std::string::const_iterator	_i;
  TcpClient::Ptr		_client;
  std::string			_buff3;
  std::string			_buff2;
  std::string			_buff1;
};

#endif
