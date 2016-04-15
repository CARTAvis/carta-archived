#pragma once

#include <QString>

namespace Carta
{
namespace Lib
{
class ProfileInfo
{
public:
    /// Methods of summarizing profiles
    enum class AggregateType
    {
        MEAN,
        MEDIAN,
        SUM,
        FLUX_DENSITY,
        OTHER
    };

    AggregateType getAggregateType() const;

    /// set the aggregate type
    ProfileInfo & setAggregateType( const AggregateType & knownType );


    QString unit() const { return m_unit; }

    ProfileInfo & setUnit( const QString & unit );

protected:
    QString m_unit;
    AggregateType m_aggregateType = AggregateType::OTHER;
};



} // namespace Lib
} // namespace Carta
