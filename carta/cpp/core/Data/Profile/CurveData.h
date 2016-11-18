/***
 * A set of data that comprises a curve.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/IImage.h"
#include "CartaLib/ProfileInfo.h"
#include <QColor>
#include <QObject>


namespace Carta {
namespace Data {

class InitialGuess;
class Layer;
class LineStyles;
class ProfileStatistics;
class ProfilePlotStyles;
class Region;
class UnitsFrequency;
class UnitsWavelength;

class CurveData : public Carta::State::CartaObject {
friend class Profiler;
public:

    /**
     * Clear fit information stored in the curve.
     */
    void clearFit();

    /**
     * Return the color to use in plotting the points of the curve.
     * @return - the color to use in plotting the points of the curve.
     */
    QColor getColor() const;


    /**
     * Return information about the plot point under the cursor.
     * @param x - the x-coordinate of the point under the cursor.
     * @param y - the y-coordinate of the point under the cursor.
     * @param error - the approximation error in matching the mouse point
     *      to the closest point on the plot curve.
     * @return - information about the curve point closest to the mouse point.
     */
    QString getCursorText( double x, double y, double* error) const;

    /**
     * Return the number of data points in the curve.
     * @return - the number of data points in the curve.
     */
    int getDataCount() const;

    /**
     * Return a default name to be used for the curve if the user hasn't specified
     * a custom name.
     * @return - a default descriptive name for the curve.
     */
    QString getDefaultName() const;

    /**
     * Return the two-dimensional data that represent a fit to this curve.
     * @return - the two-dimensional data that represent a curve fit.
     */
    std::vector< std::pair<double,double> > getFitData() const;

    /**
     * Get the initial guess for the fit center.
     * @param index - the index of the guess in case of multiple fits.
     * @return - the initial guess for the fit center.
     */
    double getFitParamCenter( int index ) const;

    /**
     * Get the initial guess for the fit peak.
     * @param index - the index of the guess in case of multiple fits.
     * @return - the initial guess for the fit peak.
     */
    double getFitParamPeak( int index ) const;

    /**
     * Get the initial guess for the fit fbhw.
     * @param index - the index of the guess in case of multiple fits.
     * @return - the initial guess for the fit fbhw.
     */
    double getFitParamFBHW( int index ) const;

    /**
     * Return a list of (center,peak,fbhw) information specifying the Gaussian
     * fits.
     * @return - a list of (center,peak,fbhw) Gaussian fit parameters.
     */
    std::vector<std::tuple<double,double,double> > getFitParams() const;

    /**
     * Return the coefficients of the polynomial that was fit to the curve.
     * @return - the coefficients of the polynomial that was fit to the curve.
     */
    std::vector<double> getFitPolyCoeffs() const;

    /**
     * Return a list of fit residuals.
     * @return - a list of residual data form a fit.
     */
    std::vector<std::pair<double,double> > getFitResiduals() const;

    /**
     * Return the RMS of the fit.
     * @return - the RMS of the fit.
     */
    double getFitRMS() const;

    /**
     * Return stored parameters for fitting this curve.
     * @return - a string representation of fitting parameters.
     */
     QString getFitState() const;

     /**
      * Return the status of the fit.
      * @return - the fit status (whether or not it was completed).
      */
     QString getFitStatus() const;

    /**
     * Return a list of parameters that specify the Gaussians that were fit to the
     * curve.
     * @return - a list containing <center,peak,fbhw> information for the Gaussians
     *      that were fit to the curve.
     */
    std::vector<std::tuple<double,double,double> > getGaussParams() const;

    /**
     * Return the image used to generate the curve.
     * @return - the image used to generate the curve.
     */
    std::shared_ptr<Layer> getLayer() const;

    /**
     * Return an identifier for the style to use in drawing lines.
     * @return - an identifier for the style used to draw lines.
     */
    QString getLineStyle() const;

    /**
     * Return the line style to use in drawing the fit curve.
     * @return - a description of the line style to use in drawing the fit curve.
     */
    QString getLineStyleFit() const;

    /**
     * Return the minimum and maximum x- and y-values of points on
     * the curve.
     * @param xmin - pointer to the curve minimum x-value.
     * @param xmax - pointer to the curve maximum x-value.
     * @param ymin - pointer to the curve minimum y-value.
     * @param ymax - pointer to the curve maximum y-value.
     */
    void getMinMax(double* xmin, double* xmax, double* ymin,
               double* ymax ) const;

    /**
     * Return the curve data.
     * @return - the (x,y) pairs that make up the plot curve.
     */
    std::vector< std::pair<double, double> > getPlotData() const;

    /**
     * Return information for calculating a profile.
     * @return - information for calculating a profile.
     */
    Carta::Lib::ProfileInfo getProfileInfo() const;


    /**
     * Return an identifier for the curve.
     * @return - a curve identifier.
     */
    QString getName() const;

    /**
     * Get a list of positions and descriptions for the fit Gaussian peaks.
     * @param xUnit - the units of the x-axis.
     * @param yUnit - the units of the y-axis.
     * @return - a list of positions and descripts for the fit Gaussian peaks.
     */
    std::vector< std::tuple<double,double,QString> > getPeakLabels( const QString& xUnit, const QString& yUnit ) const;

    /**
     * Return the region over which the curve is defined.
     * @return - the region over which the curve is defined.
     */
    std::shared_ptr<Region> getRegion() const;

    /**
     * Return the rest frequency used for the profile.
     * @return - the rest frequency used for the profile.
     */
    double getRestFrequency() const;

    /**
     * Return the units of rest frequency.
     * @return - the rest frequency units.
     */
    QString getRestUnits() const;

    /**
     * Return the spectral type used to compute the profile.
     * @return - the spectral type used to compute the profile.
     */
    QString getSpectralType() const;

    /**
     * Return the units of the spectral axis.
     * @return - the spectral axis units.
     */
    QString getSpectralUnit() const;

    /**
     * Return the internal state of the curve as a string.
     * @return - the curve state.
     */
    QString getStateString() const;

    /**
     * Return the statistic used to summarize profiles.
     * @return - the statistic used to summarize profiles.
     */
    QString getStatistic() const;

    /**
     * Get the curve x-coordinates.
     * @return - the curve x-coordinate values.
     */
    std::vector<double> getValuesX() const;

    /**
     * Get the fit curve x-coordinates.
     * @return - the fit curve x-coordinate values.
     */
    std::vector<double> getValuesXFit() const;

    /**
     * Get the curve y-coordinates.
     * @return - the curve y-coordinate values.
     */
    std::vector<double> getValuesY() const;

    /**
     * Get the fit curve y-coordinates.
     * @return - the fit curve y-coordinate values.
     */
    std::vector<double> getValuesYFit() const;

    /**
     * Returns whether or not this profile is being actively used (visible) or not.
     * @return - if the profile is being actively used; false, otherwise.
     */
    bool isActive() const;

    /**
     * Returns true if the identifiers passed in matches those in this curve;
     * false otherwise.
     * @param layer - a layer containing an image
     * @param region - a region in an image.
     * @param otherProfInfo - information for producing a profile.
     * @return - true if the information passed in match those in this profile; false,
     * 		otherwise.
     */
    bool isMatch( std::shared_ptr<Layer> layer, std::shared_ptr<Region> region,
    		Carta::Lib::ProfileInfo otherProfInfo ) const;

    /**
     * Return whether or not the curve has been fit.
     * @return - true if the curve has been fit with one or more Gaussian/polynomials, etc;
     *      false otherwise.
     */
    bool isFitted() const;

    /**
     * Returns whether or not the profiler is currently in frequency units.
     * @return - true for frequency units; false otherwise.
     */
    bool isFrequencyUnits() const;

    /**
     * Returns whether or not the curve has been selected for fitting.
     * @return - true if the curve is selected for fitting; false, otherwise.
     */
    bool isSelectedFit() const;

    /**
     * Set the rest frequency back to its original value.
     */
    void resetRestFrequency();

    /**
     * Set whether this profile is active (visible) or just cached for future reference.
     * @param active - true if the profile is being actively used; false, otherwise.
     * @return - true if the active status of the profile has changed; false, otherwise.
     */
    bool setActive( bool active );

    /**
     * Set the color to use in plotting the points of the curve.
     * @param color - the color to use in plotting curve points.
     */
    void setColor( QColor color );

    /**
     * Set the x- and y- data values that comprise the curve.
     * @param valsX - the x-coordinate values of the curve.
     * @param valsY - the y-coordinate values of the curve.
     */
    void setData( const std::vector<double>& valsX, const std::vector<double>& valsY  );

    /**
     * Set the x-values that comprise the curve.
     * @param valsX - the x-coordinate values of the curve.
     */
    void setDataX( const std::vector<double>& valsX );

    /**
     * Set the x-values that comprise the fit curve.
     * @param valsX - the x-coordinate values of the fit curve.
     */
    void setDataXFit( const std::vector<double>& valsX );

    /**
     * Set the y- data values that comprise the curve.
     * @param valsY - the y-coordinate values of the curve.
     */
    void setDataY( const std::vector<double>& valsY );

    /**
     * Set the y- data values that comprise the fit curve.
     * @param valsY - the y-coordinate values of the fit curve.
     */
    void setDataYFit( const std::vector<double>& valsY );

    /**
     * Set the RMS of the fit.
     * @param rms - the RMS of the fit.
     */
    void setFitRMS( double rms );

    /**
     * Set the status of the fit (whether or not it succeeded).
     * @param fitStatus - the status of the fit.
     */
    void setFitStatus( const QString& fitStatus );

    /**
     * Set a list of parameters that specify the Gaussians that were fit to the curve.
     * @param params - a list of <center,peak,fbhw> information that specifies the Gaussians
     *      that were fit to the curve.
     */
    void setGaussParams( const std::vector<std::tuple<double,double,double> >& params );

    /**
     * Set the x- and y- fit values for the curve.
     * @param valsX - the x-coordinate fit values of the curve.
     * @param valsY - the y-coordinate fit values of the curve.
     */
    void setFit( const std::vector<double>& valsX, const std::vector<double>& valsY  );

    /**
     * Set parameters for fitting one or more Gaussians to this curve.
     * @param fitParams - parameters for fitting one or more Gaussians to this curve.
     */
    void setFitParams( const QString& fitParams );

    /**
     * Set the coefficients of any polynomial that was fit to the curve.
     * @param polyCoeffs - the coefficients of any polynomial that was fit to the curve.
     */
    void setFitPolyCoeffs( const std::vector<double>& polyCoeffs );

    /**
     * Set the line style (outline,solid, etc) for drawing the curve.
     * @param lineStyle - the style to be used for connecting the curve points.
     */
    QString setLineStyle( const QString& lineStyle );

    /**
     * Set the line style (outline,solid, etc) for drawing the curve.
     * @param lineStyle - the style to be used for connecting the curve points.
     */
    QString setLineStyleFit( const QString& lineStyleFit );


    /**
     * Set an identifier for the curve.
     * @param curveName - an identifier for the curve.
     * @return - an error message if the profile curve name could not be set.
     */
    QString setName( const QString& curveName );

    /**
     * Set the line style to use in plotting the profile curve.
     * @param plotStyle - the line style to be used in plotting the profile
     *      curve.
     * @return - an error message if the plotting style could not be set; otherwise,
     *      an empty string.
     */
    QString setPlotStyle( const QString& plotStyle );

    /**
     * Set the region over which the curve is defined.
     * @param region - the region over which the curve is defined.
     */
    void setRegion( std::shared_ptr<Region> region );

    /**
     * Set the rest frequency to be used in calculating the profile.
     * @param freq - the rest frequency to use in calculating the profile.
     * @param errorMargin - how far the new frequncy needs to be from the old one in
     *      order to be recognized as different.
     * @param valueChanged - set to true if the stored frequency value was changed.
     * @return - an error message if the rest frequency could not be set; otherwise,
     *      an empty string.
     */
    QString setRestFrequency( double freq, double errorMargin, bool* valueChanged );

    /**
     * Set the rest frequency and units that were used in calculating the profile.
     * @param restFrequency - the rest frequency used in the calculation.
     * @param restUnit - the units of rest frequency.
     */
    void setRestQuantity( double restFrequency, const QString& restUnit );

    /**
     * Set the units of rest frequency.
     * @param restUnits - the rest frequency units.
     * @param significantDigits - the number of significant digits to store.
     * @param errorMargin - required difference before deciding a value is significantly different.
     * @return - true if the rest units were changed; false, otherwise.
     */
    bool setRestUnits( const QString& restUnits, int significantDigits, double errorMargin);

    /**
     * Set whether the rest units are frequency or wavelength.
     * @param restUnitsFreq - true if rest frequency is specified as frequency; false,
     *      if it is specified as wavelength.
     * @param significantDigits - the number of significant digits to store.
     * @param errorMargin - required difference before deciding a value is significantly different.
     * @return - whether or not the rest unit type was changed.
     */
    bool setRestUnitType( bool restUnitsFreq, int significantDigits, double errorMargin );


    /**
     * Sets whether or not the curve should be fit.
     * @param selected - true if the curve should be fit; false, otherwise.
     */
    void setSelectedFit( bool selected );

    /**
     * Store the spectral information used to compute the profile.
     * @param spectralType - the spectral type.
     * @param spectralUnit - the spectral units.
     */
    void setSpectralInfo( const QString& spectralType, const QString& spectralUnit );

    /**
     * Set the method used to summarize profile points.
     * @param stat - the method used to summarize profile points.
     * @return - an error message if the method was unrecognized; an empty string otherwise.
     */
    QString setStatistic( const QString& stat );

    /**
     * Set the layer that was used to generate the curve.
     * @param layer - the layer that was used to generate the curve.
     */
    void setLayer( std::shared_ptr<Layer> layer );

    virtual ~CurveData();
    const static QString CLASS_NAME;

private:
    const static QString ACTIVE;
    const static QString FIT;
    const static QString FIT_CENTER;
    const static QString FIT_PEAK;
    const static QString FIT_FBHW;
    const static QString FIT_CENTER_PIXEL;
    const static QString FIT_PEAK_PIXEL;
    const static QString FIT_FBHW_PIXEL;
    const static QString FIT_SELECT;
    const static QString INITIAL_GUESSES;
    const static QString POINT_SOURCE;
    const static QString PLOT_STYLE;

    const static QString STATISTIC;
    const static QString STYLE_FIT;
    const static QString REST_FREQUENCY;
    const static QString REST_FREQUENCY_UNITS;
    const static QString REST_UNIT_FREQ;
    const static QString REST_UNIT_WAVE;
    const static QString SPECTRAL_TYPE;
    const static QString SPECTRAL_UNIT;

    double _calculateRelativeError( double minValue, double maxValue ) const;
    void _calculateRelativeErrors( double& errorX, double& errorY ) const;
    void _convertRestFrequency( const QString& oldUnits, const QString& newUnits,
            int significantDigits, double errorMargin );
    static QString _generateName( std::shared_ptr<Layer> layer, std::shared_ptr<Region> region );
    QString _generatePeakLabel( int index, const QString& xUnit, const QString& yUnit ) const;
    static Carta::Lib::ProfileInfo _generateProfileInfo( double restFrequency, const QString& restUnit,
    	const QString& statistic, const QString& spectralType, const QString& spectralUnit );
    void _initializeDefaultState();
    void _initializeStatics();
    bool _isPointSource() const;

    void _saveCurve();
    void _setPointSource( bool pointSource );

    static bool m_registered;
    static LineStyles* m_lineStyles;
    static ProfileStatistics* m_stats;
    static ProfilePlotStyles* m_plotStyles;
    static UnitsFrequency* m_frequencyUnits;
    static UnitsWavelength* m_wavelengthUnits;

    CurveData( const QString& path, const QString& id );
    class Factory;

    std::vector<double> m_plotDataX;
    std::vector<double> m_plotDataY;
    std::vector<double> m_fitDataX;
    std::vector<double> m_fitDataY;
    std::vector<std::tuple<double,double,double> > m_gaussParams;
    std::vector<double> m_fitPolyCoeffs;

    double m_fitRMS;
    QString m_fitStatus;

    //So that the curve can be reproduced with different parameters we store the
    //image and region information.
    std::shared_ptr<Layer> m_layer;
    std::shared_ptr<Region> m_region;

    Carta::State::StateInterface m_stateFit;

    bool m_nameSet;

	CurveData( const CurveData& other);
	CurveData& operator=( const CurveData& other );
};
}
}
