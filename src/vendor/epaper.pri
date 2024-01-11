linux-oe-g++ {
    LIBS += -L$$PWD/epaper -lqsgepaper
    INCLUDEPATH += $$PWD/epaper
    DEFINES += EPAPER
}
