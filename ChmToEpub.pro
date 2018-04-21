#-------------------------------------------------
#
# Project created by QtCreator 2012-07-16T01:35:19
#
#-------------------------------------------------

QT += core xml
CONFIG += console
TEMPLATE = app

TARGET = ChmToEpub
CONFIG(release, debug|release) {
    DESTDIR = release
} else {
    DESTDIR = debug
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

INCLUDEPATH += . src src/epub src/chm lib/chmplus lib/quazip lib/htmltidy
LIBS += ./lib/chmplus/libchmplus.a ./lib/quazip/libquazip.lib ./lib/htmltidy/libhtmltidy.a

HEADERS += \
    application.h \
    src/epub/XmlWriter.h \
    src/epub/NcxWriter.h \
    src/epub/EpubWriter.h \
    src/epub/OpfWriter.h

SOURCES += \
    main.cpp \
    application.cpp \
    src/epub/XmlWriter.cpp \
    src/epub/NcxWriter.cpp \
    src/epub/EpubWriter.cpp \
    src/epub/OpfWriter.cpp

OTHER_FILES += deploy.cmd
