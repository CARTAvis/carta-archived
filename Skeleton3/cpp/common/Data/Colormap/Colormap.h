/***
 * Manages colormap settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "Data/LinkableImpl.h"

class ImageView;

namespace Carta {
namespace Lib {
namespace PixelPipeline {
class IColormapNamed;
}
}
}

namespace Carta {

namespace Data {

class Colormaps;
class IColoredView;
class Controller;
class TransformsData;

class Colormap : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    virtual QString addLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;
    virtual QString removeLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Clear existing state.
     */
    void clear();

    /**
     * Return a string representing the colormap state of a particular type.
     * @param sessionId - an identifier for the user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding colormap state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Force a flush of state to the client.
     */
    void refreshState();

    /**
     * Set the name of the current color map.
     * @param colorMapName a unique identifier for the color map.
     * @return error information if the color map was not successfully set.
     */
    QString setColorMap( const QString& colorMapName );


    /**
     * Set the name of the data transform.
     * @param transformString a unique identifier for a data transform.
     * @return error information if the data transfrom was not set.
     */
    QString setDataTransform( const QString& transformString );

    /**
     * Set the gamma color map parameter.
     * @param gamma a parameter for color mapping.
     * @return error information if gamma could not be set.
     */
    QString setGamma( double gamma );

    /**
     * Returns the selected controller for this colormap.
     * @return the map's selected controller.
     */
    Controller* getControllerSelected() const;
    virtual ~Colormap();
    const static QString CLASS_NAME;

signals:
    void colorMapChanged( Colormap* map );

private slots:
    void _setColorProperties( Controller* target );

private:
    QString _commandCacheColorMap( const QString& params );
    QString _commandCacheSize( const QString& params );
    QString _commandInterpolatedColorMap( const QString& params );
    QString _commandSetColorMap( const QString& params );
    QString _commandInvertColorMap( const QString& params );
    QString _commandReverseColorMap( const QString& params );
    QString _commandSetColorMix( const QString& params );

    bool _processColorStr( const QString key, const QString colorStr, bool* valid );

    void _initializeDefaultState();
    void _initializeCallbacks();
    void _initializeStatics();

    static bool m_registered;
    const static QString COLOR_MAP_NAME;
    const static QString REVERSE;
    const static QString INVERT;
    const static QString RED_PERCENT;
    const static QString GREEN_PERCENT;
    const static QString BLUE_PERCENT;
    const static QString COLORED_OBJECT;
    const static QString COLOR_MIX;
    const static QString COLOR_MIX_RED;
    const static QString COLOR_MIX_GREEN;
    const static QString COLOR_MIX_BLUE;
    const static QString CACHE_SIZE;
    const static QString INTERPOLATED;
    const static QString CACHE;
    const static QString SCALE_1;
    const static QString SCALE_2;
    const static QString GAMMA;
    const static QString TRANSFORM_IMAGE;
    const static QString TRANSFORM_DATA;

    Colormap( const QString& path, const QString& id );

    class Factory;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Supported color maps
    static Colormaps* m_colors;

    //Supported data transforms
    static TransformsData* m_dataTransforms;


    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;
    int m_significantDigits;

	Colormap( const Colormap& other);
	Colormap operator=( const Colormap& other );
};
}
}
