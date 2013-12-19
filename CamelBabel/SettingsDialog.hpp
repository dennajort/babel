#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include "SipHandler.hpp"

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit    SettingsDialog(QWidget *parent = 0);
  virtual     ~SettingsDialog();

signals:

public slots:
  void  cancelButton();
  void  registerButton();

private:
Ui::SettingsDialog      *_ui;
};

#endif // SETTINGSDIALOG_HPP
