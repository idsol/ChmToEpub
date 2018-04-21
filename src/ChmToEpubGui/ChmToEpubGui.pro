#-------------------------------------------------
#
# Project created by QtCreator 2012-07-16T01:35:19
# Gui Frontend for ChmToEpub.exe
#
#-------------------------------------------------

QT += core gui
TEMPLATE = app

TARGET = ChmToEpubGui
CONFIG(release, debug|release) {
    DESTDIR = ../../release
} else {
    DESTDIR = ../../debug
}

# intermediate files (*.o, moc_*, qrc_*, ui_*)
release {
    OBJECTS_DIR = build/obj/release
}
debug {
    OBJECTS_DIR = build/obj/debug
}
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

HEADERS += \
    MainWindow.h

SOURCES += \
    main.cpp \
    MainWindow.cpp

FORMS += \
    MainWindow.ui

OTHER_FILES += ../../deploy.cmd
