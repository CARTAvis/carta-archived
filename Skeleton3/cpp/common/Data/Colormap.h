/***
 * Manages colormap settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

class ImageView;
class Colormaps;
class IColoredView;


class Colormap : public QObject, public CartaObject {

    Q_OBJECT

public:
    /*
     * Add an object to the list of those responding to this colormap.
     * @param obj IColoredView the object that wants to listen for colormap changes.
     * @return true if the will be notified of colormap changes; false otherwise.
     */
    bool addViewObject( std::shared_ptr<IColoredView> obj );
    virtual ~Colormap();
    const static QString CLASS_NAME;


private:

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

    Colormap( const QString& path, const QString& id );

    class Factory;

    //Views controlled by this colormap
    QList<std::shared_ptr<IColoredView> > m_coloredViews;

    //Supported color maps
    static std::shared_ptr<Colormaps> m_colors;


    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    StateInterface m_stateMouse;

	Colormap( const Colormap& other);
	Colormap operator=( const Colormap& other );
};
