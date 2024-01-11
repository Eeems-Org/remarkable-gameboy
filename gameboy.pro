TEMPLATE = subdirs
SUBDIRS = \
    src \
    libqboy

libqboy.subdir = src/vendor/libqboy
src.subdir = src
src.depends = libqboy

INSTALLS += $$SUBDIRS
