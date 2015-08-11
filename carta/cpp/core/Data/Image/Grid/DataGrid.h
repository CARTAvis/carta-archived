/***
 * Manages grid control settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/Hooks/GetWcsGridRenderer.h"

namespace Carta {
namespace Lib {
    class IWcsGridRenderService;
    namespace PixelPipeline {
        class IColormapNamed;
    }
}
}

namespace Carta {

namespace Data {

class CoordinateSystems;
class Fonts;
class Themes;

class DataGrid : public Carta::State::CartaObject {

friend class ControllerData;
friend class GridControls;

public:

    virtual ~DataGrid();

    const static QString CLASS_NAME;
    const static QString GRID;
private:
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> _getRenderer();
    /**
     * Returns the currently selected coordinate system.
     * @return the coordinate system that is selected.
     */
    Carta::Lib::KnownSkyCS _getSkyCS() const;
    bool _isGridVisible() const;
    bool _resetState( const Carta::State::StateInterface& otherState );
    QStringList _setAxesColor( int redAmount, int greenAmount, int blueAmount, bool* axesColorChanged );
    QString _setAxesThickness( int thickness, bool* thicknessChanged );
    QString _setAxesTransparency( int transparency, bool* transparencyChanged );
    QString _setCoordinateSystem( const QString& coordSystem, bool* coordChanged );
    QString _setFontFamily( const QString& fontFamily, bool* familyChanged );
    QString _setFontSize( int fontSize, bool* sizeChanged );
    QStringList _setGridColor( int redAmount, int greenAmount, int blueAmount, bool* gridColorChanged );
    QString _setGridSpacing( double spacing, bool* spacingChanged );
    QString _setGridTransparency( int transparency, bool* transparencyChanged );
    QString _setGridThickness( int thickness, bool* coordChanged );
    QStringList _setLabelColor( int redAmount, int greenAmount, int blueAmount, bool* labelColorChanged );
    QString _setShowAxis( bool showAxis, bool* gridChanged );
    QString _setShowCoordinateSystem( bool showCoordinateSystem, bool* coordChanged );
    QString _setShowGridLines( bool showLines, bool* gridChanged );
    QString _setShowInternalLabels( bool showInternalLabels, bool * gridChanged );
    QString _setShowStatistics( bool showStatistics, bool * statisticsChanged );
    QString _setShowTicks( bool showTicks, bool* ticksChanged );
    QString _setTickThickness( int tickThickness, bool* thicknessChanged );
    QStringList _setTickColor( int redAmount, int greenAmount, int blueAmount, bool* colorChanged );
    QString _setTickTransparency( int transparency, bool* transparencyChanged );
    QString _setTheme( const QString& theme, bool* themeChanged );


    Carta::State::StateInterface _getState();
    QPen _getPen( const QString& key, const Carta::State::StateInterface& state );
    void _initializeDefaultPen( const QString& key, int red, int green, int blue,
            int alpha, int width );
    void _initializeDefaultState();
    void _initCoordSystems();
    void _initializeGridRenderer();
    void _initializeSingletons();
    void _resetGridRenderer();
    QStringList _setColor( const QString& key, int redAmount, int greenAmount, int blueAmount,
            bool* colorChanged );

    const static QString AXES;
    const static QString COORD_SYSTEM;
    const static QString DIRECTION;
    const static QString FONT;
    const static QString LABEL_COLOR;
    const static QString SHOW_AXIS;
    const static QString SHOW_COORDS;
    const static QString SHOW_INTERNAL_LABELS;
    const static QString SHOW_GRID_LINES;
    const static QString SHOW_STATISTICS;
    const static QString SHOW_TICKS;
    const static QString SPACING;
    const static QString THEME;
    const static QString TICK;
    const static int PEN_FACTOR;

    static bool m_registered;

    DataGrid( const QString& path, const QString& id );

    class Factory;

    /// wcs grid render service
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> m_wcsGridRenderer;

    static CoordinateSystems* m_coordSystems;
    static Fonts* m_fonts;
    static Themes* m_themes;
    double m_errorMargin;

	DataGrid( const DataGrid& other);
	DataGrid& operator=( const DataGrid& other );
};
}
}
