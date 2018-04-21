#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>

class Application : public QObject
{
    Q_OBJECT

public:
    Application(int argc, char *argv[]);

    int exec();

public slots:
    void epubWriter_writeStarted(int maximum);
    void epubWriter_writeProgress(int progress, const QString& text);
    void epubWriter_writeFinished(bool ok);

private:
    Application(); /* prohibited */

    bool isValid() const;
    void printUsage();

private:
    QString _inputFilename;  // CHM file as input
    QString _outputFilename; // EPUB file as output
    bool _verbose; // ��ʾ��ϸ����, Ĭ��false
    bool _forceOverwrite; // ǿ�Ƹ��ǣ�Ĭ��false
};

#endif
