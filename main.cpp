#include <QtCore>
#include "application.h"

int main(int argc, char *argv[])
{
    Application app(argc, argv);

    // core: codec
    QTextCodec* zhCN = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForCStrings(zhCN);
    QTextCodec::setCodecForTr(zhCN);

    return app.exec();
}
