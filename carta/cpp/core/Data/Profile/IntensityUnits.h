/***
 * List of available intensity units.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <QStringList>

namespace Carta {

namespace Data {

class IntensityUnits : public Carta::State::CartaObject {

public:

    /**
     * Returns the default intensity unit.
     * @return the default intensity unit.
     */
    QString getDefault() const;

    /**
     * Translates a possible case insensitive unit string into one
     * that matches the actual units exactly.
     * @param unitStr - an identifier for intensity units that may not match
     *      in case.
     * @return - the actual intensity units or an empty string if the units are
     *      not recognized.
     */
    QString getActualUnits( const QString& unitStr ) const;

    const static QString CLASS_NAME;
    const static QString UNIT_LIST;

    virtual ~IntensityUnits();

private:

    const static QString NAME_PEAK;
    const static QString NAME_JYBEAM;
    const static QString NAME_JYSR;
    const static QString NAME_JYARCSEC;
    const static QString NAME_JY;
    const static QString NAME_KELVIN;

    void _initializeDefaultState();
    void _initUnit( int * index, const QString& name);

    static bool m_registered;
    IntensityUnits( const QString& path, const QString& id );
    class Factory;

	IntensityUnits( const IntensityUnits& other);
	IntensityUnits& operator=( const IntensityUnits& other );
};
}
}
