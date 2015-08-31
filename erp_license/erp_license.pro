#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T23:05:21
#
#-------------------------------------------------

QT       += core gui
QT 	+=sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = erp_license
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbman.cpp

HEADERS  += mainwindow.h \
    dbman.h

FORMS    += mainwindow.ui

win32: LIBS += -lQt5Sql
