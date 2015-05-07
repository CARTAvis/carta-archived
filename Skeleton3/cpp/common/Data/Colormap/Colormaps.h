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
namespace PixelPipeline {
class IColormapNamed;
}
}
}

namespace Carta {

namespace Data {

class Colormaps : public Carta::State::CartaObject {

public:

    /**
     * Returns true if the name represents a valid color map; false, otherwise.
     * @param name a QString identifying a color map.
     * @return true if the name represents a valid color map; false, otherwise.
     */
    bool isMap( const QString& name ) const;

    /**
     * Returns the colormap associated with the index.
     * @param index a integer representing a valid color map index.
     * @return the color map associated with the index.
     */
     std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> getColorMap( const QString& name ) const;

    /**
     * Returns a list of available color maps.
     * @return a QStringList containing the names of available color maps.
     */
    QStringList getColorMaps() const;

    virtual ~Colormaps();

    const static QString COLOR_LIST;
    const static QString CLASS_NAME;
private:

    void _initializeDefaultState();
    void _initializeCallbacks();
    QString _commandGetColorStops( const QString& params );

    std::vector < std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> > m_colormaps;

    static bool m_registered;
    const static QString COLOR_MAPS;
    const static QString COLOR_NAME;
    const static QString COLOR_MAP_COUNT;
    Colormaps( const QString& path, const QString& id );

    class Factory;


	Colormaps( const Colormaps& other);
	Colormaps operator=( const Colormaps& other );
};

}
}
