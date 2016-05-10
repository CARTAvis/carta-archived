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
        RMS,
        SUM,
        FLUX_DENSITY,
        VARIANCE,
        MIN,
        MAX,
        OTHER
    };

    /**
     * Constructor.
     */
    ProfileInfo();

    /**
     * Return the method to be used for aggregating data.
     * @return - the method used for aggregating data.
     */
    AggregateType getAggregateType() const;

    /**
     * Return the rest frequency used in generating the profile.
     * @return - the rest frequency used in generating the profile.
     */
    double getRestFrequency() const;

    /**
     * Return the rest frequency unit.
     * @return - the rest frequency unit.
     */
    QString getRestUnit() const;

    /**
     * Set the method used for aggregating data.
     * @param aggType - the method used for aggregating data.
     */
    ProfileInfo & setAggregateType( const AggregateType & aggType );

    virtual ~ProfileInfo();

protected:
    AggregateType m_aggregateType = AggregateType::OTHER;
    double m_restFrequency;
    QString m_restUnit;
};

} // namespace Lib
} // namespace Carta
