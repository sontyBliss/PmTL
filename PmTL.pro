#-------------------------------------------------
#
# Project created by QtCreator 2013-06-02T16:53:50
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PmTL
TEMPLATE = app


SOURCES += main.cpp\
        PmTL.cpp \
    pmtllogindialog.cpp \
    addplayerdialog.cpp

HEADERS  += PmTL.h \
    pmtllogindialog.h \
    addplayerdialog.h

FORMS    += mainwindow.ui \
    pmtllogindialog.ui \
    addplayerdialog.ui
