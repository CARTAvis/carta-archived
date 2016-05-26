/***
 * List of available wavelength units.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <QStringList>

namespace Carta {

namespace Data {

class UnitsWavelength : public Carta::State::CartaObject {

public:

    /**
     * Returns the default wavelength unit.
     * @return the default wavelength unit.
     */
    QString getDefault() const;

    /**
     * Translates a possible case insensitive unit string into one
     * that matches the actual units exactly.
     * @param unitStr - an identifier for wavelength units that may not match
     *      in case.
     * @return - the actual wavelength units or an empty string if the units are
     *      not recognized.
     */
    QString getActualUnits( const QString& unitStr ) const;

    const static QString CLASS_NAME;
    const static QString UNIT_LIST;
    const static QString UNIT_MM;
    const static QString UNIT_UM;
    const static QString UNIT_NM;
    const static QString UNIT_ANGSTROM;
    virtual ~UnitsWavelength();

private:

    void _initializeDefaultState();
    void _initUnit( int * index, const QString& unit );

    static bool m_registered;
    UnitsWavelength( const QString& path, const QString& id );
    class Factory;


	UnitsWavelength( const UnitsWavelength& other);
	UnitsWavelength& operator=( const UnitsWavelength& other );
};
}
}
