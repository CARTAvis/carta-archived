/***
 * Manages colormap settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

class ImageView;
class Colormaps;


class Colormap : public QObject, public CartaObject {

    Q_OBJECT

public:

    virtual ~Colormap();
    const static QString CLASS_NAME;

signals:
    void colorMapChanged( int index );

private:

    QString _commandSetColorMap( const QString& params );
    QString _commandInvertColorMap( const QString& params );
    QString _commandReverseColorMap( const QString& params );
    QString _commandSetColorMix( const QString& params );
    bool _processColorStr( const QString key, const QString colorStr, bool* valid );

    void _initializeDefaultState();
    void _initializeCallbacks();

    static bool m_registered;
    const static QString COLOR_MAP_INDEX;
    const static QString REVERSE;
    const static QString INVERT;
    const static QString RED_PERCENT;
    const static QString GREEN_PERCENT;
    const static QString BLUE_PERCENT;
    const static QString COLOR_MIX;
    const static QString COLOR_MIX_RED;
    const static QString COLOR_MIX_GREEN;
    const static QString COLOR_MIX_BLUE;

    Colormap( const QString& path, const QString& id );

    class Factory;

    //Data View
    std::shared_ptr<ImageView> m_view;

    //Supported color maps
    static std::shared_ptr<Colormaps> m_colors;


    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    StateInterface m_stateMouse;

	Colormap( const Colormap& other);
	Colormap operator=( const Colormap& other );
};
