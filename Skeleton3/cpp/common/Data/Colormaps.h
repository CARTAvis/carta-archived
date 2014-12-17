/***
 * List of all available colormaps.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>

namespace Carta {
    namespace Lib {
        class IColormapScalar;
    }
}


class Colormaps : public CartaObject {

public:
    /**
     * Returns the index of the colormap.
     * @param name a QString identifying a colormap.
     * @return the index of the colormap in the list.
     */
    int getIndex( const QString& name ) const;
    /**
     * Returns the colormap associated with the index.
     * @param index a integer representing a valid color map index.
     * @return the color map associated with the index.
     */
    std::shared_ptr<Carta::Lib::IColormapScalar> getColorMap( int index ) const;
    const static QString COLOR_LIST;
    virtual ~Colormaps();
    const static QString CLASS_NAME;
private:
    void _initializeDefaultState();
    void _initializeCallbacks();
    QString _commandGetColorStops( const QString& params );

    std::vector < std::shared_ptr<Carta::Lib::IColormapScalar> > m_colormaps;

    static bool m_registered;
    const static QString COLOR_MAPS;
    const static QString COLOR_INDEX;
    const static QString COLOR_MAP_COUNT;
    Colormaps( const QString& path, const QString& id );

    class Factory;


	Colormaps( const Colormaps& other);
	Colormaps operator=( const Colormaps& other );
};
