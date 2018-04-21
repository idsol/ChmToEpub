#include "OutputOptionDialog.h"
#include "ui_OutputOptionDialog.h"

OutputOptionDialog::OutputOptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputOptionDialog)
{
    ui->setupUi(this);
}

OutputOptionDialog::~OutputOptionDialog()
{
    delete ui;
}
