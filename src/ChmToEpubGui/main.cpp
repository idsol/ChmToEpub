#include <QtGui/QApplication>
#include <QTextCodec>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // core: codec
    QTextCodec* zhCN = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForCStrings(zhCN);
    QTextCodec::setCodecForTr(zhCN);

    // decoration: font
    QFont appFont(app.font());
    appFont.setFamily("Tahoma,Î¢ÈíÑÅºÚ,ËÎÌå");
    appFont.setPointSize(10);
    app.setFont(appFont);

    // decoration: style
    app.setStyle("CleanLooks");

    MainWindow mw;
    mw.show();

    return app.exec();
}
