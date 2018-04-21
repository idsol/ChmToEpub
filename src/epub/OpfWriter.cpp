#include "OpfWriter.h"
#include <QtCore>
#include "ChmFile.h"
#include "ChmEntry.h"
#include "ChmToc.h"

class OpfWriterPrivate
{
public:
    OpfWriter* q;

    QHash<QString,QString> mimeTypes;

    OpfWriterPrivate(OpfWriter* q) {
        this->q = q;

        initMimetypes();
    }

public:
    void initMimetypes();

    void writeMetadata();
    void writeCoverImageMeta();
    void writeManifest();
    void writeManifestItem(ChmToc* tocEntry);
    QString getEntryId(const QString& entryName);
    bool isValidIdCharacter(QChar ch);
    void writeSpine();
    void writeSpineItem(ChmToc* tocEntry);
    void writeGuide();
    void writeGuideItem(ChmToc* tocEntry);

private:
    OpfWriterPrivate(); /* prohibited */
};

OpfWriter::OpfWriter(const ChmFile& chm, QIODevice& device, const QString& uuid)
    : XmlWriter(chm, device, uuid)
{
    d = new OpfWriterPrivate(this);
}

OpfWriter::~OpfWriter()
{
    delete d;
}

void OpfWriter::execute()
{
    _writer->writeStartDocument();
    _writer->writeStartElement("package");
    _writer->writeAttribute("xmlns", "http://www.idpf.org/2007/opf");
    _writer->writeAttribute("unique-identifier", "BookID");
    _writer->writeAttribute("version", "2.0");

    d->writeMetadata();
    d->writeManifest();
    d->writeSpine();
    d->writeGuide();

    _writer->writeEndElement();
    _writer->writeEndDocument();
}

void OpfWriterPrivate::initMimetypes()
{
    mimeTypes["jpg"] = "image/jpeg";
    mimeTypes["jpeg"] = "image/jpeg";
    mimeTypes["png"] = "image/png";
    mimeTypes["gif"] = "image/gif";
    mimeTypes["tif"] = "image/tiff";
    mimeTypes["tiff"] = "image/tiff";
    mimeTypes["bm"] = "image/bmp";
    mimeTypes["bmp"] = "image/bmp";
    mimeTypes["svg"] = "image/svg+xml";

    mimeTypes["ncx"] = "application/x-dtbncx+xml";

    // We convert all HTML document types to XHTML
    mimeTypes["xml"] = "application/xhtml+xml";
    mimeTypes["xhtml" ] = "application/xhtml+xml";
    mimeTypes["html"] = "application/xhtml+xml";
    mimeTypes["htm"] = "application/xhtml+xml";
    mimeTypes["css"] = "text/css";

    // Hopefully we won't get a lot of these
    mimeTypes["xpgt"] = "application/vnd.adobe-page-template+xml";

    // Until the standards gods grace us with font mimetypes, these will have to do
    mimeTypes["otf"] = "application/x-font-opentype";
    mimeTypes["ttf"] = "application/x-font-truetype";
}

void OpfWriterPrivate::writeMetadata()
{
    QXmlStreamWriter* writer = q->_writer;

    writer->writeStartElement("metadata");
    writer->writeAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    writer->writeAttribute("xmlns:opf", "http://www.idpf.org/2007/opf");

    QString docTitle = q->_chm.title();
    if (docTitle.trimmed().isEmpty()) {
        docTitle = QFileInfo(q->_chm.filename()).completeBaseName();
    }
    writer->writeTextElement("dc:title", docTitle);
    writer->writeStartElement("dc:identifier");
    writer->writeAttribute("id", "BookID");
    writer->writeAttribute("opf:scheme", "UUID");
    writer->writeCharacters(q->_uuid);
    writer->writeEndElement();
    writer->writeTextElement("dc:language", "zh");
    writer->writeStartElement("dc:creator");
    writer->writeCharacters("ChmToEpub");
    writer->writeEndElement();

    writeCoverImageMeta();

    writer->writeEndElement(); // </metadata>
}

void OpfWriterPrivate::writeCoverImageMeta()
{
    // TODO: implement
}

void OpfWriterPrivate::writeManifest()
{
    QXmlStreamWriter* writer = q->_writer;

    writer->writeStartElement("manifest");

    writer->writeEmptyElement("item");
    writer->writeAttribute("id", "ncx");
    writer->writeAttribute("href", "fb.ncx");
    writer->writeAttribute("media-type", mimeTypes["ncx"]);

    ChmToc* rootTocEntry = q->_chm.tocTree();
    if (rootTocEntry) {
        foreach (ChmToc* tocEntry, rootTocEntry->children) {
            writeManifestItem(tocEntry);
        }
    }

    writer->writeEndElement();
}

void OpfWriterPrivate::writeManifestItem(ChmToc* tocEntry)
{
    if (!tocEntry)
        return;

    QXmlStreamWriter* writer = q->_writer;

    if (!tocEntry->path.isEmpty()) {
        writer->writeEmptyElement("item");
        writer->writeAttribute("id", getEntryId(tocEntry->path));
        writer->writeAttribute("href", tocEntry->path);
        QString suffix = QFileInfo(tocEntry->path).suffix().toLower();
        writer->writeAttribute("media-type", mimeTypes[suffix]);
    }

    foreach (ChmToc* childEntry, tocEntry->children) {
        writeManifestItem(childEntry);
    }
}

QString OpfWriterPrivate::getEntryId(const QString& entryName)
{
    QString ret = entryName.simplified();
    QFileInfo fi(ret);
    ret = fi.path() + "_" + fi.completeBaseName();

    // remove all forbidden characters.
    int i = 0;
    while (i < ret.size()) {
        if (!isValidIdCharacter(ret.at(i)))
            ret.remove(i, 1);
        else
            ++i;
    }

    if (ret.isEmpty()) {
        QString guid = QUuid::createUuid().toString();
        guid = guid.mid(1, guid.size() - 2);
        return guid;
    }

    // IDs cannot start with a number, a dash or a dot
    QChar firstChar = ret.at(0);
    if (firstChar.isNumber() || firstChar == '-' || firstChar == '.') {
        ret.prepend('x');
    }

    return ret;
}

/**
 * This is probably more rigorous than the XML spec, but it's simpler.
 * (spec ref: http://www.w3.org/TR/xml-id/#processing)
 */
bool OpfWriterPrivate::isValidIdCharacter(QChar ch)
{
    return ch.isLetterOrNumber()
        || ch == QLatin1Char('=')
        || ch == QLatin1Char('-')
        || ch == QLatin1Char('_')
        || ch == QLatin1Char('.')
        || ch == QLatin1Char(':');
}

void OpfWriterPrivate::writeSpine()
{
    QXmlStreamWriter* writer = q->_writer;

    writer->writeStartElement("spine");
    writer->writeAttribute("toc", "ncx");
    ChmToc* rootTocEntry = q->_chm.tocTree();
    if (rootTocEntry) {
        foreach (ChmToc* tocEntry, rootTocEntry->children) {
            writeSpineItem(tocEntry);
        }
    }
    writer->writeEndElement();
}

void OpfWriterPrivate::writeSpineItem(ChmToc* tocEntry)
{
    if (!tocEntry)
        return;

    QXmlStreamWriter* writer = q->_writer;

    if (!tocEntry->path.isEmpty()) {
        writer->writeEmptyElement("itemref");
        writer->writeAttribute("idref", getEntryId(tocEntry->path));
    }

    foreach (ChmToc* childEntry, tocEntry->children) {
        writeSpineItem(childEntry);
    }
}

void OpfWriterPrivate::writeGuide()
{
    QXmlStreamWriter* writer = q->_writer;

    writer->writeStartElement("guide");
    ChmToc* rootTocEntry = q->_chm.tocTree();
    if (rootTocEntry) {
        foreach (ChmToc* tocEntry, rootTocEntry->children) {
            writeGuideItem(tocEntry);
        }
    }
    writer->writeEndElement();
}

void OpfWriterPrivate::writeGuideItem(ChmToc* tocEntry)
{
    if (!tocEntry)
        return;

    QXmlStreamWriter* writer = q->_writer;

    if (!tocEntry->path.isEmpty()) {
        writer->writeEmptyElement("reference");
        writer->writeAttribute("type", "text");
        writer->writeAttribute("title", tocEntry->title);
        writer->writeAttribute("href", tocEntry->path);
    }

    foreach (ChmToc* childEntry, tocEntry->children) {
        writeGuideItem(childEntry);
    }
}
