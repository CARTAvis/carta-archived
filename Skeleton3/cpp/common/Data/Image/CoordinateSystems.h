/***
 * List of all available CoordinateSystems.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/CartaLib.h"
#include <vector>

namespace Carta {
namespace Lib {
namespace PixelPipeline {
class IColormapNamed;
}
}
}

namespace Carta {

namespace Data {

class CoordinateSystems : public Carta::State::CartaObject {

public:

    /**
     * Returns true if the name represents a valid coordinate system; false, otherwise.
     * @param name a QString identifying a coordinate system.
     * @return true if the name represents a valid coordinate system; false, otherwise.
     */
    bool isCoordinateSystem( const QString& name ) const;

    /**
     * Returns a list of available coordinate systems.
     * @return a QStringList containing the names of available coordinate systems.
     */
    QStringList getCoordinateSystems() const;

    /**
     * Returns the index of the coordinate system matching the name or -1 if no such
     * coordinate system exists.
     * @param name - an identifier for a coordinate system.
     * @return the official enum entry of the coordinate system.
     */
    Carta::Lib::KnownSkyCS getIndex( const QString& name) const;

    /**
     * Returns the name of the default coordinate system.
     * @return the name of the default coordinate system.
     */
    QString getDefault() const;

    virtual ~CoordinateSystems();


    const static QString CLASS_NAME;
private:

    void _initializeDefaultState();
    void _initializeCallbacks();

    static bool m_registered;
    QMap < Carta::Lib::KnownSkyCS, QString > m_coordSystems;
    const static QString SYSTEM_NAME;
    const static QString COUNT;
    const static QString UNKNOWN;
    const static QString J2000;
    const static QString B1950;
    const static QString ICRS;
    const static QString GALACTIC;
    const static QString ECLIPTIC;
    CoordinateSystems( const QString& path, const QString& id );

    class Factory;

	CoordinateSystems( const CoordinateSystems& other);
	CoordinateSystems& operator=( const CoordinateSystems& other );
};

}
}
