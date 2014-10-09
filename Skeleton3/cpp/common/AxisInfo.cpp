#include "AxisInfo.h"

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
