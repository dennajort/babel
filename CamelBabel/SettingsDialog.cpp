#include "SettingsDialog.hpp"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
  QDialog(parent),
  _ui(new Ui::SettingsDialog)
{
  _ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
  delete _ui;
}
