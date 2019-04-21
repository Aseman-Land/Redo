QT -= gui
QT += network
DESTDIR = ../build

CONFIG += c++11 console

TARGET = redoclient
TEMPLATE = lib

DEFINES += REDOCLIENT_LIBRARY

SOURCES += \
    redoclient.cpp \
    redoparser.cpp

HEADERS += \
    redoclient.h \
    redoclient_global.h \
    redoparser.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

