/**
 *
 **/


#pragma once

#include "cartalib_global.h"
#include <QtGlobal>
#include <QString>
#include <QDebug>
#include <cassert>
#include <iostream>

/// all carta code lives here (or will eventually)
namespace Carta
{
/// code we want to share with plugins should go here
/// \par for example, image interface, metadata interface, hooks
namespace Lib
{ }

/// all code that is necessary for the core functionality should go here
/// but only parts that are not specific to desktop/server/scripted clients
/// and parts that plugins do not need to know about
namespace Core
{ }

/// desktop client related code
/// probably not really necessary
namespace Desktop
{ }

/// server client related code
/// probably not really necessary
namespace Server
{ }

/// scripted client related code
/// probably not really necessary
namespace Scripted
{ }
}

/// a macro that adds 'Me' and 'SharedPtr' typedefs to a class
#define CLASS_BOILERPLATE( className ) \
public: \
    typedef className              Me; \
    typedef std::shared_ptr < Me > SharedPtr


/// currently does nothing, but may serve as a place to keep global settings for
/// the whole library
class CARTALIBSHARED_EXPORT CartaLib
{

public:
    CartaLib();
};


namespace Carta { namespace Lib {
inline
void cartaAssert(const char *assertion, const char *file, const char * func, int line) {
    std::cerr << "ASSERT: " << assertion << "\n"
              << "....in: " << file << ":" << line << "\n"
              << ".func.: " << func << "\n"
              << "Aborting now, sorry\n";
    std::abort();
}
}}

/// You can use this macro to always perform the assert, but be careful because
/// it will coredump, leaving user upset... It's like the blue screen of death :)
#define CARTA_ASSERT_ALWAYS(cond) \
    ((!(cond)) ? Carta::Lib::cartaAssert(#cond,__FILE__,__PRETTY_FUNCTION__, __LINE__) : qt_noop())

#ifdef CARTA_RUNTIME_CHECKS
/// use this macro for regular development, it will be compiled out
/// in a release build
#define CARTA_ASSERT(cond) CARTA_ASSERT_ALWAYS(cond)
#else
#define CARTA_ASSERT(cond) qt_noop();
#endif

/// @todo move everything below to Carta::Lib namespace

/// known sky coordinate systems
enum class KnownSkyCS
{
    Unknown, J2000, B1950, ICRS, Galactic, Ecliptic
};

/// sky formatting option
enum class SkyFormatting
{
    Sexagesimal,  /// < e.g. HH:MM:SS.SSS or DD:MM:SS.SSS
    Degrees,      /// < raw degrees
    Radians,      /// < raw radians
    Default       /// < pick whatever is appripriate for the current sky system
};

/// QString streaming helper (to output QStrings)
template < typename STREAM >
STREAM &
operator<< ( STREAM & stream, const QString & str )
{
    return stream << str.toStdString();
}

/// QString streaming helper (to read in QStrings)
//template < typename STREAM >
//STREAM &
//operator>> ( STREAM & stream, QString & str )
//{
//    std::string tmpstr;
//    stream >> tmpstr;
//    str = tmpstr.c_str();
//    return stream;
//}

/// clamp a value to be in range [v1..v2]
template < typename T >
inline
T
clamp( const T & v, const T & v1, const T & v2 )
{
    if ( v < v1 ) {
        return v1;
    }
    if ( v > v2 ) {
        return v2;
    }
    return v;
}

/// just like std::swap, but only if v1 > v2
template < typename T >
inline void
swap_ordered( T & v1, T & v2 )
{
    if ( v1 > v2 ) {
        std::swap( v1, v2 );
    }
}

// ===-----------------------------------------------------------------------------===

/// debug helper to print std::vector...

template < typename T >
QDebug
operator<< ( QDebug dbg, const std::vector < T > & v )
{
    dbg.nospace() << "std::vector<" << typeid( v[0] ).name() << " x " << v.size() << ">=(";
    for ( size_t i = 0 ; i < v.size() ; i++ ) {
        dbg << ( i > 0 ? "," : "" ) << v[i];
        if ( i > 5 ) {
            dbg << " ...";
        }
    }
    dbg.nospace() << ")";
    return dbg.space();
}


