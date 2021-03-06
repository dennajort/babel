#ifndef	__SIPHANDLER_HPP__
#define	__SIPHANDLER_HPP__

#include <QObject>
#include <QTcpSocket>
#include <QString>

enum                e_state
  {
    NONE = 0,
    CREATE = 1,
    CONNECT = 2,
    ADD_CONTACT = 3
  };

class	SipHandler : public QObject
{
  Q_OBJECT

public:
  SipHandler(QObject *parent = 0);
  ~SipHandler();
  bool          isConnected();
  void          setStatus(const unsigned int status);
  void          sendEndCall(const unsigned int id);

public slots:
  void		connectMe();
  void          disconnectMe();

private slots:
  void		readData();
  void		socketError(QAbstractSocket::SocketError socketError);
  void          handleCreateAccount(const QString &username, const QString &password);
  void          handleConnectUser(const QString &username, const QString &password, const QString &udpPort);
  void		handleListContacts();
  void		handleAddContact(const QString &contact);
  void		handleDeleteContact(const unsigned int id);
  void		handleCall(const unsigned int id);
  void          handleAcceptCall(const unsigned int id);
  void          handleDeclineCall(const unsigned int id);
  void		handleSendMessage(const unsigned int id, const QString &message);
  void		handleGetMessages(const unsigned int id);

signals:
  void		error(const QString error);
  void		displayMessage(const QString &message);
  void		registerError();
  void          clientConnected(const bool res);
  void		contact(const unsigned int id, const QString &username, const unsigned int status, const QString &mood);
  void          callRequest(const unsigned int id);
  void		contactIp(const unsigned int id, const QString &ip, quint16 port);
  void          declinedCall(const unsigned int id);
  void          endCall(const unsigned int id);
  void		addContactResult(const bool result);
  void		message(const unsigned int id, const QString &message, const QString &date);

private:
  void		tcpSend(const QString &message);
  void		getHelloInfos(const QStringList &stringList);
  void		handleError(const QStringList &stringList);
  void		handleContact(const QStringList &stringList);
  void          handleCallRequest(const QStringList &stringList);
  void		handleContactIp(const QStringList &stringList);
  void          handleDeclinedCall(const QStringList &stringList);
  void          handleEndCall(const QStringList &stringList);
  void		handleCreateResponse(const QStringList &stringList);
  void          handleConnectResponse(const QStringList &stringList);
  void		handleAddContactResponse(const QStringList &stringList);
  void		handleMessage(const QStringList &stringList);

  e_state	_state;
  QTcpSocket	*_socket;
  QString      	_id;
  QString	_hash;
};

#endif	/* __SIPHANDLER_HPP__ */
