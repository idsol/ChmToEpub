#ifndef CHMFILE_H
#define CHMFILE_H

#include <QObject>

class ChmFilePrivate;
class ChmToc;
class ChmEntry;
struct ChmFileHandle;

class Q_DECL_EXPORT ChmFile : public QObject
{
    Q_OBJECT

public:
    ChmFile(QObject *parent = 0);
    ChmFile(const QString& filename);
    virtual ~ChmFile();

public:
    bool open();
    bool isOpen() const;
    void close();

    QString filename() const;
    ChmFileHandle* handle() const;

    QString title() const;

    ChmToc* tocTree() const;
    QList<ChmEntry*> entries() const;
    ChmEntry entry(const QString& name) const;
    bool entryExists(const QString& name) const;

private:
    ChmFilePrivate* d;
};

#endif // CHMFILE_H
