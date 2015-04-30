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
    /// e.g. when converting to 'world' coordinates, what are the units...
    /// note that for sky coordinates this is not the unit that will be formatted
    QString
    unit() const { return m_unit; }

    AxisInfo &
    setUnit( const QString & unit );

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
