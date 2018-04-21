#ifndef EPUBWRITER_H
#define EPUBWRITER_H

#include <QObject>

class EpubWriterPrivate;

class EpubWriter : public QObject
{
    Q_OBJECT

public:
    EpubWriter(const QString& chmFilename, const QString& epubFilename, QObject* parent = 0);
    virtual ~EpubWriter();

    void execute();

    QString chmFilename() const;
    QString epubFilename() const;

    int executionDuration() const;

    bool hasError() const;
    QString errorText() const;

signals:
    void writeStarted(int maximum);
    void writeProgress(int progress, const QString& text);
    void writeFinished(bool ok);

private:
    friend class EpubWriterPrivate;
    EpubWriterPrivate* d;
};

#endif // EPUBWRITER_H
