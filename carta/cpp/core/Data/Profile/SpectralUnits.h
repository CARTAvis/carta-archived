/***
 * List of available spectral units.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <QStringList>

namespace Carta {

namespace Data {

class SpectralUnits : public Carta::State::CartaObject {

public:

    /**
     * Returns the default spectral unit.
     * @return the default spectral unit.
     */
    QString getDefault() const;

    /**
     * Translates a possible case insensitive unit string into one
     * that matches the actual units exactly.
     * @param unitStr - an identifier for spectral units that may not match
     *      in case.
     * @return - the actual spectral units or an empty string if the units are
     *      not recognized.
     */
    QString getActualUnits( const QString& unitStr ) const;

    const static QString CLASS_NAME;
    const static QString UNIT_LIST;
    virtual ~SpectralUnits();

private:

    const static QString NAME_VELOCITY_RADIO;
    const static QString NAME_VELOCITY_OPTICAL;
    const static QString NAME_FREQUENCY;
    const static QString NAME_WAVELENGTH;
    const static QString NAME_CHANNEL;
    const static QString UNIT_MS;
    const static QString UNIT_KMS;
    const static QString UNIT_HZ;
    const static QString UNIT_MHZ;
    const static QString UNIT_GHZ;
    const static QString UNIT_MM;
    const static QString UNIT_UM;
    const static QString UNIT_NM;
    const static QString UNIT_ANGSTROM;

    void _initializeDefaultState();
    void _initUnit( int * index, const QString& name, const QString& unit );

    static bool m_registered;
    SpectralUnits( const QString& path, const QString& id );
    class Factory;


	SpectralUnits( const SpectralUnits& other);
	SpectralUnits& operator=( const SpectralUnits& other );
};
}
}
