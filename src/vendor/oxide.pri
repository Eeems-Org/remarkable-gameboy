linux-oe-g++ {
    isEmpty(SYSROOT){
        SYSROOT = $$(SYSROOT)
    }
    isEmpty(SYSROOT){
        SYSROOT = $$(OECORE_TARGET_SYSROOT)
    }
    isEmpty(SYSROOT){
        SYSROOT = $$(SDKTARGETSYSROOT)
    }
    isEmpty(SYSROOT){
        SYSROOT = $$(PKG_CONFIG_SYSROOT_DIR)
    }
    isEmpty(SYSROOT){
        error("SYSROOT not set")
    }
    OXIDE_DIR = $$PWD/oxide/home/root/.vellum
    INCLUDEPATH += $$OXIDE_DIR/include
    LIBS += -L$$OXIDE_DIR/lib -loxide -lblight -lsentry
    LIBS += -L$$SYSROOT/usr/lib/plugins/scenegraph
    LIBS += -L$$SYSROOT/usr/plugins/scenegraph
    LIBS += -lqsgepaper
    QMAKE_LFLAGS += -Wl,-rpath-link,$$OXIDE_DIR/lib
    QMAKE_LFLAGS += -Wl,-rpath-link,$$SYSROOT/usr/lib/plugins/scenegraph
    DEFINES += EPAPER
    QMAKE_RPATHDIR += $$OXIDE_DIR/lib
    QMAKE_RPATHDIR += /usr/lib/plugins/scenegraph
}
