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
#include <memory>

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
    typedef className Me; \
    typedef std::unique_ptr < Me > UniquePtr; \
    typedef std::shared_ptr < Me > SharedPtr; \
    typedef std::shared_ptr < const Me > ConstSharedPtr

namespace Carta
{
namespace Lib
{
///
/// \brief print out a message and abort
/// \param assertion message to print
/// \param file file where the error occurred
/// \param func function where the error occurred
/// \param line line number where the error occurred
/// \param extraMessage optional extra message
///
inline
void
cartaAssertAbort( const char * assertion, const char * file, const char * func, int line,
                  QString extraMessage = "")
{
    std::cerr << "ASSERT: " << assertion << "\n"
              << "....in: " << file << ":" << line << "\n"
              << "..func: " << func << "\n";
    if( ! extraMessage.isEmpty()) {
        std::cerr << "...msg: " << extraMessage.toStdString() << "\n";
    }
    std::cerr << "Aborting now!\n";
    std::abort();
}

/// known sky coordinate systems
/// \warning make sure the first entry is =0, and the last entry is Error
enum class KnownSkyCS
{
    Unknown = 0, ///< intended as a return value
    Default, ///< intended as input value (requested system = default)
    J2000,
    B1950,
    ICRS,
    Galactic,
    Ecliptic,
    Error ///< intended as an indication of error
};

/// convenience function to convert KnownSkyCS to int
int knownSkyCS2int( KnownSkyCS cs);

/// convenience function to convert int to KnownSkyCS
/// in case the input int is invalid, we return error
KnownSkyCS int2knownSkyCS( int cs);

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

}
}

/// You can use this macro to always perform the assert, but be careful because
/// it will coredump, leaving user upset... It's like the blue screen of death :)
#define CARTA_ASSERT_ALWAYS_X( cond, msg ) \
    ( ( ! ( cond ) ) ? Carta::Lib::cartaAssertAbort( # cond, __FILE__, __PRETTY_FUNCTION__, \
                                                __LINE__, msg ) : qt_noop() )

/// assert macro that is compiled out in release, with additional message
/// \note the CARTA_RUNTIME_CHECKS is defined in common.pri, depending on the type of build
#if CARTA_RUNTIME_CHECKS > 0
#define CARTA_ASSERT_X( cond, msg ) CARTA_ASSERT_ALWAYS_X( cond, msg )
#else
#define CARTA_ASSERT_X( cond, msg ) do { } while ((false) && (cond))
#endif

/// use this macro for regular development, it will be compiled out
/// in a release build
#define CARTA_ASSERT( cond ) CARTA_ASSERT_X( cond, "")

/// @todo move everything below to Carta::Lib namespace


/// sky formatting option
enum class SkyFormatting
{
    Sexagesimal,  /// < e.g. HH:MM:SS.SSS or DD:MM:SS.SSS
    Degrees,      /// < raw degrees
    Radians,      /// < raw radians
    Default       /// < pick whatever is appripriate for the current sky system
};

/// QString streaming helper (to output QStrings)
/*template < typename STREAM >
STREAM &
operator<< ( STREAM & stream, const QString & str )
{
    return stream << str.toStdString();
}*/

inline std::ostream &
operator<< (std::ostream& stream, const QString& str ){
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

/// just like std::swap, but only if v1 > v2
template < typename T >
inline void
swap_ordered( T & v1, T & v2 )
{
    if ( v1 > v2 ) {
        std::swap( v1, v2 );
    }
}

/// helper to convert double to base64
namespace Carta {
namespace Lib {
QString double2base64( double x);
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
