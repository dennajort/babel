#ifndef	__PARSER_HH__
#define	__PARSER_HH__

#include <string>

class	Parser
{
public:
  Parser(TcpClientPtr);
  ~Parser();

  void	parse(const std::string &);
private:
  void	caseFirstC();
  void	caseFirstS();
  void	caseFirstA();
  void	caseSecondE();
  void	caseFirstD();
  void	caseFirstDE();

  void	caseConnect();
  void	caseConnectUsername();
  void	caseConnectPassword();

  void	caseCreateAccount();
  void	caseCreateAccountUsername();
  void	caseCreateAccountPassword();

  void	caseCall();
  void	caseCallId();

  void	caseSetStatus();
  void	caseSetStatusNewStatus();
  void	caseSendMessage();
  void	caseSendMessageToId();

  void	caseListContacts();

  void	caseAddContact();
  void	caseAddContactUsername();

  void	caseAcceptCall();
  void	caseAcceptCallId();

  void	caseDeleteContact();

  void	caseDeclineCall();
  void	caseDeclineCallId();

  void	caseGetMessages();

  std::string			_line;
  std::string::const_iterator	_i;
  TcpClientPtr			_client;
  std::string			_buff2;
  std::string			_buff1;
};

#endif
