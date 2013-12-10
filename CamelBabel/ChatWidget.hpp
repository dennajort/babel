#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QString>
#include <QWidget>
#include "RTPCallManager.hpp"

namespace Ui {
  class ChatWidget;
}

class ChatWidget : public QWidget
{
  Q_OBJECT
  
public:
  explicit    ChatWidget(const QString &me, const QString &contact, bool incall,
                         RTPCallManager *rtpCallManager, QWidget *parent = 0);
  virtual     ~ChatWidget();
  
public slots:
  void        callClicked();
  void        setCallButton(bool stat);

signals:
  void        callStarted();
  void        callFinished();

private:
  Ui::ChatWidget      *_ui;

  QString             _me;
  QString             _contact;

  bool                _inCall;

  RTPCallManager      *_rtpCallManager;
};

#endif // CHATWIDGET_HPP
