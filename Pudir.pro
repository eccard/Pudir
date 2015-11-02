#/***********************************************************************************************
# * Pudir  - A pusher of directory to Android
# *   Version: 1.0
# * Copyright 2015 eccard
# * MIT License
# ************************************************************************************************/

#-------------------------------------------------
#
# Project created by QtCreator 2015-11-02T12:02:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pudir
TEMPLATE = app


SOURCES += sources/main.cpp\
           sources/mainwindow.cpp\
           sources/aboutdialog.cpp

HEADERS  += headers/mainwindow.h \
            headers/aboutdialog.h \

FORMS    += mainwindow.ui \
            aboutdialog.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    LICENSE

