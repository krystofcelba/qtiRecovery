#-------------------------------------------------
#
# Project created by QtCreator 2011-03-19T19:02:16
#
#-------------------------------------------------

QT       += core gui

TARGET = qtiRecovery
TEMPLATE = app

INCLUDEPATH += -I./include -I/usr/local/include -I/opt/local/include
LIBS += -L/usr/lib -L/opt/local/lib  -lreadline -lusb-1.0


SOURCES += main.cpp\
        mainwindow.cpp \
    libirecovery.c

HEADERS  += mainwindow.h \
    libirecovery.h

FORMS    += mainwindow.ui
