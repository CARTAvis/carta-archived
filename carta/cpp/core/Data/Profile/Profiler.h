/***
 * Manages Profiler settings.
 *
 */

#pragma once

#include <cmath>
#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "CartaLib/IImage.h"
#include "ProfileExtractor.h"

#include <QObject>

namespace Carta {
namespace Lib {

namespace Image {
class ImageInterface;
}
}
}


class ImageView;


namespace Carta {
namespace Plot2D {
class Plot2DGenerator;
}

namespace Data {

class Plot2DManager;
class Controller;
class CurveData;
class GenerateModes;
class LegendLocations;
class LinkableImpl;
class Layer;
class Settings;

class UnitsIntensity;
class UnitsSpectral;


class Profiler : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    QString addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    QString removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    virtual QList<QString> getLinks() const Q_DECL_OVERRIDE;

    /**
     * Returns the units on the bottom axis of the profile.
     * @return - the profile bottom axis units.
     */
    QString getAxisUnitsBottom() const;

    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;

    /**
     * Generate a new profile based on default settings.
     * @return - an error message if the new profile could not be generated; an empty
     *      string otherwise.
     */
    QString profileNew();

    /**
     * Generate a new profile based on the given profile.
     * @param baseName - an identifier for the profile to copy.
     * @return - an error message if a copy of the given profile could not be
     *      generated; an empty string otherwise.
     */
    QString profileCopy( const QString& baseName );

    /**
     * Delete the indicated profile.
     * @param name - an identifier for the profile to delete.
     * @return - an error message if the indicated profile could not be removed;
     *      an empty string otherwise.
     */
    QString profileRemove( const QString& name );

    /**
     * Set the rest frequency back to its original value for the given curve.
     * @param curveName - an identifier for a profile curve.
     * @return - an error message if the rest frequency could not be reset; otherwise, an
     *      empty string.
     */
    QString resetRestFrequency( const QString& curveName );


    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Set the bottom axis units.
     * @param unitStr - set the label to use for the bottom axis of the plot.
     * @return - an error message if the units could not be set; otherwise, an
     *      empty string.
     */
    QString setAxisUnitsBottom( const QString& unitStr );

    /**
     * Set the left axis units.
     * @param unitStr - set the label to use for the left axis of the plot.
     * @return - an error message if the units could not be set; otherwise, an
     *      empty string.
     */
    QString setAxisUnitsLeft( const QString& unitStr );


    /**
     * Set the color of a particular data set on the plot.
     * @param name - an identifier for a data set.
     * @param redAmount - the amount of red in the color [0,255].
     * @param greenAmount - the amount of green in the color [0,255].
     * @param blueAmount - the amount of blue in the color [0,255].
     * @return - one or more error messages if the color of the data set cannot be set.
     */
    QStringList setCurveColor( const QString& name, int redAmount, int greenAmount, int blueAmount );

    /**
     * Change the name of a profile curve.
     * @param id - the old name of the curve.
     * @param newName - the new name of the curve.
     */
    QString setCurveName( const QString& id, const QString& newName );

    /**
     * Set which if any profiles should be automatically generated.
     * @param modeStr - an identifier for a profile generate mode.
     * @return - an error message if the profile generate mode was not recognized
     *      or could not be set.
     */
    QString setGenerateMode( const QString& modeStr );

    /**
     * Set whether or not to show/hide grid lines on the plot.
     * @param showLines - true to show grid lines; false otherwise.
     */
    void setGridLines( bool showLines );

    /**
     * Set the drawing style for the Profiler (outline, filled, etc).
     * @param style a unique identifier for a Profiler drawing style.
     * @return an error message if there was a problem setting the draw style; an empty string otherwise.
     */
    QString setLineStyle( const QString& name, const QString& lineStyle );

    /**
     * Set whether the legend should be internal or external to the plot.
     * @param externalLegend - true for an external legend; false for an internal legend.
     */
    void setLegendExternal( bool externalLegend );

    /**
     * Set whether or not to show a sample line next to legend items.
     * @param showLegendLine - true if a legend line should be shown next to legend
     *      items; false otherwise.
     */
    void setLegendLine( bool showLegendLine );

    /**
     * Set the location of the legend on the plot (right, bottom, etc).
     * @param locationStr - an identifier for the location of a legend on the plot.
     * @return - an error message if the legend location could not be set; an empty string otherwise.
     */
    //Note:  Different locations are available based on whether the legend is external/internal
    //to the plot so make sure the that flag is set correctly before trying to set a location.
    QString setLegendLocation( const QString& locationStr );

    /**
     * Set whether or not to show a legend on the plot.
     * @param showLegend - true to show a legend on the plot; false otherwise.
     */
    void setLegendShow( bool showLegend );

    /**
     * Set the plot style (continuous, step, etc).
     * @param name - an identifier for a profile curve.
     * @param plotStyle - an identifier for a plot style.
     * @return - an error string if the plot style could not be set; otherwise, an
     *      empty string.
     */
    QString setPlotStyle( const QString& name, const QString& plotStyle );

    /**
     * Set the rest frequency used to generate a profile for the given curve.
     * @param freq - the rest frequency.
     * @param curveName - an identifier for a profile curve.
     * @return - an error message if the rest frequency could not be set; otherwise,
     *      an empty string.
     */
    QString setRestFrequency( double freq, const QString& curveName );

    /**
     * Set the rest frequency units used to generate a profile for the given curve.
     * @param restUnits - the rest frequency units.
     * @param curveName - an identifier for a profile curve.
     * @return - an error message if the rest frequency units could not be set; otherwise,
     *      an empty string.
     */
    QString setRestUnits( const QString& restUnits, const QString& curveName );

    /**
     * Set whether or not rest frequency units are given in frequency or wavelength.
     * @param restUnitsFreq - true if rest frequency units are specified as frequency;
     *      false otherwise.
     * @param curveName - an identifier for a profile curve.
     * @return - an error message if the type of rest frequency units could not be set;
     *      otherwise, an empty string.
     */
    QString setRestUnitType( bool restUnitsFreq, const QString& curveName );

    /**
     * Set the index of the profile settings tab that should be selected.
     * @param index - the index of the profile settings tab that should be selected.
     * @return - an error message if the tab index could not be set; an empty string otherwise.
     */
    QString setTabIndex( int index );

    /**
     * Set whether or not to add extra space at each end of the x-axis.
     * @param zoomBuffer - true to add extra padding at each end of the x-axis; false otherwise.
     */
    void setZoomBuffer( bool zoomBuffer );

    /**
     * Set the amount of extra space to add to each end of the x-axis.
     * @param zoomBufferSize - a percentage of the actual plot space to add to each end of the x-axis.
     * @return - an error message if the buffer percentage could not be set.
     */
    QString setZoomBufferSize( double zoomBufferSize );

    /**
     * Set the zoom range in world coordinates.
     * @param zoomMin - the lower boundary of the zoom window.
     * @param zoomMax - the upper boundary of the zoom window.
     * @return - an error message if the zoom range could not be set; otherwise, an empty string.
     */
    QString setZoomRange( double zoomMin, double zoomMax );

    /**
     * Set the zoom range as a percentage of the plot range.
     * @param zoomMinPercent - a value in [0,100] indicating the lower boundary of the zoom window.
     * @param zoomMaxPercent - a value in [0,100] indicating the upper boundary of the zoom window.
     * @return - an error message if the zoom range could not be set as a percentage; otherwise,
     *      an empty string.
     */
    QString setZoomRangePercent( double zoomMinPercent, double zoomMaxPercent );


    virtual ~Profiler();
    const static QString CLASS_NAME;

protected:
    //Callback for moving the frame.
    virtual void timerEvent( QTimerEvent* event );

private slots:
    void _cursorUpdate( double x, double y );
    void _loadProfile( Controller* controller);
    void _movieFrame();
    void _updateChannel( Controller* controller, Carta::Lib::AxisInfo::KnownType type );
    QString _zoomToSelection();

private:
    const static QString AXIS_UNITS_BOTTOM;
    const static QString AXIS_UNITS_LEFT;
    const static QString CURVES;
    const static QString CURVE_SELECT;
    const static QString GEN_MODE;
    const static QString GRID_LINES;
    const static QString IMAGES;
    const static QString LEGEND_SHOW;
    const static QString LEGEND_LINE;
    const static QString LEGEND_LOCATION;
    const static QString LEGEND_EXTERNAL;
    const static QString REGIONS;
    const static QString SHOW_TOOLTIP;
    const static QString TOOL_TIPS;
    const static QString TAB_INDEX;
    const static QString ZOOM_BUFFER;
    const static QString ZOOM_BUFFER_SIZE;
    const static QString ZOOM_MIN;
    const static QString ZOOM_MAX;
    const static QString ZOOM_MIN_PERCENT;
    const static QString ZOOM_MAX_PERCENT;
    const static double ERROR_MARGIN;

    //Assign a color to the curve.
    void _assignColor( std::shared_ptr<CurveData> curveData );
    void _assignCurveName( std::shared_ptr<CurveData>& profileCurve ) const;

    void _clearData();

    //Convert axis units.
    void _convertX( std::vector<double>& converted,
            std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
            const QString& oldUnit, const QString& newUnit ) const;
    std::vector<double> _convertUnitsX( std::shared_ptr<CurveData> curveData,
            const QString& newUnit = QString() ) const;
    std::vector<double> _convertUnitsY( std::shared_ptr<CurveData> curveData ) const;

    void _generateData( std::shared_ptr<Layer> layer, bool createNew = false );
    void _generateData( std::shared_ptr<Carta::Lib::Image::ImageInterface> image,
             int curveIndex, const QString& layerName, bool createNew = false );

    Controller* _getControllerSelected() const;
    std::vector<std::shared_ptr<Layer> > _getDataForGenerateMode( Controller* controller) const;
    int _getExtractionAxisIndex( std::shared_ptr<Carta::Lib::Image::ImageInterface> image ) const;
    double _getMaxFrame() const;
    QString _getLegendLocationsId() const;
    /**
     * Returns the server side id of the Profiler user preferences.
     * @return the unique server side id of the user preferences.
     */
    QString _getPreferencesId() const;

    int _findCurveIndex( const QString& curveId ) const;

    void _initializeDefaultState();
    void _initializeCallbacks();
    void _initializeStatics();

    void _saveCurveState();
    void _saveCurveState( int index );

    void _updateAvailableImages( Controller* controller );

    void _updatePlotBounds();

    //Notify the plot to redraw.
    void _updatePlotData();

    //Breaks a string of the form "Frequency (GHz)" into a type "Frequency"
    //and units "GHz".
    static QString _getUnitType( const QString& unitStr );
    static QString _getUnitUnits( const QString& unitStr );
    static bool m_registered;

    //For right now we are supporting only one linked controller.
    bool m_controllerLinked;

    Profiler( const QString& path, const QString& id );
    class Factory;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Preferences
    std::unique_ptr<Settings> m_preferences;

    //Directs how the plot should be drawn and manages
    //updates for the plot.
    std::unique_ptr<Plot2DManager> m_plotManager;

    //Location of the legends on the plot
    std::unique_ptr<LegendLocations> m_legendLocations;

    //Plot data
    QList< std::shared_ptr<CurveData> > m_plotCurves;

    QString m_leftUnit;
    QString m_bottomUnit;

    //For a movie.
    int m_oldFrame;
    int m_currentFrame;
    int m_timerId;

    //State specific to the data that is loaded.
    Carta::State::StateInterface m_stateData;

    static UnitsSpectral* m_spectralUnits;
    static UnitsIntensity* m_intensityUnits;

    static GenerateModes* m_generateModes;


    static QList<QColor> m_curveColors;

	Profiler( const Profiler& other);
	Profiler operator=( const Profiler& other );
};
}
}
