#ifndef NCXWRITER_H
#define NCXWRITER_H

#include "XmlWriter.h"

class NcxWriterPrivate;

/**
 * Writes the NCX file of EPUB.
 */
class NcxWriter : public XmlWriter
{
public:
    NcxWriter(const ChmFile& chm, QIODevice& device, const QString& uuid);
    virtual ~NcxWriter();

    void execute(); ///< @override

protected:
    friend class NcxWriterPrivate;
    NcxWriterPrivate* d;
};

#endif // NCXWRITER_H
