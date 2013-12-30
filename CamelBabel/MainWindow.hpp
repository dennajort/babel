#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QListWidget>
#include <QCloseEvent>
#include "SipHandler.hpp"
#include "RTPCallManager.hpp"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit      MainWindow(QWidget *parent = 0);
  virtual       ~MainWindow();

public slots:
  void		aboutToQuit();
  void		iconActivated(QSystemTrayIcon::ActivationReason reason);
  void		changeMainWindowState();
  void		about();
  void          settings();
  void		addContact();
  void		deleteContact();
  void          contactSelected();
  void          callStarted();
  void          callFinished();
  void          displayMessage(const QString &message);
  void          clientConnected(const bool res);
  void          disconnected();
  void          registerError();
  void		contact(const unsigned int id, const QString &username,
			const unsigned int status, const QString &mood);
  void          callRequest(const unsigned int id);
  void          addContactResult(bool res);
  void          changeStatus(int index);
  void          sendMessageToCurrent(const QString &message);

signals:
  void          setStatus(const unsigned int status, const QString &mood);
  void          addContact(const QString &contact);
  void          deleteContact(const unsigned int id);
  void          listContacts();
  void          call(const unsigned int id);
  void          acceptCall(const unsigned int id);
  void          declineCall(const unsigned int id);
  void          changeCallButton(bool);
  void          getMessages(const unsigned int id);
  void          sendMessage(const unsigned int id, const QString &message);

protected:
  virtual void	closeEvent(QCloseEvent *event);

private:
  void          addChat(const unsigned int id, const QString &contact, const unsigned int status);
  QListWidgetItem *getContactById(const unsigned int id);
  bool          contactAlreadyAdded(const QString &contact) const;
  void          moveListItemToPos(const int fromPos, const int toPos);
  void          moveChatWidgetToPos(const int fromPos, const int toPos);
  void          moveContactToPos(const QString &contact, const int pos);
  void		createTrayIcon();
  void		writeSettings();
  void		readSettings();

  Ui::MainWindow          *_ui;

  QIcon                   _availableImg;
  QIcon                   _awayImg;
  QIcon                   _doNotDisturbImg;
  QIcon                   _offlineImg;

  QSystemTrayIcon         *_trayIcon;
  QMenu                   *_trayIconMenu;

  QString                  _me;

  bool                    _inCall;

  RTPCallManager          *_rtpCallManager;
  SipHandler              *_sipHandler;
};

#endif // MAINWINDOW_HPP
