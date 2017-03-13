#include "AxisInfo.h"
#include <tuple>

namespace Carta {
namespace Lib {

AxisInfo::KnownType
AxisInfo::knownType() const
{
    return m_knownType;
}

AxisInfo &
AxisInfo::setKnownType( const AxisInfo::KnownType & knownType )
{
    m_knownType = knownType;
    return * this;
}

HtmlString
AxisInfo::shortLabel() const
{
    return m_shortLabel;
}

AxisInfo &
AxisInfo::setShortLabel( const HtmlString & shortLabel )
{
    m_shortLabel = shortLabel;
    return * this;
}

HtmlString
AxisInfo::longLabel() const
{
    return m_longLabel;
}

AxisInfo &
AxisInfo::setLongLabel( const HtmlString & longLabel )
{
    m_longLabel = longLabel;
    return * this;
}

AxisInfo &
AxisInfo::setUnit( const QString & suffix )
{
    m_unit = suffix;
    return * this;
}

//static AxisInfo makeAxisInfo(
//        const AxisInfo::KnownType & type,
//        QString longPlain, QString longHtml,
//        QString shortPlain, QString shortHtml,
//        QString unit)
//{
//    AxisInfo inf;
//    inf.setKnownType( type);
//    inf.setLongLabel( HtmlString(longPlain, longHtml));
//    inf.setShortLabel( HtmlString(shortPlain, shortHtml));
//    inf.setUnit( unit);
//    return inf;
//}

//AxisInfo AxisInfo::get_j2000_ra()
//{
//    return makeAxisInfo(
//                KnownType::DIRECTION_LON,
//                "Right ascension", "Right ascension",
//                "RA", "&alhpa;",
//                "deg");
//}

//AxisInfo AxisInfo::get_j2000_dec()
//{
//    return makeAxisInfo(
//                KnownType::DIRECTION_LAT,
//                "Declination", "Declination",
//                "DEC", "&delta;",
//                "deg");
//}


//QString shortenLongAxisLabel(QString longAxisName)
//{
//    return longAxisName;
//}


std::tuple<HtmlString, HtmlString> nicifyAxisLabel(QString rawAxisName)
{
    HtmlString longLabel, shortLabel;

    longLabel = HtmlString::fromPlain( rawAxisName);
    shortLabel = longLabel;

    return std::make_tuple( longLabel, shortLabel);
}

} // namespace Lib
} // namespace Carta
