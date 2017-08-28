#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T09:52:04
#
#-------------------------------------------------

QT       -= gui

TARGET = ModBusTcp
TEMPLATE = lib
DESTDIR = ../

DEFINES += MODBUSTCP_LIBRARY

SOURCES += modbustcp.cpp \
    module_modbustcp.cpp

HEADERS += modbustcp.h\
        ModBusTcp_global.h \
    module_modbustcp.h

QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-psabi
QMAKE_CXXFLAGS += -Wno-unused-but-set-variable

CONFIG+=debug_and_release
