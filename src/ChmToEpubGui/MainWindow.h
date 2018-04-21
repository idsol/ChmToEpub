#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QStack>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();

private slots:
    void addInputFilesButton_clicked();
    void inputFilesListWidget_itemSelectionChanged();
    void removeInputFileButton_clicked();
    void clearInputFilesButton_clicked();
    void inputFileCountChanged();
    void outputFileButton_clicked();
    void outputDirButton_clicked();
    void convertButton_clicked();

    void process_readyReadStandardOutput();
    void process_readyReadStandardError();
    void process_finished(int retCode);

private:
    void logClear();
    void log(const QString& text);
    void convert(const QString& inputFilename);

private:
    Ui::MainWindow* ui;
    QProcess* _process;
    QStack<QString> _inputFileStack;
};

#endif // MAINWINDOW_H
