#include <boost/lexical_cast.hpp>
#include "Parser.hh"

Parser::Parser(TcpClient *client)
  : _client(client)
{

}


Parser::~Parser()
{

}

void
Parser::parse(const std::string &line)
{
  _line = line;
  _i = _line.begin();
  switch (*_i)
    {
    case 'C':
      return caseFirstC();
    case 'S':
      return caseFirstS();
    case 'L':
      return caseListContacts();
    case 'A':
      return caseFirstA();
    case 'D':
      return caseFirstD();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseFirstS()
{
  _i++;
  switch (*_i)
    {
    case 'E':
      return caseSecondE();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseFirstA()
{
  _i++;
  switch (*_i)
    {
    case 'D':
      return caseAddContact();
    case 'C':
      return caseAcceptCall();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseSecondE()
{
  _i++;
  switch (*_i)
    {
    case 'T':
      return caseSetStatus();
    case 'N':
      return caseSendMessage();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseFirstC()
{
  _i++;
  switch (*_i)
    {
    case 'O':
      return caseConnect();
    case 'R':
      return caseCreateAccount();
    case 'A':
      return caseCall();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseFirstD()
{
  _i++;
  switch (*_i)
    {
    case 'E':
      return caseFirstDE();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseFirstDE()
{
  _i++;
  switch (*_i)
    {
    case 'L':
      return caseDeleteContact();
    case 'C':
      return caseDeclineCall();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseConnect()
{
  std::string	tmp("NNECT\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseConnectUsername();
}

void
Parser::caseConnectUsername()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      return _client->handleParserError();
    case '\t':
      if (_buff1.empty())
        return _client->handleParserError();
      _buff2 = "";
      return caseConnectPassword();
    default:
      _buff1 += *_i;
      return caseConnectUsername();
    }
}

void
Parser::caseConnectPassword()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff2.empty())
        return _client->handleParserError();
      return _client->handleParserConnect(_buff1, _buff2);
    case '\t':
      return _client->handleParserError();
    default:
      _buff2 += *_i;
      return caseConnectPassword();
    }
}

void
Parser::caseCreateAccount()
{
  std::string	tmp("EATE_ACCOUNT\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseCreateAccountUsername();
}

void
Parser::caseCreateAccountUsername()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      return _client->handleParserError();
    case '\t':
      if (_buff1.empty())
        return _client->handleParserError();
      _buff2 = "";
      return caseCreateAccountPassword();
    default:
      _buff1 += *_i;
      return caseCreateAccountUsername();
    }
}

void
Parser::caseCreateAccountPassword()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff2.empty())
        return _client->handleParserError();
      return _client->handleParserCreateAccount(_buff1, _buff2);
    case '\t':
      return _client->handleParserError();
    default:
      _buff2 += *_i;
      return caseCreateAccountPassword();
    }
}

void
Parser::caseCall()
{
  std::string	tmp("LL\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseCallId();
}

void
Parser::caseCallId()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff1.empty())
        return _client->handleParserError();
      return _client->handleParserCallId(boost::lexical_cast<unsigned int>(_buff1));
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      _buff1 += *_i;
      return caseCallId();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseSetStatus()
{
  std::string	tmp("T_STATUS\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseSetStatusNewStatus();
}

void
Parser::caseSetStatusNewStatus()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff1.empty())
        return _client->handleParserError();
      return _client->handleParserSetStatus(_buff1);
    case '\t':
      return _client->handleParserError();
    default:
      _buff1 += *_i;
      return caseSetStatusNewStatus();
    }
}

void
Parser::caseSendMessage()
{
  std::string	tmp("ND_MESSAGE\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseSendMessageToId();
}

void
Parser::caseSendMessageToId()
{
  _i++;
  switch (*_i)
    {
    case '\t':
      if (_buff1.empty())
        return _client->handleParserError();
      _buff2 = "";
      return caseSendMessageMessage();
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      _buff1 += *_i;
      return caseSendMessageToId();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseSendMessageMessage()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff2.empty())
        return _client->handleParserError();
      return _client->handleParserSendMessage(boost::lexical_cast<unsigned int>(_buff1), _buff2);
    case '\t':
      return _client->handleParserError();
    default:
      _buff2 += *_i;
      return caseSendMessageMessage();
    }
}

void
Parser::caseListContacts()
{
  std::string	tmp("IST_CONTACTS\n");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  return _client->handleParserListContacts();
}
 
void
Parser::caseAddContact()
{
  std::string	tmp("D_CONTACT\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseAddContactUsername();
}

void
Parser::caseAddContactUsername()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff1.empty())
        return _client->handleParserError();
      return _client->handleParserAddContact(_buff1);
    case '\t':
      return _client->handleParserError();
    default:
      _buff1 += *_i;
      return caseAddContactUsername();
    }
}

void
Parser::caseDeleteContact()
{
  std::string	tmp("LETE_CONTACT\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseDeleteContactId();
}

void
Parser::caseDeleteContactId()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff1.empty())
        return _client->handleParserError();
      return _client->handleParserDeleteContact(boost::lexical_cast<unsigned int>(_buff1));
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      _buff1 += *_i;
      return caseDeleteContactId();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseDeclineCall()
{
  std::string	tmp("CLINE_CALL\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseDeclineCallId();
}

void
Parser::caseDeclineCallId()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff1.empty())
        return _client->handleParserError();
      return _client->handleParserDeclineCall(boost::lexical_cast<unsigned int>(_buff1));
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      _buff1 += *_i;
      return caseDeclineCallId();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseAcceptCall()
{
  std::string	tmp("CEPT_CALL\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseDeclineCallId();
}

void
Parser::caseAcceptCallId()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff1.empty())
        return _client->handleParserError();
      return _client->handleParserAcceptCall(boost::lexical_cast<unsigned int>(_buff1));
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      _buff1 += *_i;
      return caseAcceptCallId();
    default:
      return _client->handleParserError();
    }
}

void
Parser::caseGetMessages()
{
  std::string	tmp("ET_MESSAGES\t");

  for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
      _i++;
      if (*i != *_i)
        return _client->handleParserError();
    }
  _buff1 = "";
  return caseGetMessagesId();
}

void
Parser::caseGetMessagesId()
{
  _i++;
  switch (*_i)
    {
    case '\n':
      if (_buff1.empty())
        return _client->handleParserError();
      return _client->handleParserGetMessages(boost::lexical_cast<unsigned int>(_buff1));
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      _buff1 += *_i;
      return caseGetMessagesId();
    default:
      return _client->handleParserError();
    }
}