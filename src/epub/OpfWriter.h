#ifndef OPFWRITER_H
#define OPFWRITER_H

#include "XmlWriter.h"

class OpfWriterPrivate;

class OpfWriter : public XmlWriter
{
public:
    OpfWriter(const ChmFile& chm, QIODevice& device, const QString& uuid);
    virtual ~OpfWriter();

    void execute(); ///< @override

protected:
    friend class OpfWriterPrivate;
    OpfWriterPrivate* d;
};

#endif // OPFWRITER_H
