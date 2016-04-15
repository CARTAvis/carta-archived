/***
 * List the available methods for producing a profile.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
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

    const static QString CLASS_NAME;
    const static QString STAT_LIST;

    virtual ~ProfileStatistics();

private:

    const static QString STAT_MEAN;
    const static QString STAT_MEDIAN;
    const static QString STAT_SUM;
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
