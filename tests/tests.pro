QT += testlib network
QT -= gui

DESTDIR = ../build
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

LIBS += -L../build -lredoclient
INCLUDEPATH += ../libredo
INCLUDEPATH += ../redoserver

TEMPLATE = app

SOURCES +=  tst_testredo.cpp \
    ../redoserver/redoserver.cpp \
    ../redoserver/config.cpp

HEADERS += \
    ../redoserver/redoserver.h \
    ../redoserver/config.h
