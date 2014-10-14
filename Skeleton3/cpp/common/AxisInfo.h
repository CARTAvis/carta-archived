#pragma once

#include "CartaLib/HtmlString.h"

namespace Carta
{
namespace Lib
{
/// description of an axis in an image
class AxisInfo
{
public:
    /// axis types we recognize
    enum class KnownType
    {
        DIRECTION_LON, /// < direction, longitude-ish
        DIRECTION_LAT, /// < direction, latitude-ish
        SPECTRAL,      /// < spectral axis
        STOKES,        /// < stokes axis
        TABULAR,       /// < not sure but casacore uses it, maybe it's important?
        QUALITY,       /// < not sure but casacore uses it, maybe it's important?
        OTHER
    };

    /// return the type of this axis
    KnownType
    knownType() const;

    /// set the known type
    AxisInfo &
    setKnownType( const KnownType & knownType );

    /// get the short label for the axis
    /// suitable for cursor labels
    HtmlString
    shortLabel() const;

    AxisInfo &
    setShortLabel( const HtmlString & shortLabel );

    /// get the long label for the axis
    /// suitable for plot captions
    HtmlString
    longLabel() const;

    AxisInfo &
    setLongLabel( const HtmlString & longLabel );

    /// unit of the axis
    QString
    unit() const { return m_unit; }

    AxisInfo &
    setUnit( const QString & unit );

    /// helpers for known axis types
    static AxisInfo get_j2000_ra();
    static AxisInfo get_j2000_dec();
    static AxisInfo get_b1950_ra();
    static AxisInfo get_b1950_dec();
    static AxisInfo get_icrs_ra();
    static AxisInfo get_icrs_dec();
    static AxisInfo get_galactic_ra();
    static AxisInfo get_galactic_dec();
    static AxisInfo get_ecliptic_ra();
    static AxisInfo get_ecliptic_dec();

protected:
    HtmlString m_shortLabel, m_longLabel;
    QString m_unit;
    KnownType m_knownType = KnownType::OTHER;
};

///// this function attempts to shorten a long axis name
//QString shortenLongAxisLabel( QString longAxisName);

/// this function takes a raw-text name of an axis and tries
/// to produce an HtmlString for both long and short name
std::tuple<HtmlString,HtmlString> nicifyAxisLabel( QString rawAxisName);

} // namespace Lib
} // namespace Carta
