#include "ConnectionDialog.hpp"
#include "ui_ConnectionDialog.h"

ConnectionDialog::ConnectionDialog(QWidget *parent) :
  QDialog(parent),
  _ui(new Ui::ConnectionDialog)
{
  _ui->setupUi(this);
}

ConnectionDialog::~ConnectionDialog()
{
  delete _ui;
}
