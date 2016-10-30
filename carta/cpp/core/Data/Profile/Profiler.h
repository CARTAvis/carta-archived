/***
 * Manages Profiler settings.
 *
 */

#pragma once

#include <cmath>
#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "Data/Region/Region.h"
#include "CartaLib/IImage.h"
#include "CartaLib/Hooks/ProfileResult.h"
#include "CartaLib/Hooks/FitResult.h"

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
class LineStyles;
class LinkableImpl;
class Layer;
class ProfileFitService;
class ProfileRenderService;
class ProfileStatistics;

class Settings;

class UnitsIntensity;
class UnitsSpectral;
class UnitsFrequency;
class UnitsWavelength;


class Profiler : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    QString addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    QString removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    virtual QList<QString> getLinks() const Q_DECL_OVERRIDE;

    /**
     * Clear stored fit information.
     */
    void clearFits();

    /**
     * Returns the units on the x-axis of the profile.
     * @return - the profile x-axis units.
     */
    QString getAxisUnitsX() const;

    /**
        * Returns the units on the y-axis of the profile.
        * @return - the profile y-axis units.
        */
       QString getAxisUnitsY() const;

    /**
     * Return the number of Gaussians to fit to the curve.
     * @return - the number of Gaussians to fit to the curve.
     */
    int getGaussCount() const;

    /**
     * Get the number of manual guesses for Gaussian curve fits.
     * @return - the number of manual guesses for Gaussian curve fits.
     */
    int getGuessCount() const;

    /**
     * Returns the line style to use in drawing the fit curve.
     * @return - the fit curve line style.
     */
    QString getLineStyleFit() const;

    /**
     * Return the number of polynomial terms to fit to the curve.
     * @return - the degree of the polynomial to fit to the curve.
     */
    int getPolyCount() const;

    /**
     * Return the rest frequency used to compute the profile.
     * @param curveName - an identifier for the profile curve.
     * @return the rest frequency used to compute the profile.
     */
    double getRestFrequency( const QString& curveName ) const;

    /**
     * Return the rest units used in computing the profile.
     * @param curveName - an identifier for the profile curve.
     * @return - the rest units used in computing the profile.
     */
    QString getRestUnits( const QString& curveName ) const;

    /**
     * Return the name of the layer that has been selected to profile.
     * @return - the name of the layer to be profiled.
     */
    QString getSelectedLayer() const;

    /**
     * Return the name of the region that has been selected to profile.
     * @return - the name of the region to be profiled.
     */
    QString getSelectedRegion() const;

    /**
     * Return the current spectral type.
     * @return - the current spectral type.
     */
    QString getSpectralType() const;

    /**
     * Return the current spectral units.
     * @return - the current spectral units.
     */
    QString getSpectralUnits() const;


    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Return the aggregation statistic used to compute the profile.
     * @param curveName - an identifier for the curve.
     * @return - the aggregation statistic used to compute the profile.
     */
    QString getStatistic( const QString& curveName ) const;

    /**
     * Returns the upper limit of the zoom range on the x-axis.
     * @return - the upper limit of the zoom range on the x-axis.
     */
    double getZoomMax() const;

    /**
     * Returns the lower limit of the zoom range on the x-axis.
     * @return - the lower limit of the zoom range on the x-axis.
     */
    double getZoomMin() const;

    /**
     * Returns the percentile zoom for the lower bound of the x-axis.
     * @return - the percentile zoom for the lower bound of the x-axis.
     */
    double getZoomMinPercent() const;

    /**
     * Returns the percentilee zoom for the upper bound of the x-axis.
     * @return - the percentile zoom for the upper bound of the x-axis.
     */
    double getZoomMaxPercent() const;

    /**
     * Returns whether or not profiles will be automatically generated.
     * @return - true if profiles are automatically generated; false, otherwise.
     */
    bool isAutoGenerate() const;

    /**
     * Return true if initial guesses will be specified manually; false, otherwise.
     * @return - true if initial guesses will be specified manually, false, otherwise.
     */
    bool isFitManualGuess( ) const;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;

    /**
     * Return whether or not random heuristics will be used for the initial fit
     * guesses when performing a fit.
     * @return - whether random heuristics will be used for the initial fit guesses
     *      when performing a fit.
     */
    bool isRandomHeuristics() const;

    /**
     * Returns whether or not labels at the peaks of the Gaussians should be shown.
     * @return - true if labels at the peaks of the fit Gaussains should be shown; false, otherwise.
     */
    bool isShowPeakLabels() const;

    /**
     * Returns whether or not a separate residual plot should be shown.
     * @return - true if a separate residual plot should be shown; false, otherwise.
     */
    bool isShowResiduals() const;

    /**
     * Generate a new profile based on default settings.
     * @return - an error message if the new profile could not be generated; an empty
     *      string otherwise.
     */
    QString profileNew();

    /**
     * Delete the indicated profile.
     * @param name - an identifier for the profile to delete.
     * @return - an error message if the indicated profile could not be removed;
     *      an empty string otherwise.
     */
    QString profileRemove( const QString& name );

    /**
     * Force a state refresh.
     */
    virtual void refreshState() Q_DECL_OVERRIDE;

    /**
     * Reset the initial fit manual guesses.
     */
    void resetInitialFitGuesses();

    /**
     * Set the rest frequency back to its original value for the given curve.
     * @param curveName - an identifier for a profile curve.
     * @return - an error message if the rest frequency could not be reset; otherwise, an
     *      empty string.
     */
    QString resetRestFrequency( const QString& curveName );


    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Set whether or not profiles should be automatically generated.
     * @param autoGenerate - true if they should be automatically generated; false,
     * 		otherwise.
     */
    void setAutoGenerate( bool autoGenerate );

    /**
     * Set the bottom axis units.
     * @param unitStr - set the label to use for the x-axis of the plot.
     * @return - an error message if the units could not be set; otherwise, an
     *      empty string.
     */
    QString setAxisUnitsX( const QString& unitStr );

    /**
     * Set the y-axis units.
     * @param unitStr - set the label to use for the y-axis of the plot.
     * @return - an error message if the units could not be set; otherwise, an
     *      empty string.
     */
    QString setAxisUnitsY( const QString& unitStr );


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
     * Set the list of curves to be fit with Gaussians/Polynomials.
     * @param curveNames - the list of curves to be fit.
     * @return - an error message if there was a problem identifying
     *      the curves to be fit; an empty string otherwise.
     */
    QString setFitCurves( const QStringList curveNames );

    /**
     * Return a list of initial fit guesses for Gaussian parameters.
     * @return - the list of initial fit guesses for Gaussian parameters.
     */
    std::vector<std::tuple<double,double,double> > getFitGuesses();

    /**
     * Set manual fit guesses in image coordinates.
     * @param guesses- a list of fit guesses in image coordinates.
     * @return - an error message if the initial fit guesses could not be set.
     */
    QString setFitInitialGuesses(const std::vector<std::tuple<double,double,double> >& guesses );

    /**
     * Set manual fit guesses in image coordinates.
     * @param guesses- a list of fit guesses in image coordinates.
     * @return - an error message if the initial fit guesses could not be set.
     */
    QString setFitInitialGuessesPixels(const std::vector<std::tuple<int,int,int> >& guessPixels );

    /**
     * Set whether or not manual initial guesses will be specified for
     * fitting.
     * @param manualGuess - true if manual initial guesses will be used;
     *      false, otherwise.
     */
    void setFitManualGuess( bool manualGuess );

    /**
     * Set the number of Gaussians to fit to the curve.
     * @param count - the number of Gaussians to fit to the curve.
     */
    QString setGaussCount( int count );

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
     * Set the line style to use for the fit curve.
     * @param lineStyleFit - the line style to use for the fit curve.
     */
    QString setLineStyleFit( const QString& lineStyleFit );

    /**
     * Set the number of polynomial terms to use in fitting the curve.
     * @param count - the degree of the polynomial.
     */
    QString setPolyCount( int degree );

    /**
     * Set the plot style (continuous, step, etc).
     * @param name - an identifier for a profile curve.
     * @param plotStyle - an identifier for a plot style.
     * @return - an error string if the plot style could not be set; otherwise, an
     *      empty string.
     */
    QString setPlotStyle( const QString& name, const QString& plotStyle );

    /**
     * Set whether or not random heuristics will be used for the initial guesses
     * when performing a fit.
     * @param randomHeuristics - true if random heuristics should be used; false,
     *      otherwise.
     */
    void setRandomHeuristics( bool randomHeuristics );

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
     * Set the selected profile.
     * @param curveId - an identifier for the selected profile.
     * @return - an error message if the profile could not be selected; an empty string
     *      otherwise.
     */
    QString setSelectedCurve( const QString& curveId );

    /**
     * Set the layer to profile.
     * @param imageId - an identifier for the image to profile.
     * @return - an error message if the image could not be selected; an empty string
     *      otherwise.
     */
    QString setSelectedLayer( const QString& imageId );

    /**
     * Set the region to profile.
     * @param regionId - an identifier for the region to profile.
     * @return - an error message if the region could not be selected; an empty string
     *      otherwise.
     */
    QString setSelectedRegion( const QString& regionId );

    /**
     * Sets whether information about the point underneath the mouse cursor should
     * be shown or not.
     * @param showCursor - true if information about the point underneath the mouse
     *      cursor should be shown; false, otherwise.
     */
    void setShowCursor( bool showCursor );

    /**
     * Set whether or not to show manual fit guesses in the UI.
     * @param showFitGuesses - true if manual fit guesses should be displayed;
     *      false otherwise.
     * @return - an error message if the parameter could not be set; an empty
     *      string otherwise.
     */
    //Note:  Manual guesses will not be shown if manual mode has not
    //first been set.
    QString setShowFitGuesses( bool showFitGuesses );

    /**
     * Set whether or not residuals should be shown in the UI.
     * @param showFitResiduals - true if fit residuals should be displayed;
     *      false, otherwise.
     */
    void setShowFitResiduals( bool showFitResiduals );

    /**
     * Set whether fit statistics should be displayed.
     * @param showFitStatistics - true if fit statistics should be displayed;
     *      false otherwise.
     */
    void setShowFitStatistics( bool showFitStatistics );

    /**
     * Sets whether or not the position of the current frame should be shown.
     * @param showFrame - true if the position of the current frame should be shown;
     *      false otherwise.
     */
    void setShowFrame( bool showFrame );

    /**
     * Set whether or not the mean and RMS should be displayed.
     * @param showMeanRMS - true if the mean and RMS should be displayed;
     *      false, otherwise.
     */
    void setShowMeanRMS( bool showMeanRMS );

    /**
     * Set whether or not peak labels should be displayed indicating fit
     * information.
     * @param showPeakLabels - true to show labels at Gaussian fit peaks;
     *      false, otherwise.
     */
    void setShowPeakLabels( bool showPeakLabels );

    /**
     * Set the number of significant digits to use in storing numbers.
     * @param digits - the number of significant digits to use in storing numbers.
     * @return - an error message if the number of significant digits could not be set;
     *      otherwise, an empty string.
     */
    QString setSignificantDigits( int digits );

    /**
     * Set the method used to compute the profile.
     * @param statStr - an identifier for a method used to summarize a profile.
     * @param curveName - an identifier for a profile curve.
     * @return - an error method if the statistic could not be set; otherwise,
     *      an empty string.
     */
    QString setStatistic( const QString& statStr, const QString& curveName );

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
    void _fitFinished(const std::vector<Carta::Lib::Hooks::FitResult>& result);
    void _loadProfile( Controller* controller);
    void _movieFrame();
    void _plotSizeChanged();
    void _profileRendered(const Carta::Lib::Hooks::ProfileResult& result,
            std::shared_ptr<Layer> layer, std::shared_ptr<Region> region, bool createNew );
    void _removeUnsupportedCurves();
    void _resetFitGuessPixels();
    void _updateChannel( Controller* controller, Carta::Lib::AxisInfo::KnownType type );
    void _updateZoomRangeBasedOnPercent();
    QString _zoomToSelection();

private:
    const static QString AUTO_GENERATE;
    const static QString AXIS_UNITS_BOTTOM;
    const static QString AXIS_UNITS_LEFT;
    const static QString CURVES;
    const static QString CURVE_SELECT;
    const static QString FIT_STATISTICS;
    const static QString GAUSS_COUNT;
    const static QString GEN_MODE;
    const static QString GRID_LINES;
    const static QString HEURISTICS;
    const static QString IMAGE_SELECT;
    const static QString LEGEND_SHOW;
    const static QString LEGEND_LINE;
    const static QString LEGEND_LOCATION;
    const static QString LEGEND_EXTERNAL;
    const static QString MANUAL_GUESS;
    const static QString NO_REGION;
    const static QString PLOT_HEIGHT;
    const static QString PLOT_WIDTH;
    const static QString PLOT_LEFT;
    const static QString PLOT_TOP;
    const static QString POLY_DEGREE;
    const static QString REGION_SELECT;
    const static QString REST_UNITS;
    const static QString SHOW_FRAME;
    const static QString SHOW_GUESSES;
    const static QString SHOW_MEAN_RMS;
    const static QString SHOW_PEAK_LABELS;
    const static QString SHOW_RESIDUALS;
    const static QString SHOW_STATISTICS;
    const static QString SHOW_CURSOR;
    const static QString TAB_INDEX;
    const static QString ZOOM_BUFFER;
    const static QString ZOOM_BUFFER_SIZE;
    const static QString ZOOM_MIN;
    const static QString ZOOM_MAX;
    const static QString ZOOM_MIN_PERCENT;
    const static QString ZOOM_MAX_PERCENT;
    const static int ERROR_MARGIN;

    //Assign a color to the curve.
    void _assignColor( std::shared_ptr<CurveData> curveData );

    void _clearData();

    //Convert axis units.
    void _convertX( std::vector<double>& converted, std::shared_ptr<Layer> layer,
            const QString& oldUnit, const QString& newUnit ) const;
    void _convertDataX( std::vector<double>& converted, const QString& bottomUnit,
            std::shared_ptr<CurveData> curveData ) const;
    void _convertDataY( std::vector<double>& converted, const std::vector<double>& plotDataX,
            std::shared_ptr<CurveData> curveData, const QString& newUnits ) const;
    std::vector<double> _convertUnitsX( std::shared_ptr<CurveData> curveData,
            const QString& newUnit ) const;
    std::vector<double> _convertUnitsXFit( std::shared_ptr<CurveData> curveData,
            const QString& bottomUnit ) const;
    std::vector<double>  _convertUnitsXFitParams( std::shared_ptr<CurveData> curveData,
            const QString & bottomUnit ) const;
    std::vector<double> _convertUnitsY( std::shared_ptr<CurveData> curveData,
            const QString& newUnit ) const;
    std::vector<double> _convertUnitsYFit( std::shared_ptr<CurveData> curveData,
            const QString& newUnit ) const;
    std::vector<double>  _convertUnitsYFitParams( std::shared_ptr<CurveData> curveData,
            const QString & newUnit ) const;

    int _findCurveIndex( const QString& curveId ) const;

    bool _generateCurve( std::shared_ptr<Layer> layer, std::shared_ptr<Region> region );

    void _generateData( std::shared_ptr<Layer> layer, std::shared_ptr<Region> region,
            bool createNew = false);

    void _generateFit( );
    std::vector<std::tuple<double,double,double> > _generateFitGuesses( int count, bool random );

    Controller* _getControllerSelected() const;
    std::pair<double,double> _getCurveRangeX() const;
    std::vector<std::shared_ptr<Layer> > _getDataForGenerateMode( Controller* controller) const;
    int _getExtractionAxisIndex( std::shared_ptr<Carta::Lib::Image::ImageInterface> image ) const;

    QString _getFitStatusMessage( Carta::Lib::Fit1DInfo::StatusType statType) const;
    QString _getLegendLocationsId() const;

    /**
     * Returns the server side id of the Profiler user preferences.
     * @return the unique server side id of the user preferences.
     */
    QString _getPreferencesId() const;
    std::vector<std::shared_ptr<Region> > _getRegionForGenerateMode() const;

    void _initializeDefaultState();
    void _initializeCallbacks();
    void _initializeStatics();

    void _makeInitialGuesses( int count );

    void _saveCurveState();
    void _saveCurveState( int index );

    void _setErrorMargin();

    void _updateResidualData();
    void _updateFitStatistics();
    QString _updateFitStatistic( int index );

    void _updatePlotBounds();

    //Notify the plot to redraw.
    void _updatePlotData();

    void _updatePlotDisplay();

    bool _updateProfiles( Controller* controller );
    void _updateSelectedCurve();

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

    //For a movie.
    int m_oldFrame;
    int m_currentFrame;
    int m_timerId;

    //When two items with decimals are judged to be the same.
    double m_errorMargin;

    //State specific to the data that is loaded.
    Carta::State::StateInterface m_stateData;
    //Fit parameters
    Carta::State::StateInterface m_stateFit;
    //Fit statistics
    Carta::State::StateInterface m_stateFitStatistics;


    static UnitsSpectral* m_spectralUnits;
    static UnitsIntensity* m_intensityUnits;
    static UnitsFrequency* m_frequencyUnits;
    static UnitsWavelength* m_wavelengthUnits;
    static ProfileStatistics* m_stats;
    static GenerateModes* m_generateModes;
    static LineStyles* m_lineStyles;

    static QList<QColor> m_curveColors;

    //Compute the profile in a thread
    std::unique_ptr<ProfileRenderService> m_renderService;

    //Out source the job of fitting the curve.
    std::unique_ptr<ProfileFitService> m_fitService;



    int m_residualPlotIndex;

	Profiler( const Profiler& other);
	Profiler& operator=( const Profiler& other );
};
}
}
