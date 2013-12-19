#include <QSettings>
#include <QCryptographicHash>
#include "SipProtocol.hpp"
#include "SipHandler.hpp"

SipHandler::SipHandler(QWidget *window)
  : _state(NONE)
{
  qDebug() << "SipHandler";
  _socket = new QTcpSocket(this);
  connect(_socket, SIGNAL(readyRead()),
	  this, SLOT(readData()));
  connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
	  this, SLOT(socketError(QAbstractSocket::SocketError)));
  connect(_socket, SIGNAL(disconnected()),
	  window, SLOT(disconnected()));


  connect(window, SIGNAL(setStatus(const QString&)),
	  this, SLOT(handleSetStatus(const QString&)));
  connect(window, SIGNAL(listContacts()),
	  this, SLOT(handleListContacts()));
  connect(window, SIGNAL(addContact(const QString&)),
	  this, SLOT(handleAddContact(const QString&)));
  connect(window, SIGNAL(deleteContact(const unsigned int)),
	  this, SLOT(handleDeleteContact(const unsigned int)));
  connect(window, SIGNAL(call(const unsigned int)),
	  this, SLOT(handleCall(const unsigned int)));
  connect(window, SIGNAL(sendMessage(const unsigned int, const QString&)),
	  this, SLOT(handleSendMessage(const unsigned int, const QString&)));
  connect(window, SIGNAL(getMessages(const unsigned int)),
	  this, SLOT(handleGetMessages(const unsigned int)));

  //connect(this, SIGNAL(error(QString)), window, SLOT());
  //connect(this, SIGNAL(displayMessage(const QString&)), window, SLOT());
  //connect(this, SIGNAL(registerError()), window, SLOT());
  connect(this, SIGNAL(clientConnected(const bool)), window, SLOT(clientConnected(const bool)));
  //connect(this, SIGNAL(contact(const unsigned int, const QString&, const QString&)), window, SLOT());
  //connect(this, SIGNAL(contactIp(const unsigned int, const QString&)), window, SLOT());
  //connect(this, SIGNAL(addContactResult(bool)), window, SLOT());
  //connect(this, SIGNAL(message(const unsigned int, const QString&, const QString&)), window, SLOT());
}

SipHandler::~SipHandler()
{
  delete _socket;
}

void SipHandler::connectMe()
{
  QSettings	settings;

  _socket->abort();
  _socket->connectToHost(settings.value("account/server").toString(),
			 settings.value("account/port").toInt());
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
	      if (stringList[0] == SIP_HELLO)
		getHelloInfos(stringList);
	      else if (stringList[0] == SIP_CONTACT)
		handleContact(stringList);
	      else if (stringList[0] == SIP_CONTACT_IP)
		handleContactIp(stringList);
	      else if (stringList[0] == SIP_MESSAGE)
		handleMessage(stringList);
	      else if (_state == CREATE && stringList[0] == SIP_RESP &&
		       (stringList[1] == SIP_SUCCESS || stringList[1] == SIP_UNAUTHORIZED))
		handleCreateResponse(stringList);
	      else if (_state == CONNECT && stringList[0] == SIP_RESP &&
			 (stringList[1] == SIP_SUCCESS || stringList[1] == SIP_UNAUTHORIZED))
		handleConnectResponse(stringList);
	      else if (_state == ADD_CONTACT && stringList[0] == SIP_RESP &&
		       (stringList[1] == SIP_SUCCESS || stringList[1] == SIP_UNAUTHORIZED))
		handleAddContactResponse(stringList);
	      else if (stringList[0] == SIP_RESP && (stringList[1] == SIP_UNAUTHORIZED ||
						     stringList[1] == SIP_NOTFOUND ||
						     stringList[1] == SIP_ALREADYCONNECTED))
		handleError(stringList);
	    }
        }
    }
}

void SipHandler::socketError(QAbstractSocket::SocketError socketError)
{
  (void)socketError;
  emit error(_socket->errorString());
}

void SipHandler::handleCreateAccount(const QString &username, const QString &password)
{
  QCryptographicHash        sha1(QCryptographicHash::Sha1);

  qDebug() << username;
  qDebug() << password;
  sha1.addData(password.toLatin1().data());
  tcpSend(QString(SIP_CREATE_ACCOUNT) + '\t' + username + '\t' + sha1.result().toHex());
  _state = CREATE;
}

void SipHandler::handleConnectUser(const QString &username, const QString &password)
{
  QCryptographicHash	sha1(QCryptographicHash::Sha1);
  QByteArray	       	byteArray;

  sha1.addData(password.toLatin1().data());
  byteArray = sha1.result();
  sha1.reset();
  byteArray += password;
  sha1.addData(byteArray);
  tcpSend(QString(SIP_CONNECT) + '\t' + username + '\t' + sha1.result().toHex());
  _state = CONNECT;
}

void SipHandler::handleSetStatus(const QString &status)
{
  tcpSend(QString(SIP_SET_STATUS) + '\t' + status);
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
      handleCreateAccount(settings.value("username", "").toString(), settings.value("password", "").toString());
  else
      handleConnectUser(settings.value("username", "").toString(), settings.value("password", "").toString());
}

void SipHandler::handleError(const QStringList &stringList)
{
  emit displayMessage(stringList[2]);
}

void SipHandler::handleContact(const QStringList &stringList)
{
  emit contact(stringList[1].toUInt(), stringList[2], stringList[3]);
}

void SipHandler::handleContactIp(const QStringList &stringList)
{
  emit contactIp(stringList[1].toUInt(), stringList[2]);
}

void SipHandler::handleCreateResponse(const QStringList &stringList)
{
  if (stringList[1] == SIP_UNAUTHORIZED)
    emit registerError();
  else
    {
      QSettings settings;
      handleConnectUser(settings.value("username", "").toString(), settings.value("password", "").toString());
      _state = NONE;
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
      //ajouter slots/signaux pour clientConnected, le catcher a main windows dans une methode dédiée
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
