#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include "SettingsDialog.hpp"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
  QDialog(parent),
  _ui(new Ui::SettingsDialog)
{
  QSettings settings;

  _ui->setupUi(this);
  settings.beginGroup("account");
  _ui->usernameLineEdit->setText(settings.value("username", "").toString());
  _ui->passwordLineEdit->setText(settings.value("password", "").toString());
  if (settings.value("savePassword", true).toBool())
    _ui->saveMyPasswordCheckBox->setChecked(true);
  if (settings.value("register", true).toBool())
    _ui->registerCheckBox->setChecked(true);
  _ui->serverLineEdit->setText(settings.value("server", "localhost").toString());
  _ui->portLineEdit->setText(settings.value("port", "4242").toString());
  _ui->moodLineEdit->setText(settings.value("mood", "I love CamelBabel !").toString());
  settings.endGroup();
}

SettingsDialog::~SettingsDialog()
{
  delete _ui;
}

void SettingsDialog::cancelButton()
{
  hide();
}

void SettingsDialog::saveSettings()
{
  QSettings        settings;

  settings.beginGroup("account");
  settings.setValue("username", _ui->usernameLineEdit->text());
  settings.setValue("password", _ui->passwordLineEdit->text());
  settings.setValue("savePassword", _ui->saveMyPasswordCheckBox->isChecked());
  settings.setValue("register", _ui->registerCheckBox->isChecked());
  settings.setValue("server", _ui->serverLineEdit->text());
  settings.setValue("port", _ui->portLineEdit->text());
  settings.setValue("mood", _ui->moodLineEdit->text());
  settings.endGroup();
  if (!_ui->usernameLineEdit->text().isEmpty() && !_ui->passwordLineEdit->text().isEmpty())
    hide();
  else
    QMessageBox::warning(this, "Settings", "Please enter your username and your password !");
}
