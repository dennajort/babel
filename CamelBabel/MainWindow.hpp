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

signals:
  void          changeCallButton(bool);

protected:
  virtual void	closeEvent(QCloseEvent *event);

private:
  void          addChat(const QString &contact);
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
  QIcon                   _offlineImg;

  QSystemTrayIcon         *_trayIcon;
  QMenu                   *_trayIconMenu;

  QString                  _me;

  bool                    _inCall;

  RTPCallManager          *_rtpCallManager;
  SipHandler		*_sipHandler;
};

#endif // MAINWINDOW_HPP
