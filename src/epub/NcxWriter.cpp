#include "NcxWriter.h"
#include <QtCore>
#include "ChmFile.h"
#include "ChmEntry.h"
#include "ChmToc.h"

class NcxWriterPrivate
{
public:
    NcxWriter* q;

    NcxWriterPrivate(NcxWriter* q) {
        this->q = q;
    }

public:
    void writeHead();
    void writeDocTitle();
    void writeNavMap();
    void writeNavPoint(ChmToc* tocEntry, int& playOrder);
    void writeFallbackNavPoint();
    int tocDepth();

private:
    NcxWriterPrivate(); /* prohibited */
};

NcxWriter::NcxWriter(const ChmFile& chm, QIODevice& device, const QString& uuid)
    : XmlWriter(chm, device, uuid)
{
    d = new NcxWriterPrivate(this);
}

NcxWriter::~NcxWriter()
{
    delete d;
}

void NcxWriter::execute()
{
    _writer->writeStartDocument();
    _writer->writeDTD("<!DOCTYPE ncx PUBLIC \n"
                      "    \"-//NISO//DTD ncx 2005-1//EN\"\n"
                      "    \"http://www.daisy.org/z3986/2005/ncx-2005-1.dtd\">\n");

    _writer->writeStartElement("ncx");
    _writer->writeAttribute("version", "2005-1");
    _writer->writeAttribute("xmlns", "http://www.daisy.org/z3986/2005/ncx/");

    d->writeHead();
    d->writeDocTitle();
    d->writeNavMap();

    _writer->writeEndElement();
    _writer->writeEndDocument();
}

void NcxWriterPrivate::writeHead()
{
    QXmlStreamWriter* writer = q->_writer;

    writer->writeStartElement("head");

    writer->writeEmptyElement("meta");
    writer->writeAttribute("name", "dtb:uid");
    writer->writeAttribute("content", q->_uuid);

    writer->writeEmptyElement("meta");
    writer->writeAttribute("name", "dtb:depth");
    writer->writeAttribute("content", QString::number(tocDepth()));

    writer->writeEmptyElement("meta");
    writer->writeAttribute("name", "dtb:totalPageCount");
    writer->writeAttribute("content", "0");

    writer->writeEmptyElement("meta");
    writer->writeAttribute("name", "dtb:maxPageNumber");
    writer->writeAttribute("content", "0");

    writer->writeEndElement();
}

void NcxWriterPrivate::writeDocTitle()
{
    QString docTitle = q->_chm.title();
    if (docTitle.trimmed().isEmpty()) {
        docTitle = QFileInfo(q->_chm.filename()).completeBaseName();
    }

    q->_writer->writeStartElement("docTitle");
    q->_writer->writeTextElement("text", docTitle);
    q->_writer->writeEndElement();
}

void NcxWriterPrivate::writeNavMap()
{
    int playOrder = 1;

    q->_writer->writeStartElement("navMap");

    ChmToc* rootTocEntry = q->_chm.tocTree();
    if (rootTocEntry && !rootTocEntry->children.isEmpty()) {
        // The NavMap is written recursively.
        // writeNavPoint is called for each entry in the tree.
        foreach (ChmToc* tocEntry, rootTocEntry->children) {
            writeNavPoint(tocEntry, playOrder);
        }
    }
    else {
        // No headings? Well the spec *demands* an NCX file with
        // a NavMap with at least one NavPoint, so put in a dummy one.
        writeFallbackNavPoint();
    }

    q->_writer->writeEndElement();
}

void NcxWriterPrivate::writeNavPoint(ChmToc* tocEntry, int& playOrder)
{
    QXmlStreamWriter* writer = q->_writer;

    writer->writeStartElement("navPoint");

    writer->writeAttribute("id", QString("navPoint-%1").arg(playOrder));
    writer->writeAttribute("playOrder", QString("%1").arg(playOrder));

    playOrder++;

    writer->writeStartElement("navLabel");
    writer->writeTextElement("text", tocEntry->title);
    writer->writeEndElement();

    writer->writeEmptyElement("content");
    writer->writeAttribute("src", tocEntry->path);

    foreach (ChmToc* child, tocEntry->children) {
        writeNavPoint(child, playOrder);
    }

    writer->writeEndElement();
}

void NcxWriterPrivate::writeFallbackNavPoint()
{
    QXmlStreamWriter* writer = q->_writer;

    writer->writeStartElement("navPoint");
    writer->writeAttribute("id", "navPoint-1");
    writer->writeAttribute("playOrder", "1");

    writer->writeStartElement("navLabel");
    writer->writeTextElement("text", "Start");
    writer->writeEndElement();

    ChmToc* rootTocEntry = q->_chm.tocTree();
    writer->writeEmptyElement("content");
    writer->writeAttribute("src", rootTocEntry ? rootTocEntry->path : "/");

    writer->writeEndElement();
}

int NcxWriterPrivate::tocDepth()
{
    // TODO: return the actual depth
    return 1;
}
