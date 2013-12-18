#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QWidget>
#include <QDialog>

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit            SettingsDialog(QWidget *parent = 0);
  virtual             ~SettingsDialog();

public slots:
  virtual void        accept();

private:
  Ui::SettingsDialog        *_ui;
};

#endif // SETTINGSDIALOG_HPP
