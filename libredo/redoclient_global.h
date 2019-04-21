#ifndef REDOCLIENT_GLOBAL_H
#define REDOCLIENT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(REDOCLIENT_LIBRARY)
#  define REDOCLIENT_EXPORT Q_DECL_EXPORT
#else
#  define REDOCLIENT_EXPORT Q_DECL_IMPORT
#endif

#endif // REDOCLIENT_GLOBAL_H