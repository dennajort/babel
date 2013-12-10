#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QString>
#include <QWidget>

namespace Ui {
  class ChatWidget;
}

class ChatWidget : public QWidget
{
  Q_OBJECT
  
public:
  explicit    ChatWidget(const QString &me, const QString &contact, QWidget *parent = 0);
  virtual     ~ChatWidget();
  
private:
  Ui::ChatWidget      *_ui;

  const QString       _me;
  const QString       _contact;
};

#endif // CHATWIDGET_HPP
