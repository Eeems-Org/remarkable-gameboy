TEMPLATE = aux

contains(OXIDE_ARCH, aarch64) {
    OXIDE_URL = https://github.com/Eeems-Org/oxide/releases/download/3.0/build-aarch64.zip
} else:contains(OXIDE_ARCH, armv7) {
    OXIDE_URL = https://github.com/Eeems-Org/oxide/releases/download/3.0/build-armv7.zip
} else:contains(QT_ARCH, arm64) {
    OXIDE_URL = https://github.com/Eeems-Org/oxide/releases/download/3.0/build-aarch64.zip
} else {
    OXIDE_URL = https://github.com/Eeems-Org/oxide/releases/download/3.0/build-armv7.zip
}

OXIDE_DIR = $$PWD

download_oxide.target = $$OXIDE_DIR/.extracted
download_oxide.commands = \
    mkdir -p $$OXIDE_DIR && \
    if which wget >/dev/null 2>&1; then \
        wget -q -O $$OXIDE_DIR/build.zip $$OXIDE_URL; \
    elif which curl >/dev/null 2>&1; then \
        curl -sL -o $$OXIDE_DIR/build.zip $$OXIDE_URL; \
    else \
        echo "ERROR: Neither wget nor curl found. Cannot download liboxide."; \
        exit 1; \
    fi && \
    unzip -qo $$OXIDE_DIR/build.zip -d $$OXIDE_DIR && \
    rm $$OXIDE_DIR/build.zip && \
    touch $$OXIDE_DIR/.extracted

QMAKE_EXTRA_TARGETS += download_oxide
PRE_TARGETDEPS += $$OXIDE_DIR/.extracted
