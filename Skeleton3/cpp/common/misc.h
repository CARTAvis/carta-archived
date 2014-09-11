/*
 * Miscellaneous utilities, for which we have not found a good home.
 *
 */


#ifndef COMMON_MISC_H
#define COMMON_MISC_H

#include <QString>

/// text formatting
enum class TextFormat { Plain, Html };

/// known sky coordinate systems
enum class KnownSkyCS { Unknown, J2000, B1950, ICRS, Galactic, Ecliptic };

/// sky formatting option
enum class SkyFormat { Sexagecimal, Degrees, Radians };

/// QString streaming helper (to output QStrings)
template< typename STREAM>
STREAM& operator<< ( STREAM & stream, const QString & str )
{
    return stream << str.toStdString();
}

/// QString streaming helper (to read in QStrings)
template< typename STREAM>
STREAM& operator>> ( STREAM & stream, QString & str )
{
    std::string tmpstr;
    stream >> tmpstr;
    str = tmpstr.c_str();
    return stream;
}

/// clamp a value to be in range [v1..v2]
template <typename T>
inline
T clamp( const T & v, const T & v1, const T & v2)
{
    if( v < v1) return v1;
    if( v > v2) return v2;
    return v;
}

/// just like std::swap, but only if v1 > v2
template <typename T>
inline void
swap_ordered( T & v1, T & v2)
{
    if( v1 > v2) std::swap( v1, v2);
}





#endif // COMMON_MISC_H
