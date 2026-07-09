TARGET = gameboy
VERSION = 1.0

QT += gui
QT += quick
QT += dbus

CONFIG += ltcg
CONFIG += c++11
CONFIG += c++17
CONFIG += c++20
CONFIG += c++latest
CONFIG += qml_debug
CONFIG += qtquickcompiler
CONFIG += qmltypes
QT += gui-private

QML_IMPORT_NAME = codes.eeems.gameboy
QML_IMPOrt_PATH += .
QML_IMPORT_MAJOR_VERSION = 1

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x051510

SOURCES += \
        gameboy.cpp \
        main.cpp

CONFIG(debug, debug|release){
    LIBS += -lunwind
    contains(DEFINES, SANITIZER){
        QMAKE_LFLAGS += -fno-omit-frame-pointer
        QMAKE_LFLAGS += -fsanitize-recover=address

        QMAKE_LFLAGS += -fsanitize=address
        QMAKE_LFLAGS += -fsanitize=leak
        # QMAKE_LFLAGS += -fsanitize=thread # Incompatible with address and leak
        QMAKE_LFLAGS += -fsanitize=undefined
        QMAKE_LFLAGS += -fsanitize=pointer-compare
        QMAKE_LFLAGS += -fsanitize=pointer-subtract
    }
}

linux-oe-g++ {
    QMAKE_RPATHDIR += /lib /usr/lib /home/root/.vellum/lib
}

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QMAKE_LFLAGS += -flto
QMAKE_CFLAGS += -fPIC
QMAKE_CXXFLAGS += -fPIC

linux-oe-g++ {
    target.path = /home/root/.vellum/bin
} else {
    target.path = /usr/bin
}
INSTALLS += target
linux-oe-g++ {
    applications.files = $$_PRO_FILE_PWD_/gameboy.oxide
    applications.path = /home/root/.local/share/applications/
    INSTALLS += applications
}

HEADERS += \
    gameboy.h \
    gameboythread.h

RESOURCES += \
    qml.qrc

include(vendor/oxide.pri)
include(vendor/libqboy.pri)
