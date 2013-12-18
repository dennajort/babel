#ifndef CONNECTIONDIALOG_HPP
#define CONNECTIONDIALOG_HPP

#include <QDialog>

namespace Ui {
  class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit    ConnectionDialog(QWidget *parent = 0);
  virtual     ~ConnectionDialog();
  
signals:
  
public slots:
  
private:
  Ui::ConnectionDialog      *_ui;
};

#endif // CONNECTIONDIALOG_HPP
