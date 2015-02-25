#-------------------------------------------------
#
# Project created by QtCreator 2014-11-23T01:46:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Shannon-Fano-Kodierung
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    sfcodec.cpp \
    symbol.cpp \
    sflist.cpp

HEADERS  += mainwindow.h \
    sfcodec.h \
    symbol.h \
    sflist.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    README.txt \
    LICENSE.txt
