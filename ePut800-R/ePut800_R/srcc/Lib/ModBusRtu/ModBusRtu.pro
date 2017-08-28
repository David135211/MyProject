#-------------------------------------------------
#
# Project created by QtCreator 2016-08-03T17:26:42
#
#-------------------------------------------------

QT       -= gui

TARGET = ModBusRtu
TEMPLATE = lib
DESTDIR = ../

DEFINES += MODBUSRTU_LIBRARY

SOURCES += modbusrtu.cpp \
    module_modbusrtu.cpp

HEADERS += modbusrtu.h\
        ModBusRtu_global.h \
    module_modbusrtu.h

QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-psabi
QMAKE_CXXFLAGS += -Wno-unused-but-set-variable

CONFIG+=debug_and_release
