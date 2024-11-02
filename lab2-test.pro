QT       += core gui widgets testlib

TARGET = lab2-test
TEMPLATE = app

CONFIG += c++11 testcase

SOURCES += 

HEADERS += 

INCLUDEPATH += \
    src \
    src/core \
    src/ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target