#include "XmlWriter.h"
#include <QXmlStreamWriter>

XmlWriter::XmlWriter(const ChmFile& chm, QIODevice& device, const QString& uuid)
    : _chm(chm)
    , _ioDevice(device)
    , _writer(new QXmlStreamWriter(&_ioDevice))
    , _uuid(uuid)
{
    _writer->setAutoFormatting(true);
    _writer->setCodec("utf8");
}

XmlWriter::~XmlWriter()
{
    delete _writer;
}
