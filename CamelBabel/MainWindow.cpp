#include <QMessageBox>
#include <QSettings>
#include <QInputDialog>
#include <QVariant>
#include <QTimer>
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
  _doNotDisturbImg(":/image/do_not_disturb.png"),
  _offlineImg(":/images/offline.png"),
  _trayIcon(new QSystemTrayIcon(this)),
  _trayIconMenu(new QMenu(this)),
  _inCall(false),
  _rtpCallManager(new RTPCallManager(this)),
  _sipHandler(new SipHandler(this))
{
  _ui->setupUi(this);
  readSettings();
  createTrayIcon();
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

  if (!contact.isEmpty() && !contactAlreadyAdded(contact))
    emit addContact(contact);
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

void MainWindow::displayMessage(const QString &message)
{
  _trayIcon->showMessage("Error", message, QSystemTrayIcon::MessageIcon(2), 4000);
}

void MainWindow::clientConnected(const bool res)
{
  int   currentIndex = _ui->statusCombo->currentIndex();

  qDebug() << "MainWindow::clientConnected";
  if (res && currentIndex >= 0 && currentIndex < 3)
    {
      if (!currentIndex)
        {
          _trayIcon->setIcon(_availableImg);
          _sipHandler->setStatus(0);
        }
      else if (currentIndex == 1)
        {
          _trayIcon->setIcon(_awayImg);
          _sipHandler->setStatus(2);
        }
      else
        {
          _trayIcon->setIcon(_doNotDisturbImg);
          _sipHandler->setStatus(1);
        }
    }
  else
    _ui->statusCombo->setCurrentIndex(3);
}

void MainWindow::disconnected()
{
  qDebug() << "MainWindow::disconnected";
  if (_ui->statusCombo->currentIndex() != 3)
    {
      _ui->statusCombo->setCurrentIndex(3);
      _trayIcon->showMessage("Disconnected", "You've been disconnected !",
                            QSystemTrayIcon::MessageIcon(2), 4000);
      QTimer::singleShot(10000, _sipHandler, SLOT(connectMe()));
    }
}

void  MainWindow::registerError()
{
  qDebug() << "register error";
}

void MainWindow::contact(const unsigned int id, const QString &username,
			 const unsigned int status, const QString &mood)
{
  Q_UNUSED(mood);
  addChat(id, username, status);
}

void MainWindow::callRequest(const unsigned int id)
{
  QListWidgetItem       *item = getContactById(id);
  if (item != NULL && _inCall == false)
    {
      _ui->contactList->setCurrentItem(item);
      _ui->chatStack->setCurrentIndex(_ui->contactList->currentRow());
      qDebug() << "call request";
      int ret = QMessageBox::question(this, "Call request", "Accept call from " + item->text(),
                                     QMessageBox::Yes | QMessageBox::No);
      if (ret == QMessageBox::Yes)
        {
          (reinterpret_cast<ChatWidget*>(_ui->chatStack->currentWidget()))->callClicked();
          emit acceptCall(id);
        }
      else
        emit declineCall(id);
    }
  else
    emit declineCall(id);
}

void MainWindow::addContactResult(bool res)
{
  if (res)
    emit listContacts();
}

//
// A verifier
//
void MainWindow::changeStatus(int index)
{
  qDebug() << "MainWindow::changeStatus";
  if (!_sipHandler->isConnected() && index < 3)
    _sipHandler->connectMe();
  else if (!index)
    {
      _trayIcon->setIcon(_availableImg);
      _sipHandler->setStatus(0);
    }
  else if (index == 1)
    {
      _trayIcon->setIcon(_awayImg);
      _sipHandler->setStatus(2);
    }
  else if (index == 2)
    {
      _trayIcon->setIcon(_doNotDisturbImg);
      _sipHandler->setStatus(1);
    }
  else
    {
      _trayIcon->setIcon(_offlineImg);
      _ui->contactList->clear();
      if (_sipHandler->isConnected())
        _sipHandler->disconnectMe();
    }
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
  QListWidgetItem       *item = getContactById(id);

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
        item->setIcon(_doNotDisturbImg);
      else
        item->setIcon(_awayImg);
    }
}

QListWidgetItem *MainWindow::getContactById(const unsigned int id)
{
  QListWidgetItem       *item = NULL;

  for (int i = 0; i < _ui->contactList->count(); ++i)
    {
      QListWidgetItem *tmp = _ui->contactList->item(i);
      if (tmp->data(Qt::UserRole).toUInt() == id)
        {
          return (tmp);
        }
    }
  return (NULL);
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
  _trayIconMenu->addAction(_ui->actionActiver);
  _trayIconMenu->addSeparator();
  _trayIconMenu->addAction(_ui->actionSettings);
  _trayIconMenu->addAction(_ui->actionAbout);
  _trayIconMenu->addSeparator();
  _trayIconMenu->addAction(_ui->actionQuit);

  _trayIcon->setContextMenu(_trayIconMenu);
  _trayIcon->setToolTip("CamelBabel");
  _trayIcon->setIcon(_offlineImg);
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
