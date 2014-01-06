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
  void        appendMessage(const QString &name, const QString &date, const QString &message);
  void        sendCall();
  void        endCall();
  void        startCall(const QString &ip, quint16 port);
  bool        isInCall() const {return (_inCall);}
  
public slots:
  void        callClicked(const bool startCall = true);
  void        setCallButton(bool stat);
  void        sendText();

signals:
  void        callStarted(bool startCall);
  void        callFinished();
  void        sendMessageToCurrent(const QString &message);

private:
  Ui::ChatWidget      *_ui;

  QString             _me;
  QString             _contact;

  bool                _inCall;

  RTPCallManager      *_rtpCallManager;
};

#endif // CHATWIDGET_HPP
