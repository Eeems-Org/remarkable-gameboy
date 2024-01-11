#-------------------------------------------------
#
# Project created by QtCreator 2013-03-12T15:51:17
#
#-------------------------------------------------

QT       -= gui

TARGET = qboy
TEMPLATE = lib

DEFINES += LIBQBOY_LIBRARY

CONFIG -= debug
CONFIG += release

SOURCES += libqboy.cpp \
    z80.cpp \
    z80alu.cpp \
    z80mmu.cpp \
    z80register.cpp \
    gbgpu.cpp \
    gbkeypad.cpp \
    z80timer.cpp \
    z80mbc.cpp

HEADERS += libqboy.h\
        libqboy_global.h \
    z80.h \
    z80alu.h \
    z80mmu.h \
    z80register.h \
    gbgpu.h \
    gbkeypad.h \
    z80timer.h \
    z80mbc.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
