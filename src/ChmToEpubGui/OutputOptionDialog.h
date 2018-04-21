#ifndef OUTPUTOPTIONDIALOG_H
#define OUTPUTOPTIONDIALOG_H

#include <QDialog>

namespace Ui {
    class OutputOptionDialog;
}

class OutputOptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OutputOptionDialog(QWidget *parent = 0);
    ~OutputOptionDialog();

private:
    Ui::OutputOptionDialog *ui;
};

#endif // OUTPUTOPTIONDIALOG_H
