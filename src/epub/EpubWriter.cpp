#include "EpubWriter.h"
#include <QtCore>
#include "ChmFile.h"
#include "ChmEntry.h"
#include "ChmToc.h"
#include "quazip.h"
#include "quazipfile.h"
#include "quazipfileinfo.h"
#include "NcxWriter.h"
#include "OpfWriter.h"
#include "tidy.h"
#include "buffio.h"

class EpubWriterPrivate
{
public:
    EpubWriter* q;

    QString chmFilename; // chm file as input file
    QString epubFilename; // epub file as output file

    ChmFile* chmFile;
    QuaZip* epubFile;

    int progress;
    int maxProgress;

    bool hasError;
    QString errorText;

    QDateTime execStartTime;
    QDateTime execEndTime;

public:
    EpubWriterPrivate(const QString& chmFilename, const QString& epubFilename, EpubWriter* q) {
        this->q = q;

        this->chmFilename = chmFilename;
        this->epubFilename = epubFilename;
        chmFile = new ChmFile(chmFilename);
        epubFile = new QuaZip(epubFilename);
        epubFile->setFileNameCodec("GBK");
        epubFile->setCommentCodec("GBK");

        progress = 0;
        maxProgress = 0;
        hasError = false;

        execStartTime = QDateTime::currentDateTime();
        execEndTime = QDateTime::currentDateTime();
    }

    ~EpubWriterPrivate() {
        chmFile->close();
        epubFile->close();

        delete epubFile;
        delete chmFile;
    }

    bool writeMetaFiles();
    bool writeContentFiles();
    bool writeContentFile(ChmEntry* entry);

    void setError(const QString& value) {
        errorText = value;
        hasError = true;
        execEndTime = QDateTime::currentDateTime();
        emit q->writeFinished(false);
    }

private:
    EpubWriterPrivate(); /* prohibited */
};

EpubWriter::EpubWriter(const QString& chmFilename, const QString& epubFilename, QObject* parent)
    : QObject(parent)
{
    d = new EpubWriterPrivate(chmFilename, epubFilename, this);
}

EpubWriter::~EpubWriter()
{
    delete d;
}

void EpubWriter::execute()
{
    d->execStartTime = QDateTime::currentDateTime();

    d->chmFile->open();
    d->epubFile->open(QuaZip::mdCreate);

    int fileCount = 0;
    fileCount += 1; // mimetype
    fileCount += 3; // META-INF/container.xml, OPS/fb.opf, OPS/fb.ncx
    fileCount += d->chmFile->entries().count(); // OPS/*.*
    d->progress = 0;
    d->maxProgress = fileCount;
    emit writeStarted(d->maxProgress);

    d->writeMetaFiles();
    d->writeContentFiles();

    d->execEndTime = QDateTime::currentDateTime();
    emit writeFinished(true);
}

QString EpubWriter::chmFilename() const
{
    return d->chmFilename;
}

QString EpubWriter::epubFilename() const
{
    return d->epubFilename;
}

int EpubWriter::executionDuration() const
{
    return d->execStartTime.msecsTo(d->execEndTime);
}

bool EpubWriter::hasError() const
{
    return d->hasError;
}

QString EpubWriter::errorText() const
{
    return d->errorText;
}

bool EpubWriterPrivate::writeMetaFiles()
{
    QString guid = QUuid::createUuid().toString();
    guid = guid.mid(1, guid.size() - 2); // "{...}" => "..."
    QString entryName;

    // mimetype
    entryName = "mimetype";
    QuaZipFile mimetypeEntry(epubFile);
    if (!mimetypeEntry.open(QIODevice::WriteOnly, QuaZipNewInfo(entryName))) {
        setError(QObject::tr("Can't write epub entry: `%1`").arg(entryName));
        return false;
    }
    mimetypeEntry.write("application/epub+zip");
    mimetypeEntry.close();
    emit q->writeProgress(progress++, QObject::tr("Entry `%1` written.").arg(entryName));

    // META-INF/container.xml
    entryName = "META-INF/container.xml";
    QuaZipFile container_xml_entry(epubFile);
    if (!container_xml_entry.open(QIODevice::WriteOnly, QuaZipNewInfo(entryName))) {
        setError(QObject::tr("Can't write epub entry: `%1`").arg(entryName));
        return false;
    }
    QXmlStreamWriter container_xml(&container_xml_entry);
    container_xml.setAutoFormatting(true);
    container_xml.setCodec("utf8");
    container_xml.writeStartDocument();
    container_xml.writeStartElement("container");
    container_xml.writeAttribute("version", "1.0");
    container_xml.writeAttribute("xmlns", "urn:oasis:names:tc:opendocument:xmlns:container");
        container_xml.writeStartElement("rootfiles");
            container_xml.writeEmptyElement("rootfile");
            container_xml.writeAttribute("full-path", "OPS/fb.opf");
            container_xml.writeAttribute("media-type", "application/oebps-package+xml");
        container_xml.writeEndElement();
    container_xml.writeEndElement();
    container_xml.writeEndDocument();
    container_xml_entry.close();
    emit q->writeProgress(progress++, QObject::tr("Entry `%1` written.").arg(entryName));

    // OPS/fb.opf
    entryName = "OPS/fb.opf";
    QuaZipFile opfEntry(epubFile);
    if (!opfEntry.open(QIODevice::WriteOnly, QuaZipNewInfo(entryName))) {
        setError(QObject::tr("Can't write epub entry: `%1`").arg(entryName));
        return false;
    }
    OpfWriter opfWriter(*chmFile, opfEntry, guid);
    opfWriter.execute();
    emit q->writeProgress(progress++, QObject::tr("Entry `%1` written.").arg(entryName));

    // OPS/fb.ncx
    entryName = "OPS/fb.ncx";
    QuaZipFile ncxEntry(epubFile);
    if (!ncxEntry.open(QIODevice::WriteOnly, QuaZipNewInfo(entryName))) {
        setError(QObject::tr("Can't write epub entry: `%1`").arg(entryName));
        return false;
    }
    NcxWriter ncxWriter(*chmFile, ncxEntry, guid);
    ncxWriter.execute();
    emit q->writeProgress(progress++, QObject::tr("Entry `%1` written.").arg(entryName));
    return true;
}

bool EpubWriterPrivate::writeContentFiles()
{
    if (!chmFile->isOpen()) {
        setError(QObject::tr("Can't open file %1 for reading, aborted").arg(chmFilename));
        return false;
    }

    if (!epubFile->isOpen()) {
        setError(QObject::tr("Can't open file `%1` for writing, aborted.").arg(epubFilename));
        return false;
    }

    bool ret = true;
    foreach (ChmEntry* entry, chmFile->entries()) {
        ret = writeContentFile(entry);
    }

    return ret;
}

bool EpubWriterPrivate::writeContentFile(ChmEntry* entry)
{
    if (!entry || !epubFile)
        return false;

    // ignore dirs, meta/special files
    if (entry->type() == ChmEntry::NormalDir || entry->type() == ChmEntry::SpecialDir
        || entry->type() == ChmEntry::MetaDir || entry->type() == ChmEntry::MetaFile
        || entry->type() == ChmEntry::SpecialFile) {
        return true;
    }

    QuaZipFile outEntry(epubFile);
    QString entryName = "OPS/" + entry->name();
    if (!outEntry.open(QIODevice::WriteOnly, QuaZipNewInfo(entryName))) {
        setError(QObject::tr("Can't write epub entry: `%1`").arg(entryName));
        return false;
    }
    QByteArray inbuf = entry->readAll();
    QString suffix = QFileInfo(entryName).suffix().toLower();
    bool isHtml = suffix == "htm" || suffix == "html";
    if (isHtml) {
        // use htmtidy lib to transform HTML to XHTML
        TidyBuffer tout = {0};
        int rc = -1;
        bool ok;
        TidyDoc tdoc = tidyCreate();
        tidyOptSetBool(tdoc, TidyShowWarnings, no); // supress warnings written to console
        ok = tidyOptSetBool(tdoc, TidyXhtmlOut, yes); // convert to XHTML
        if (ok)
            rc = tidyParseString(tdoc, inbuf.data()); // parse the input
        if (rc >= 0)
            rc = tidyCleanAndRepair(tdoc); // tidy it up!
        if (rc >= 0)
            rc = tidySaveBuffer(tdoc, &tout); // pretty print
        if (rc >= 0 && tout.bp) {
            QByteArray tbuf((const char*) tout.bp);
            outEntry.write(tbuf);
            tidyBufFree(&tout);
        }
        else {
            outEntry.write(inbuf);
            setError(QObject::tr("Failed to tidy html entry: '%1', output as is.").arg(entryName));
        }
        tidyRelease(tdoc);
    }
    else {
        outEntry.write(inbuf);
    }
    outEntry.close();

    emit q->writeProgress(progress++, QObject::tr("Entry `%1` written.").arg(entryName));

    return true;
}
