#include "ChatWidget.hpp"
#include "ui_ChatWidget.h"
#include "RTPCallManager.hpp"

ChatWidget::ChatWidget(const QString &me, const QString &contact, QWidget *parent) :
  QWidget(parent),
  _ui(new Ui::ChatWidget),
  _me(me),
  _contact(contact)
{
  _ui->setupUi(this);
  RTPCallManager *rtpCallManager = new RTPCallManager(this);
  (void)rtpCallManager;
}

ChatWidget::~ChatWidget()
{
  delete _ui;
}
