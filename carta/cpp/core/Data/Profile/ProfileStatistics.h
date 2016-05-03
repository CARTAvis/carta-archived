/***
 * List the available methods for producing a profile.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/ProfileInfo.h"
#include <QStringList>

namespace Carta {

namespace Data {



class ProfileStatistics : public Carta::State::CartaObject {

public:

    /**
     * Returns the default statistic used to compute profiles.
     * @return the default statistic used to compute profiles.
     */
    QString getDefault() const;

    /**
     * Translates a possible case insensitive statistic string into one
     * that matches the actual statistic exactly.
     * @param statStr - an identifier for a statistic that may not match
     *      in case.
     * @return - the actual statistic or an empty string if the statistic is
     *      not recognized.
     */
    QString getActualStatistic( const QString& statStr ) const;


    /**
     * Returns the aggregate type corresponding to the string description.
     * @param statStr - an identifier for a method of computing profiles.
     * @return - the corresponding aggregate type.
     */
    Carta::Lib::ProfileInfo::AggregateType getTypeFor( const QString& statStr ) const;

    /**
     * Returns a string description of the aggregate type.
     * @param agType - a method for generating profiles.
     * @return - a string descriptor of the method.
     */
    QString typeToString( Carta::Lib::ProfileInfo::AggregateType agType ) const;

    const static QString CLASS_NAME;
    const static QString STAT_LIST;

    virtual ~ProfileStatistics();

private:

    const static QString STAT_MEAN;
    const static QString STAT_MEDIAN;
    const static QString STAT_SUM;
    const static QString STAT_VARIANCE;
    const static QString STAT_MIN;
    const static QString STAT_MAX;
    const static QString STAT_RMS;
    const static QString STAT_FLUX_DENSITY;

    void _initializeDefaultState();
    void _initStat( int * index, const QString& name);

    static bool m_registered;
    ProfileStatistics( const QString& path, const QString& id );
    class Factory;

	ProfileStatistics( const ProfileStatistics& other);
	ProfileStatistics& operator=( const ProfileStatistics& other );
};
}
}
