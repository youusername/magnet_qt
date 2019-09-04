#-------------------------------------------------
#
# Project created by QtCreator 2019-08-27T17:56:41
#
#-------------------------------------------------

QT       += core gui network webenginewidgets xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = magnet_qt
TEMPLATE = app

#macx {
LIBS += -L/usr/local/opt/libxml2/lib -lxml2
INCLUDEPATH += /usr/local/opt/libxml2/include/libxml2

#}


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
CONFIG += c++11

SOURCES += \
        XTTableView.cpp \
        main.cpp \
        mainwindow.cpp \
        sidemodel.cpp

HEADERS += \
        XTTableView.h \
        mainwindow.h \
        sidemodel.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    icon512.png \
    rule.json
