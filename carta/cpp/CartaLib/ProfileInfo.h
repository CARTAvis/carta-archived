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
    enum class AggregateType {
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
     * Return the rest frequency in the image.
     * @return - the rest frequency in the image.
     */
    double getRestFrequency() const;

    /**
     * Return the rest frequency unit.
     * @return - the rest frequency unit.
     */
    QString getRestUnit() const;

    /**
     * Return a string indicating the profile spectral type
     * (for example, 'radio velocity' or 'optical velocity').
     * @return - a string representing the general category
     *      of spectral units.
     */
    QString getSpectralType() const;

    /**
     * Return the actual spectral unit such as 'MHz' or 'mm'.
     * @return - the spectral unit.
     */
    QString getSpectralUnit() const;

    /**
     * Equality operator.
     * @param rhs - the other ProfileInfo to compare to.
     * @return true if the other ProfileInfo matches this one; false otherwise.
     */
    bool operator==( const ProfileInfo& rhs ) const;

    /**
     * Inequality operator.
     * @param rhs - the other ProfileInfo to compare to.
     * @return true if the other ProfileInfo does not match this one; false otherwise.
     */
    bool operator!=( const ProfileInfo& rhs );

    /**
     * Set the method used for aggregating data.
     * @param aggType - the method used for aggregating data.
     */
    void setAggregateType( const AggregateType & aggType );

    /**
     * Set the rest frequency.
     * @param freq - the rest frequency used in the profile
     *      calculation.
     */
    void setRestFrequency( double freq );

    /**
     * Set a string indicating the general category of spectral
     * units.
     * @param specType - a string indicating the general category
     *      of spectral units.
     */
    void setSpectralType( const QString& specType );

    /**
     * Set the rest frequency units.
     * @param unit - the rest frequency units.
     */
    void setRestUnit( const QString& unit );

    /**
     * Set the spectral units such as "mm" or "GHz".
     * @param unit - the actual units to use in calculating the
     *      profile.
     */
    void setSpectralUnit( const QString& unit );

    virtual ~ProfileInfo();

protected:
    AggregateType m_aggregateType;
    double m_restFrequency;
    QString m_restUnit;
    QString m_spectralUnit;
    QString m_spectralType;
};

} // namespace Lib
} // namespace Carta
