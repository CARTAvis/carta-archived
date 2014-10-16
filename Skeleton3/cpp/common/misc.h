/*
 * Miscellaneous utilities, for which we have not found a good home.
 *
 */

#pragma once

#include <QString>
#include <QDebug>
#include <QTextDocumentFragment>

#include "CartaLib/HtmlString.h"

namespace Carta {
typedef Carta::Lib::TextFormat TextFormat;
typedef Carta::Lib::HtmlString HtmlString;
}

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

//namespace Carta {}

//template < typename T>
//QDebug operator<<(QDebug dbg, const T & v)
//{
//    dbg.nospace() << typeid(v).name() << ":" << Carta::toStr(v);
//    return dbg.space();
//}

