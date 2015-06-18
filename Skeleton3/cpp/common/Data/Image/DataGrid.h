/***
 * Manages grid control settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

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

class DataGrid : public Carta::State::CartaObject {

friend class DataSource;
friend class GridControls;

public:

    virtual ~DataGrid();
    const static QString CLASS_NAME;

private:
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> _getRenderer();
    bool _resetState( const Carta::State::StateInterface& otherState );
    QStringList _setAxesColor( int redAmount, int greenAmount, int blueAmount, bool* axesColorChanged );
    QString _setCoordinateSystem( const QString& coordSystem, bool* coordChanged );
    QString _setFontFamily( const QString& fontFamily, bool* familyChanged );
    QString _setFontSize( int fontSize, bool* sizeChanged );
    QStringList _setGridColor( int redAmount, int greenAmount, int blueAmount, bool* gridColorChanged );
    QString _setGridSpacing( double spacing, bool* spacingChanged );
    QString _setGridTransparency( double transparency, bool* transparencyChanged );
    QString _setGridThickness( double thickness, bool* coordChanged );
    QStringList _setLabelColor( int redAmount, int greenAmount, int blueAmount, bool* labelColorChanged );
    QString _setShowAxis( bool showAxis, bool* gridChanged );
    QString _setShowGridLines( bool showLines, bool* gridChanged );
    QString _setShowInternalLabels( bool showInternalLabels, bool * gridChanged );


    Carta::State::StateInterface _getState();
    QPen _getPen( const QString& key, const Carta::State::StateInterface& state );
    void _initializeDefaultPen( const QString& key, int red, int green, int blue,
            int alpha, double width );
    void _initializeDefaultState();
    void _initCoordSystems();
    void _initializeGridRenderer();
    void _initializeSingletons();
    void _resetGridRenderer();
    QStringList _setColor( const QString& key, int redAmount, int greenAmount, int blueAmount,
            bool* colorChanged );
    const static QString ALPHA;
    const static QString AXES;
    const static QString BLUE;
    const static QString COORD_SYSTEM;
    const static QString DIRECTION;
    const static QString FONT;
    const static QString GREEN;
    const static QString LABEL_COLOR;
    const static QString PEN_WIDTH;
    const static QString RED;
    const static QString SHOW_AXIS;
    const static QString SHOW_INTERNAL_LABELS;
    const static QString SHOW_GRID_LINES;
    const static QString SPACING;
    const static QString GRID;
    const static QString TRANSPARENCY;
    const static int PEN_FACTOR;

    static bool m_registered;

    DataGrid( const QString& path, const QString& id );

    class Factory;

    /// wcs grid render service
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> m_wcsGridRenderer;

    static CoordinateSystems* m_coordSystems;
    static Fonts* m_fonts;
    double m_errorMargin;

	DataGrid( const DataGrid& other);
	DataGrid& operator=( const DataGrid& other );
};
}
}
