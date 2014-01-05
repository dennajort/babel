#include <QSettings>
#include <QCryptographicHash>
#include <QStringList>
#include "SipProtocol.hpp"
#include "SipHandler.hpp"

SipHandler::SipHandler(QObject *parent) :
  QObject(parent),
  _state(NONE),
  _socket(new QTcpSocket(this))
{
  connect(_socket, SIGNAL(readyRead()),
	  this, SLOT(readData()));
  connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
	  this, SLOT(socketError(QAbstractSocket::SocketError)));
  connect(_socket, SIGNAL(disconnected()),
          parent, SLOT(disconnected()));
  connect(this, SIGNAL(error(QString)),
          parent, SLOT(disconnected()));

  connect(parent, SIGNAL(listContacts()),
	  this, SLOT(handleListContacts()));
  connect(parent, SIGNAL(addContact(const QString&)),
	  this, SLOT(handleAddContact(const QString&)));
  connect(parent, SIGNAL(deleteContact(const unsigned int)),
	  this, SLOT(handleDeleteContact(const unsigned int)));
  connect(parent, SIGNAL(call(const unsigned int)),
	  this, SLOT(handleCall(const unsigned int)));
  connect(parent, SIGNAL(acceptCall(const unsigned int)),
          this, SLOT(handleAcceptCall(const unsigned int)));
  connect(parent, SIGNAL(declineCall(const unsigned int)),
          this, SLOT(handleDeclineCall(const unsigned int)));
  connect(parent, SIGNAL(sendMessage(const unsigned int, const QString&)),
	  this, SLOT(handleSendMessage(const unsigned int, const QString&)));
  connect(parent, SIGNAL(getMessages(const unsigned int)),
	  this, SLOT(handleGetMessages(const unsigned int)));

  connect(this, SIGNAL(displayMessage(const QString&)),
          parent, SLOT(displayMessage(const QString&)));
  connect(this, SIGNAL(registerError()),
          parent, SLOT(registerError()));
  connect(this, SIGNAL(clientConnected(const bool)),
          parent, SLOT(clientConnected(const bool)));
  connect(this, SIGNAL(contact(const unsigned int, const QString&, const unsigned int, const QString&)),
          parent, SLOT(contact(const unsigned int, const QString&, const unsigned int, const QString&)));
  connect(this, SIGNAL(callRequest(const unsigned int)),
          parent, SLOT(callRequest(const unsigned int)));
  connect(this, SIGNAL(contactIp(const unsigned int, const QString&, quint16)),
          parent, SLOT(contactIp(const unsigned int, const QString&, quint16)));
  connect(this, SIGNAL(declinedCall(const unsigned int)),
          parent, SLOT(declinedCall(const unsigned int)));
  connect(this, SIGNAL(endCall()),
          parent, SLOT(endCall()));
  connect(this, SIGNAL(addContactResult(bool)),
          parent, SLOT(addContactResult(bool)));
  connect(this, SIGNAL(message(const unsigned int, const QString&, const QString&)),
          parent, SLOT(message(const unsigned int, const QString&, const QString&)));
}

SipHandler::~SipHandler()
{
  delete _socket;
}

bool SipHandler::isConnected()
{
  return (_socket->state() == QTcpSocket::ConnectedState);
}

void SipHandler::setStatus(const unsigned int status)
{
  QString st = QString::number(status);
  QSettings settings;
  QString mood = settings.value("account/mood", "I love CamelBabel !").toString();

  tcpSend(QString(SIP_SET_STATUS) + '\t' + st + '\t' + mood);
}

void SipHandler::sendEndCall()
{
  tcpSend(SIP_END_CALL);
}

void SipHandler::connectMe()
{
  QSettings	settings;

  _socket->abort();
  _socket->connectToHost(settings.value("account/server").toString(),
                         settings.value("account/port").toInt());
}

void SipHandler::disconnectMe()
{
  _socket->abort();
}

void SipHandler::readData()
{
  QString        line;

  while (_socket->canReadLine())
    {
      line = _socket->readLine();
      if (!line.isEmpty())
        {
          line.resize(line.length() - 1);
          QStringList        stringList = line.split('\t');
          if (!stringList.isEmpty())
            {
              if (stringList.size() == 3 && stringList[0] == SIP_HELLO)
		getHelloInfos(stringList);
	      else if (stringList.size() == 5 && stringList[0] == SIP_CONTACT)
		handleContact(stringList);
	      else if (stringList.size() == 2 && stringList[0] == SIP_CALL_REQUEST)
		handleCallRequest(stringList);
	      else if (stringList.size() == 4 && stringList[0] == SIP_CONTACT_IP)
		handleContactIp(stringList);
	      else if (stringList.size() == 2 && stringList[0] == SIP_DECLINED)
		handleDeclinedCall(stringList);
	      else if (stringList.size() == 1 && stringList[0] == SIP_END_CALL)
		handleEndCall();
	      else if (stringList.size() == 4 && stringList[0] == SIP_MESSAGE)
		handleMessage(stringList);
	      else if (_state == CREATE && stringList.size() == 3 && stringList[0] == SIP_RESP &&
		       (stringList[1] == SIP_SUCCESS || stringList[1] == SIP_UNAUTHORIZED))
		handleCreateResponse(stringList);
	      else if (_state == CONNECT && stringList.size() == 3 && stringList[0] == SIP_RESP &&
			 (stringList[1] == SIP_SUCCESS || stringList[1] == SIP_UNAUTHORIZED))
		handleConnectResponse(stringList);
	      else if (_state == ADD_CONTACT && stringList.size() == 3 && stringList[0] == SIP_RESP &&
		       (stringList[1] == SIP_SUCCESS || stringList[1] == SIP_UNAUTHORIZED))
		handleAddContactResponse(stringList);
	      else if (stringList.size() == 3 && stringList[0] == SIP_RESP && (stringList[1] == SIP_UNAUTHORIZED ||
						     stringList[1] == SIP_NOTFOUND ||
						     stringList[1] == SIP_ALREADYCONNECTED))
		handleError(stringList);
	    }
        }
    }
}

void SipHandler::socketError(QAbstractSocket::SocketError socketError)
{
  Q_UNUSED(socketError);
  emit error(_socket->errorString());
}

void SipHandler::handleCreateAccount(const QString &username, const QString &password)
{
  QCryptographicHash        sha1(QCryptographicHash::Sha1);

  sha1.addData(password.toLatin1().data());
  tcpSend(QString(SIP_CREATE_ACCOUNT) + '\t' + username + '\t' + sha1.result().toHex());
  _state = CREATE;
}

void SipHandler::handleConnectUser(const QString &username, const QString &password, const QString &udpPort)
{
  QCryptographicHash	sha1(QCryptographicHash::Sha1);
  QByteArray	       	byteArray;

  sha1.addData(password.toLatin1().data());
  byteArray = sha1.result().toHex();
  sha1.reset();
  byteArray += _hash;
  sha1.addData(byteArray);
  tcpSend(QString(SIP_CONNECT) + '\t' + username + '\t' + sha1.result().toHex() + '\t' + udpPort);
  _state = CONNECT;
}

void SipHandler::handleListContacts()
{
  tcpSend(SIP_LIST_CONTACTS);
}

void SipHandler::handleAddContact(const QString &contact)
{
  tcpSend(QString(SIP_ADD_CONTACT) + '\t' + contact);
  _state = ADD_CONTACT;
}

void SipHandler::handleDeleteContact(const unsigned int id)
{
  QString contactId = QString::number(id);

  tcpSend(QString(SIP_DELETE_CONTACT) + '\t' + contactId);
}

void SipHandler::handleCall(const unsigned int id)
{
  QString contactId = QString::number(id);

  tcpSend(QString(SIP_CALL) + '\t' + contactId);
}

void SipHandler::handleAcceptCall(const unsigned int id)
{
  QString contactId = QString::number(id);

  tcpSend(QString(SIP_ACCEPT_CALL) + '\t' + contactId);
}

void SipHandler::handleDeclineCall(const unsigned int id)
{
  QString contactId = QString::number(id);

  tcpSend(QString(SIP_DECLINE_CALL) + '\t' + contactId);
}

void SipHandler::handleSendMessage(const unsigned int id, const QString &message)
{
  QString contactId = QString::number(id);

  tcpSend(QString(SIP_SEND_MESSAGE) + '\t' + contactId + '\t' + message);
}

void SipHandler::handleGetMessages(const unsigned int id)
{
  QString contactId = QString::number(id);

  tcpSend(QString(SIP_GET_MESSAGES) + '\t' + contactId);
}

void SipHandler::tcpSend(const QString &message)
{
  QTextStream        socketInputStream(_socket);

  socketInputStream << message << endl;
}

void SipHandler::getHelloInfos(const QStringList &stringList)
{
  QSettings settings;

  _id = stringList[1];
  _hash = stringList[2];
  if (settings.value("account/register", false).toBool())
      handleCreateAccount(settings.value("account/username", "").toString(),
                          settings.value("account/password", "").toString());
  else
      handleConnectUser(settings.value("account/username", "").toString(),
                        settings.value("account/password", "").toString(),
                        settings.value("account/callPort", "4243").toString());
}

void SipHandler::handleError(const QStringList &stringList)
{
  emit displayMessage(stringList[2]);
}

void SipHandler::handleContact(const QStringList &stringList)
{
  emit contact(stringList[1].toUInt(), stringList[2], stringList[3].toUInt(), stringList[4]);
}

void SipHandler::handleCallRequest(const QStringList &stringList)
{
  emit callRequest(stringList[1].toUInt());
}

void SipHandler::handleContactIp(const QStringList &stringList)
{
  emit contactIp(stringList[1].toUInt(), stringList[2], stringList[3].toUInt());
}

void SipHandler::handleDeclinedCall(const QStringList &stringList)
{
  emit declinedCall(stringList[1].toUInt());
}

void SipHandler::handleEndCall()
{
  emit endCall();
}

void SipHandler::handleCreateResponse(const QStringList &stringList)
{
  _state = NONE;
  if (stringList[1] == SIP_UNAUTHORIZED)
    emit registerError();
  else
    {
      QSettings settings;
      handleConnectUser(settings.value("account/username", "").toString(),
                        settings.value("account/password", "").toString(),
                        settings.value("account/callPort", "4243").toString());
      settings.setValue("account/register", false);
    }
}

void SipHandler::handleConnectResponse(const QStringList &stringList)
{
  if (stringList[1] == SIP_SUCCESS)
      emit clientConnected(true);
  else
    {
      emit displayMessage(stringList[2]);
      emit clientConnected(false);
    }
  _state = NONE;
}

void SipHandler::handleAddContactResponse(const QStringList &stringList)
{
  if (stringList[1] == SIP_UNAUTHORIZED)
    emit addContactResult(false);
  else
    emit addContactResult(true);
  _state = NONE;
}

void SipHandler::handleMessage(const QStringList &stringList)
{
  emit message(stringList[1].toUInt(), stringList[2], stringList[3]);
}
