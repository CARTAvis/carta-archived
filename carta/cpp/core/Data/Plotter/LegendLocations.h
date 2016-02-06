/***
 * List of available legend locations.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <QStringList>

namespace Carta {

namespace Data {

class LegendLocations : public Carta::State::CartaObject {

public:

    /**
     * Returns the default legend location.
     * @param external - true if the legend is external to the plot; false
     *      otherwise.
     * @return the default legend location.
     */
    QString getDefaultLocation( bool external) const;

    /**
     * Translates a possible case insensitive legend location into one
     * that matches the actual legend location exactly.
     * @param locateStr - an identifier for a legend location that may not match
     *      in case.
     * @return - the legend location or an empty string if the location is
     *      not recognized.
     */
    QString getActualLocation( const QString& locateStr ) const;

    /**
     * Update the available locations for a legend based on whether the legend is
     * external or internal.
     * @param external - true if the legend is external to the plot; false if the legend
     *      should appear on the plot itself.
     */
    void setAvailableLocations( bool external );


    const static QString CLASS_NAME;
    const static QString RIGHT;
    const static QString BOTTOM;
    const static QString LEFT;
    const static QString TOP;

    //Additional locations for internal axes.
    const static QString TOP_LEFT;
    const static QString TOP_RIGHT;
    const static QString BOTTOM_LEFT;
    const static QString BOTTOM_RIGHT;

    virtual ~LegendLocations();

private:

    const static QString LOCATION_LIST;
    const static int EXTERNAL_COUNT;
    const static int INTERNAL_COUNT;

    QString _getActual( const QString& listName, const QString& positionStr ) const;
    void _initializeDefaultState();
    void _initItems();
    void _initValue( int * index, const QString& listName, const QString& name);

    static bool m_registered;
    LegendLocations( const QString& path, const QString& id );
    class Factory;

	LegendLocations( const LegendLocations& other);
	LegendLocations& operator=( const LegendLocations& other );
};
}
}
