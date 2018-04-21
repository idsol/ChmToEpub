#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QtGui>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _process(new QProcess(this))
{
    ui->setupUi(this);

    connect(ui->addInputFilesButton, SIGNAL(clicked()), this, SLOT(addInputFilesButton_clicked()));
    connect(ui->inputFilesListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(inputFilesListWidget_itemSelectionChanged()));
    connect(ui->removeInputFileButton, SIGNAL(clicked()), this, SLOT(removeInputFileButton_clicked()));
    connect(ui->clearInputFilesButton, SIGNAL(clicked()), this, SLOT(clearInputFilesButton_clicked()));
    connect(ui->outputFileButton, SIGNAL(clicked()), this, SLOT(outputFileButton_clicked()));
    connect(ui->outputDirButton, SIGNAL(clicked()), this, SLOT(outputDirButton_clicked()));
    connect(ui->convertButton, SIGNAL(clicked()), this, SLOT(convertButton_clicked()));

    connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(process_readyReadStandardOutput()));
    connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(process_readyReadStandardError()));
    connect(_process, SIGNAL(finished(int)), this, SLOT(process_finished(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addInputFilesButton_clicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Select input files"), "", tr("CHM File (*.chm)"));
    if (filenames.isEmpty())
        return;

    // avoid duplicates
    foreach (QString filename, filenames) {
        QList<QListWidgetItem*> items = ui->inputFilesListWidget->findItems(filename, Qt::MatchFixedString);
        if (items.isEmpty()) {
            ui->inputFilesListWidget->addItems(filenames);
        }
    }
    inputFileCountChanged();

    // auto fill output filename
    QFileInfo ifi(filenames.first());
    QString outputFilename = ifi.path() + "/" + ifi.completeBaseName() + ".epub";
    ui->outputFileEdit->setText(outputFilename);
    if (ui->outputDirEdit->text().isEmpty()) {
        ui->outputDirEdit->setText(ifi.path());
    }
}

void MainWindow::inputFilesListWidget_itemSelectionChanged()
{
    ui->removeInputFileButton->setEnabled(!ui->inputFilesListWidget->selectedItems().isEmpty());
}

void MainWindow::removeInputFileButton_clicked()
{
    QList<QListWidgetItem*> selItems = ui->inputFilesListWidget->selectedItems();
    foreach (QListWidgetItem* item, selItems) {
        ui->inputFilesListWidget->takeItem(ui->inputFilesListWidget->row(item));
    }

    inputFileCountChanged();
}

void MainWindow::clearInputFilesButton_clicked()
{
    ui->inputFilesListWidget->clear();

    inputFileCountChanged();
}

void MainWindow::inputFileCountChanged()
{
    bool outputFileCtrlsEnabled = ui->inputFilesListWidget->count() == 1;
    ui->outputFileEdit->setEnabled(outputFileCtrlsEnabled);
    ui->outputFileButton->setEnabled(outputFileCtrlsEnabled);
}

void MainWindow::outputFileButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Select output file"), "", tr("EPUB File (*.epub)"));
    if (filename.isNull())
        return;

    ui->outputFileEdit->setText(filename);
}

void MainWindow::outputDirButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select output dir"), "");
    if (dir.isNull())
        return;

    ui->outputDirEdit->setText(dir);
}

void MainWindow::convertButton_clicked()
{
    logClear();

    _inputFileStack.clear();
    if (ui->inputFilesListWidget->count() > 0) {
        for (int i = ui->inputFilesListWidget->count() - 1; i >= 0; --i) {
            QListWidgetItem* item = ui->inputFilesListWidget->item(i);
            _inputFileStack.push_back(item->text());
        }
    }

    if (_inputFileStack.isEmpty()) {
        log(tr("<span style='color:red'>Select input file(s) first.</font>"));
        return;
    }

    convert(_inputFileStack.pop());
}

void MainWindow::process_readyReadStandardOutput()
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    if (!process)
        return;
    log(process->readAll());
}

void MainWindow::process_readyReadStandardError()
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    if (!process)
        return;
    log(tr("<span style='color:red'>%1</span>").arg(QString(process->readAll())));
}

void MainWindow::process_finished(int retCode)
{
    if (!_inputFileStack.isEmpty()) {
        convert(_inputFileStack.pop());
    }
}

void MainWindow::logClear()
{
    ui->logEdit->clear();
}

void MainWindow::log(const QString& text)
{
    ui->logEdit->append(text);
    ui->logEdit->update();
}

void MainWindow::convert(const QString& inputFilename)
{
    QString program = qApp->applicationDirPath() + "/ChmToEpub.exe";
    QStringList args;

    if (!inputFilename.isEmpty()) {
        args << inputFilename;
    }
    QString outputFilenameOrDir = ui->inputFilesListWidget->count() > 1
                                  ? ui->outputDirEdit->text() : ui->outputFileEdit->text();
    if (!outputFilenameOrDir.isEmpty()) {
        args << outputFilenameOrDir;
    }
    if (ui->verboseSwitch->isChecked()) {
        args << "-v";
    }
    if (ui->overwriteSwitch->isChecked()) {
        args << "-o";
    }

    _process->start(program, args, QIODevice::ReadOnly);
}
