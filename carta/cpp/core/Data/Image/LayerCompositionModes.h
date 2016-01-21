/***
 * List of possible layer composition modes that can be applied to the image.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>



namespace Carta {

namespace Data {

class LayerCompositionModes : public Carta::State::CartaObject {

public:

    /**
     * Return the default stack layer composition mode.
     * @return - the default stack layer composition mode.
     */
    QString getDefault() const;

    /**
     * Returns true if the indicated layer composition mode supports transparency.
     * @param modeName - an identifier for a layer composition mode.
     * @return - true if the layer composition mode supports transparency; false
     *      otherwise.
     */
    bool isAlphaSupport( const QString& modeName ) const;

    /**
     * Returns true if the indicated layer composition mode supports a color filter.
     * @param modeName - an identifier for a layer composition mode.
     * @return - true if the layer composition mode supports a color filter; false
     *      otherwise.
     */
    bool isColorSupport( const QString& modeName ) const;

    /**
     * Returns true if the name represents a valid data transform; false, otherwise.
     * @param name a QString identifying a data transform.
     * @param actualName the actual name of the transform in case of a case mismatch
     * @return true if the name represents a valid data tranform; false, otherwise.
     */
    bool isCompositionMode( const QString& name, QString& actualName ) const;

    /**
     * Returns a list of available layer composition modes.
     * @return a QStringList containing the names of available layer composition modes.
     */
    QStringList getLayerCompositionModes() const;

    virtual ~LayerCompositionModes();

    const static QString CLASS_NAME;
    const static QString NONE;
    const static QString ALPHA;
    const static QString PLUS;

private:
    void _initializeDefaultState();

    std::vector < QString > m_modes;

    static bool m_registered;
    const static QString MODES;

    LayerCompositionModes( const QString& path, const QString& id );

    class Factory;


	LayerCompositionModes( const LayerCompositionModes& other);
	LayerCompositionModes& operator=( const LayerCompositionModes & other );
};

}
}
