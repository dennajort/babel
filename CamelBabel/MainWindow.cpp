#include <QMessageBox>
#include <QSettings>
#include <QInputDialog>
#include <QVariant>
#include <QDebug>
#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "SettingsDialog.hpp"
#include "ChatWidget.hpp"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  _ui(new Ui::MainWindow),
  _availableImg(":/images/available.png"),
  _awayImg(":/images/away.png"),
  _offlineImg(":/images/offline.png"),
  _inCall(false),
  _rtpCallManager(new RTPCallManager(this)),
  _sipHandler(new SipHandler(this))
{
  _ui->setupUi(this);
  readSettings();
  createTrayIcon();
  _ui->statusBar->showMessage("Disconnected");
}

MainWindow::~MainWindow()
{
  QSettings settings;
  if (!settings.value("account/savePassword", true).toBool())
    settings.setValue("account/password", "");
  delete _sipHandler;
  delete _rtpCallManager;
  delete _ui;
}

void MainWindow::aboutToQuit()
{
  writeSettings();
  qApp->quit();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::Trigger
      || reason == QSystemTrayIcon::DoubleClick)
    changeMainWindowState();
}

void MainWindow::changeMainWindowState()
{
  setVisible(!isVisible());
}

void MainWindow::about()
{
  QMessageBox::about(this, "CamelBabel About",
                     "CamelBabel is a VOIP Client\n"        \
                     "CamelCorp is a camel company\n"                                 \
                     "All camel reserved to post_l, farcy_b, gossel_j, lingua_a, teisse_l, zanchi_r");
}

void MainWindow::settings()
{
  SettingsDialog settingsDlg(this);
  settingsDlg.exec();
}

void MainWindow::addContact()
{
  QString	contact = QInputDialog::getText(this, "Add Contact", "Username:");

  //if (!contact.isEmpty() && !contactAlreadyAdded(contact))
    //addChat(contact);
}

void MainWindow::deleteContact()
{
  QListWidgetItem	*item = _ui->contactList->currentItem();

  if (item)
    {
      int ret = QMessageBox::warning(this, "CamelBabel", "You are about to remove " + item->text()
                                     + " from your contact list.\n"	\
                                     "Do you want to save the modification?",
                                     QMessageBox::Cancel | QMessageBox::Save);
      if (ret == QMessageBox::Save)
        {
          QWidget		*widget = _ui->chatStack->currentWidget();
          _ui->contactList->takeItem(_ui->contactList->row(item));
          _ui->chatStack->removeWidget(widget);
          delete item;
          delete widget;
        }
    }
}

void MainWindow::contactSelected()
{
  _ui->chatStack->setCurrentIndex(_ui->contactList->currentRow());
}

void MainWindow::callStarted()
{
  _inCall = true;
  emit changeCallButton(false);
}

void MainWindow::callFinished()
{
  _inCall = false;
  emit changeCallButton(true);
}

void MainWindow::clientConnected(const bool res)
{
  qDebug() << "client connected" << res;
}

void MainWindow::contact(const unsigned int id, const QString &username, const unsigned int status, const QString &mood)
{
  (void)mood;
  addChat(id, username, status);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (_trayIcon->isVisible())
    {
      event->ignore();
      hide();
    }
  else
    {
      event->accept();
      aboutToQuit();
    }
}

void MainWindow::addChat(const unsigned int id, const QString &contact, const unsigned int status)
{
  (void)status;
  QListWidgetItem       *item = NULL;

  for (int i = 1; i < _ui->contactList->count(); ++i)
    {
      QListWidgetItem *tmp = _ui->contactList->item(i);
      if (tmp->data(Qt::UserRole).toUInt() == id)
        {
          item = tmp;
          break;
        }
    }
  if (item == NULL)
    {
      item = new QListWidgetItem(_offlineImg, contact);
      ChatWidget		*chat = new ChatWidget(_me, contact, _inCall, _rtpCallManager, this);

      connect(chat, SIGNAL(callStarted()),
              this, SLOT(callStarted()));
      connect(chat, SIGNAL(callFinished()),
              this, SLOT(callFinished()));
      connect(this, SIGNAL(changeCallButton(bool)),
              chat, SLOT(setCallButton(bool)));
      item->setData(Qt::UserRole, id);
      _ui->contactList->addItem(item);
      _ui->chatStack->addWidget(chat);
    }
  else
    {
      if (status == 0)
        item->setIcon(_offlineImg);
      else if (status == 1)
        item->setIcon(_availableImg);
      else if (status == 2)
        item->setIcon(_awayImg); // METTRE BUSY
      else
        item->setIcon(_awayImg);
    }
}

void  MainWindow::connectRegister()
{
  _sipHandler->connectMe();
}

bool MainWindow::contactAlreadyAdded(const QString &contact) const
{
  for (int i = 0; i < _ui->contactList->count(); ++i)
    if (_ui->contactList->item(i)->text() == contact)
      return (true);
  return (false);
}

void MainWindow::moveListItemToPos(const int fromPos, const int toPos)
{
  _ui->contactList->insertItem(toPos, _ui->contactList->takeItem(fromPos));
}

void MainWindow::moveChatWidgetToPos(const int fromPos, const int toPos)
{
  QWidget	*widget = _ui->chatStack->widget(fromPos);
  _ui->chatStack->removeWidget(widget);
  _ui->chatStack->insertWidget(toPos, widget);
}

void MainWindow::moveContactToPos(const QString &contact, const int pos)
{
 for (int i = 0; i < _ui->contactList->count(); ++i)
    if (_ui->contactList->item(i)->text() == contact)
      {
        if (i == pos) return;
        moveListItemToPos(i, pos);
        moveChatWidgetToPos(i, pos);
        _ui->contactList->setCurrentRow(pos);
        _ui->chatStack->setCurrentIndex(pos);
        return;
      }
}

void MainWindow::createTrayIcon()
{
  _trayIconMenu = new QMenu(this);
  _trayIconMenu->addAction(_ui->actionSettings);
  _trayIconMenu->addSeparator();
  _trayIconMenu->addAction(_ui->actionQuit);

  _trayIcon = new QSystemTrayIcon(this);
  _trayIcon->setContextMenu(_trayIconMenu);
  _trayIcon->setToolTip("CamelBabel");
  _trayIcon->setIcon(QIcon(":/images/main_icon.png"));
  connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  _trayIcon->show();
}

void MainWindow::writeSettings()
{
  QSettings     settings;

  settings.beginGroup("MainWindow");
  settings.setValue("size", size());
  settings.setValue("pos", pos());
  settings.endGroup();
}

void MainWindow::readSettings()
{
  QSettings     settings;

  settings.beginGroup("MainWindow");
  resize(settings.value("size", QSize(400, 300)).toSize());
  move(settings.value("pos", QPoint(200, 200)).toPoint());
  settings.endGroup();
}
