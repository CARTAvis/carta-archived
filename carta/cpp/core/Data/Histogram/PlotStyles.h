/***
 * List of possible 2D plot styles such as line drawing or fill drawing.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class PlotStyles : public Carta::State::CartaObject {

public:

    /**
     * Return the official name of the plot style based on a name that may be a case
     * insensitive match.
     * @param styleStr - a case insensitive name for a plot style.
     * @return - the official name for the plot style or an empty string if the passed
     *      in plot style was not recognized.
     */
    QString getActualPlotStyle( const QString& styleStr ) const;

    /**
     * Return the default plot style.
     * @return - the default plot style.
     */
    QString getDefault() const;

    /**
     * Returns a list of available plot styles.
     * @return - a QStringList containing the names of available plot styles.
     */
    QStringList getPlotStyles() const;


    virtual ~PlotStyles();

    const static QString CLASS_NAME;
    static const QString PLOT_STYLE_LINE;
    static const QString PLOT_STYLE_OUTLINE;
    static const QString PLOT_STYLE_FILL;

private:

    static const QString PLOT_STYLES;


    void _initializeDefaultState();
    void _initializeCallbacks();

    static bool m_registered;

    PlotStyles( const QString& path, const QString& id );

    class Factory;


	PlotStyles( const PlotStyles& other);
	PlotStyles& operator=( const PlotStyles & other );
};

}
}
