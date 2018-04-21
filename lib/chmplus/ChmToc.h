#ifndef CHMTOC_H
#define CHMTOC_H

#include <QObject>

/**
 * Represents the tree of topics with titles and paths.
 */
class Q_DECL_EXPORT ChmToc : public QObject
{
    Q_OBJECT

public:
    explicit ChmToc(QObject* parent = 0);
    virtual ~ChmToc();

    QString title; ///< Title of the tree node.
    QString path; ///< Path to file under given topic or empty
    ChmToc* parent; ///< Pointer to parent tree node, nil if no parent
    QList<ChmToc*> children; ///< children node
};

#endif // CHMTOC_H
