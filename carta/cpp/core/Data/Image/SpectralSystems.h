/***
 * List of all available SpectralSystems.
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

class SpectralSystems : public Carta::State::CartaObject {

public:

    /**
     * Translates a non case sensitive spectral system into one
     * that is case sensitive.
     * @param system - a spectral system that may not have the proper capitalization.
     * @return - a recognized spectral system or an empty string if the spectral
     *      system is not recognized.
     */
    QString getSpectralSystem( const QString& system ) const;

    /**
     * Returns a list of available spectral systems.
     * @return a QStringList containing the names of available spectral systems.
     */
    QStringList getSpectralSystems() const;

    /**
     * Returns the index of the spectral system matching the name or -1 if no such
     * spectral system exists.
     * @param name - an identifier for a spectral system.
     * @return the official enum entry of the spectral system.
     */
    Carta::Lib::KnownSpecCS getIndex( const QString& name) const;

    /**
     * Returns the indices of supported spectral systems.
     * @return a list of indices of the recognized spectral systems.
     */
    QList<Carta::Lib::KnownSpecCS> getIndices() const;

    /**
     * Returns the name of the spectral string matching the identifier.
     * @param skyCS - an index of a spectral system.
     * @return the name of the spectral system or an empty string if no such spectral
     *      system exists.
     */
    QString getName(Carta::Lib::KnownSpecCS specCS ) const;

    /**
     * Returns the name of the default spectral system.
     * @return the name of the default spectral system.
     */
    QString getDefault() const;

    /**
     * Return the default enumerated spectral system type.
     * @return - the default enumerated spectral system type.
     */
    Carta::Lib::KnownSpecCS  getDefaultType() const;

    virtual ~SpectralSystems();


    const static QString CLASS_NAME;
private:

    void _initializeDefaultState();
    void _initializeCallbacks();

    static bool m_registered;
    QMap < Carta::Lib::KnownSpecCS, QString > m_specSystems;
    const static QString SYSTEM_NAME;
    const static QString COUNT;
    const static QString NATIVE;
    const static QString FREQUENCY;
    const static QString RADIOVELOCITY;
    const static QString OPTICALVELOCITY;
    const static QString CHANNEL;

    SpectralSystems( const QString& path, const QString& id );

    class Factory;

	SpectralSystems( const SpectralSystems& other);
	SpectralSystems& operator=( const SpectralSystems& other );
};

}
}
