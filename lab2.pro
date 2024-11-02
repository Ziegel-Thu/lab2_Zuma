QT       += core gui widgets

TARGET = lab2
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    src/main.cpp

HEADERS += 

INCLUDEPATH += \
    src \
    src/core \
    src/ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target