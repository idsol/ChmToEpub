#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QString>

class ChmFile;
class QIODevice;
class QXmlStreamWriter;

/**
 * An abstract base class for xml writers like NcxWriter and OpfWriter.
 */
class XmlWriter
{
public:
    XmlWriter(const ChmFile& book, QIODevice& device, const QString& uuid);
    virtual ~XmlWriter();

    virtual void execute() = 0;

protected:
    const ChmFile& _chm;
    QIODevice& _ioDevice;
    QXmlStreamWriter* _writer;
    QString _uuid;
};

#endif // XMLWRITER_H
