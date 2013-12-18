#ifndef	__SIPHANDLER_HPP__
#define	__SIPHANDLER_HPP__

#include <QObject>
#include <QTcpSocket>
#include <QMainWindow>

enum                e_state
  {
    NONE = 0,
    CREATE = 1,
    ADD_CONTACT = 2
  };

class	SipHandler : public QObject
{
  Q_OBJECT
public:
  SipHandler(QMainWindow *window);
  ~SipHandler();
public slots:
  void		connectMe();
private slots:
  void		readData();
  void		socketError(QAbstractSocket::SocketError socketError);
  void		handleCreateAccount(const QString &username, const QString &password);
  void		handleConnectUser(const QString &username, const QString &password);
  void		handleSetStatus(const QString &status);
  void		handleListContacts();
  void		handleAddContact(const QString &contact);
  void		handleDeleteContact(const unsigned int id);
  void		handleCall(const unsigned int id);
  void		handleSendMessage(const unsigned int id, const QString &message);
  void		handleGetMessages(const unsigned int id);
signals:
  void		error(const QString error);
  void		displayMessage(const QString &message);
  void		createResult(const bool result);
  void		contact(const unsigned int id, const QString &username, const QString &status);
  void		contactIp(const unsigned int id, const QString &ip);
  void		addContactResult(const bool result);
  void		message(const unsigned int id, const QString &message, const QString &date);
private:
  void		tcpSend(const QString &message);
  void		getHelloInfos(const QStringList &stringList);
  void		handleError(const QStringList &stringList);
  void		handleContact(const QStringList &stringList);
  void		handleContactIp(const QStringList &stringList);
  void		handleCreateResponse(const QStringList &stringList);
  void		handleAddContactResponse(const QStringList &stringList);
  void		handleMessage(const QStringList &stringList);

  e_state	_state;
  QTcpSocket	*_socket;
  QString      	_id;
  QString	_hash;
};

#endif	/* __SIPHANDLER_HPP__ */
