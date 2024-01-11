#ifndef LIBQBOY_GLOBAL_H
#define LIBQBOY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBQBOY_LIBRARY)
#  define LIBQBOYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBQBOYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBQBOY_GLOBAL_H
