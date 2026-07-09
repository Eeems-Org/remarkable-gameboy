TEMPLATE = subdirs
SUBDIRS = \
    src \
    oxide \
    libqboy

libqboy.subdir = src/vendor/libqboy
oxide.subdir = src/vendor/oxide
src.subdir = src
src.depends = libqboy oxide

INSTALLS += $$SUBDIRS
