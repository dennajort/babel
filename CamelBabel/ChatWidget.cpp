#include "ChatWidget.hpp"
#include "ui_ChatWidget.h"

ChatWidget::ChatWidget(const QString &me, const QString &contact, bool incall,
                       RTPCallManager *rtpCallManager, QWidget *parent) :
  QWidget(parent),
  _ui(new Ui::ChatWidget),
  _me(me),
  _contact(contact),
  _inCall(false),
  _rtpCallManager(rtpCallManager)
{
  _ui->setupUi(this);
  if (incall)
    _ui->callButton->setEnabled(false);
}

ChatWidget::~ChatWidget()
{
  delete _ui;
}

void ChatWidget::callClicked()
{
  if (!_inCall)
    {
      _ui->callButton->setIcon(QIcon::fromTheme("call-stop"));
      _inCall = true;
      emit callStarted();
//      _rtpCallManager->call();
    }
  else
    {
      _ui->callButton->setIcon(QIcon::fromTheme("call-start"));
      emit callFinished();
      _inCall = false;
//      _rtpCallManager->finishCall();
    }
}

void ChatWidget::setCallButton(bool status)
{
  if (!_inCall)
    _ui->callButton->setEnabled(status);
}
