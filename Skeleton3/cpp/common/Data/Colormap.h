/***
 * Manages colormap settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

class ImageView;

namespace Carta {

namespace Data {

class Colormaps;
class IColoredView;
class Controller;

class Colormap : public QObject, public CartaObject {

    Q_OBJECT

public:
    /*
     * Add a a Controller that will respond to changes in this Colormap.
     * @param obj Controller the object that will change color..
     * @return true if the Controller was successfully added; false otherwise.
     */
    bool addController( std::shared_ptr<Controller> obj );

    /**
     * Clear existing state.
     */
    void clear();

    /**
     * Set the name of the current color map.
     * @param colorMapName a unique identifier for the color map.
     * @return error information if the color map was not successfully set.
     */
    QString setColorMap( const QString& colorMapName );
    virtual ~Colormap();
    const static QString CLASS_NAME;


private:
    QString _commandCacheColorMap( const QString& params );
    QString _commandCacheSize( const QString& params );
    QString _commandInterpolatedColorMap( const QString& params );
    QString _commandSetColorMap( const QString& params );
    QString _commandInvertColorMap( const QString& params );
    QString _commandReverseColorMap( const QString& params );
    QString _commandSetColorMix( const QString& params );
    QString _commandAddColoredObject( const QString& params );
    bool _processColorStr( const QString key, const QString colorStr, bool* valid );

    void _initializeDefaultState();
    void _initializeCallbacks();

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

    Colormap( const QString& path, const QString& id );

    class Factory;

    //Views controlled by this colormap
    QList<std::shared_ptr<Controller> > m_coloredViews;

    //Supported color maps
    static std::shared_ptr<Colormaps> m_colors;


    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    StateInterface m_stateMouse;

	Colormap( const Colormap& other);
	Colormap operator=( const Colormap& other );
};
}
}
