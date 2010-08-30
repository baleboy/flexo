# #####################################################################
# Automatically generated by qmake (2.01a) Wed Apr 14 21:58:00 2010
# #####################################################################
TEMPLATE = app
TARGET = flexo 
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qt

# Input
HEADERS += mainwindow.h \
    worker.h \
    commands.h \
    exitdialog.h \
    alarm.h \
    testalarm.h \
    dbusifadaptor.h \
    balancewindow.h \
    preferenceswindow.h \
    constants.h \
    preferences.h \
    timechangedialog.h \
    record.h \
    recordstack.h \
    writer.h \
    reader.h

FORMS += mainwindow.ui \
    exitdialog.ui \
    balancewindow.ui \
    preferenceswindow.ui \
    timechangedialog.ui
SOURCES += main.cpp \
    mainwindow.cpp \
    worker.cpp \
    commands.cpp \
    exitdialog.cpp \
    testalarm.cpp \
    dbusifadaptor.cpp \
    balancewindow.cpp \
    preferenceswindow.cpp \
    constants.cpp \
    preferences.cpp \
    timechangedialog.cpp \
    record.cpp \
    recordstack.cpp \
    writer.cpp \
    reader.cpp

RESOURCES += \
    resources.qrc

QT += dbus

maemo5 {
    QT += maemo5
    LIBS += -lalarm
    SOURCES += maemo5alarm.cpp
    HEADERS += maemo5alarm.h
}

unix {
    BINDIR = /opt/flexo
    DATADIR =/usr/share

    DEFINES += DATADIR=\"$$DATADIR\" PKGDATADIR=\"$$PKGDATADIR\"

    INSTALLS += target icon64 desktop service

    target.path =$$BINDIR

    desktop.path = $$DATADIR/applications/hildon
    desktop.files += ../data/flexo.desktop
    service.path = $$DATADIR/dbus-1/services
    service.files += ../data/com.balenet.flexo.service
    icon64.path = $$DATADIR/icons/hicolor/64x64/apps
    icon64.files += images/flexo.png
 }
