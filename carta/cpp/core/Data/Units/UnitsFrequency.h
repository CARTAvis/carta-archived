/***
 * List of available frequency units.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <QStringList>

namespace Carta {

namespace Data {

class UnitsFrequency : public Carta::State::CartaObject {

public:

    /**
     * Returns the default frequency unit.
     * @return the default frequency unit.
     */
    QString getDefault() const;

    /**
     * Translates a possible case insensitive unit string into one
     * that matches the actual units exactly.
     * @param unitStr - an identifier for frequency units that may not match
     *      in case.
     * @return - the actual frequency units or an empty string if the units are
     *      not recognized.
     */
    QString getActualUnits( const QString& unitStr ) const;

    const static QString CLASS_NAME;
    const static QString UNIT_HZ;
    const static QString UNIT_MHZ;
    const static QString UNIT_GHZ;
    const static QString UNIT_LIST;
    virtual ~UnitsFrequency();

private:

    void _initializeDefaultState();
    void _initUnit( int * index, const QString& unit );

    static bool m_registered;
    UnitsFrequency( const QString& path, const QString& id );
    class Factory;


	UnitsFrequency( const UnitsFrequency& other);
	UnitsFrequency& operator=( const UnitsFrequency& other );
};
}
}
