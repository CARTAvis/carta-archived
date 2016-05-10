/***
 * List of possible plot styles for profile curves such as a continuous line connecting
 * points or a step function.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class ProfilePlotStyles : public Carta::State::CartaObject {

public:

    /**
     * Return the official name of the plot style based on a name that may be a case
     * insensitive match.
     * @param styleStr - a case insensitive name for a plot style.
     * @return - the official name for the plot style or an empty string if the passed
     *      in plot style was not recognized.
     */
    QString getActualStyle( const QString& styleStr ) const;

    /**
     * Return the default plot style.
     * @return - the default plot style.
     */
    QString getDefault() const;

    /**
     * Returns a list of available plot styles.
     * @return - a QStringList containing the names of available plot styles.
     */
    QStringList getProfilePlotStyles() const;


    virtual ~ProfilePlotStyles();

    const static QString CLASS_NAME;
    static const QString PLOT_STYLE_LINE;
    static const QString PLOT_STYLE_STEP;

private:

    static const QString PLOT_STYLES;


    void _initializeDefaultState();

    static bool m_registered;

    ProfilePlotStyles( const QString& path, const QString& id );

    class Factory;


	ProfilePlotStyles( const ProfilePlotStyles& other);
	ProfilePlotStyles& operator=( const ProfilePlotStyles & other );
};

}
}
