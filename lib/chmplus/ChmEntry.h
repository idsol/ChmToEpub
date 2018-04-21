#ifndef CHMENTRY_H
#define CHMENTRY_H

#include <QObject>

class ChmEntryPrivate;
class ChmUnitInfo;

class Q_DECL_EXPORT ChmEntry : public QObject
{
    Q_OBJECT

public:
    enum EntryType {
        None = 0,
        NormalDir = 1,
        SpecialDir = 2,
        MetaDir = 3,
        NormalFile = 4,
        SpecialFile = 5,
        MetaFile = 6
    };

public:
    ChmEntry(QObject* parent = 0);
    ChmEntry(ChmUnitInfo* unitInfo, QObject* parent = 0);
    ChmEntry(const QString& name, QObject* parent = 0);
    ChmEntry(const ChmEntry& other, QObject* parent = 0);
    void assign(const ChmEntry& other);
    virtual ~ChmEntry();

public:
    QString name() const;
    void setName(const QString& value);

    EntryType type() const;
    QString typeText() const;
    void setType(EntryType value);

    qint64 start() const;
    void setStart(qint64 value);

    qint64 pos() const;
    void setPos(qint64 value);
    bool atEnd() const;

    qint64 size() const;
    void setSize(qint64 value);

    ChmUnitInfo* unitInfo() const;
    void setUnitInfo(ChmUnitInfo* value);

    bool exists() const;

    QByteArray readAll() const;
    QByteArray read(int size);
    qint16 readShort();
    qint32 readInteger();

private:
    ChmEntryPrivate* d;
};

#endif // CHMENTRY_H
