/**
 *
 **/

#pragma once

#include <QtCore/qglobal.h>

#if defined ( CARTALIB_LIBRARY )
#  define CARTALIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CARTALIBSHARED_EXPORT Q_DECL_IMPORT
#endif
