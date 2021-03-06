# -------------------------------------------------
# Project created by QtCreator 2010-10-06T11:20:39
# -------------------------------------------------
QT += core \
    gui \
    sql

TARGET = Keepcon

CONFIG += console

CONFIG -= app_bundle

INCLUDEPATH += "C:\Program Files (x86)\OpenCV2.0\include\opencv" \
                "C:\Program Files\Lemur\Lemur 4.12\include"


LIBS += "C:\Program Files (x86)\OpenCV2.0\lib\libcv200.dll.a" \
    "C:\Program Files (x86)\OpenCV2.0\lib\libcvaux200.dll.a" \
    "C:\Program Files (x86)\OpenCV2.0\lib\libcxcore200.dll.a" \
    "C:\Program Files (x86)\OpenCV2.0\lib\libcxts200.dll.a" \
    "C:\Program Files (x86)\OpenCV2.0\lib\libhighgui200.dll.a" \
    "C:\Program Files (x86)\OpenCV2.0\lib\libml200.dll.a" \
    "C:\Program Files\Lemur\Lemur 4.12\lib\lemur.lib"

TEMPLATE = app

OBJECTS_DIR = obj

SOURCES += src/main.cpp \
    src/imagefuncs.cpp \
    src/features.cpp \
    src/utils.cpp \
    src/imagehash.cpp \
    src/cbir.cpp

HEADERS += src/imagefuncs.h \
    src/features.h \
    src/vector3d.h \
    src/utils.h \
    src/imagehash.h \
    src/cbir.h
