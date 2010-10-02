#-------------------------------------------------
#
# Project created by QtCreator 2010-09-28T14:13:46
#
#-------------------------------------------------

QT += core
QT += gui

TARGET = Prueba2
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += /media/Datos/Proyectos/OpenCV/cmake_binary_dir/OpenCV-2.1.0/include/opencv
LIBS += -L/usr/local/lib -lml -lcvaux -lhighgui -lcv -lcxcore

TEMPLATE = app

SOURCES += main.cpp \
    imagefuncs.cpp

HEADERS += \
    imagefuncs.h
