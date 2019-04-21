QT -= gui
QT += network
DESTDIR = ../build

LIBS += -L../build -lredoclient
INCLUDEPATH += ../libredo

CONFIG += c++11 console

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
