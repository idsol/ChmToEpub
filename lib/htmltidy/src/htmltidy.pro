#-------------------------------------------------
#
# HTML tidy lib taken from sigil 0.53 source package.
# @sa tidy.sourceforge.net
#
#-------------------------------------------------

QT += core
QT -= gui

TARGET = htmltidy
CONFIG += qt
CONFIG -= app_bundle

TEMPLATE = lib
# header files to be exposed: tidy.h, tidyenum.h
HEADERS += *.h
SOURCES += *.c
