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
namespace Lib {

namespace Image {
class ImageInterface;
}
}
}

namespace Carta {
namespace Data {

class InitialGuess;
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
     * Copy the state of the other curve into this one.
     * @param other - the curve whose state should be copied.
     */
    void copy( const std::shared_ptr<CurveData> & other );

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
     * Return the two-dimensional data that represent a fit to this curve.
     * @return - the two-dimensional data that represent a curve fit.
     */
    std::vector< std::pair<double,double> > getFitData() const;

    /**
     * Return stored parameters for fitting this curve.
     * @return - a string representation of fitting parameters.
     */
    QString getFitParams() const;

    /**
     * Returns the image that was used in the profile calculation.
     * @return - the image used in the profile calculation.
     */
    std::shared_ptr<Carta::Lib::Image::ImageInterface> getImage() const;

    /**
     * Return an identifier for the style to use in drawing lines.
     * @return - an identifier for the style used to draw lines.
     */
    QString getLineStyle() const;

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
     * Return the name of the image used to generate the profile curve.
     * @return - the name of the image used to generate the profile curve.
     */
    QString getNameImage() const;

    /**
     * Return the name of the region used to generate the profile curve.
     * @return - the name of the region used to generate the profile curve.
     */
    QString getNameRegion() const;

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
     * Return the image used to generate the curve.
     * @return - the image used to generate the curve.
     */
    std::shared_ptr<Carta::Lib::Image::ImageInterface> getSource() const;

    /**
     * Get the curve x-coordinates.
     * @return - the curve x-coordinate values.
     */
    std::vector<double> getValuesX() const;

    /**
     * Get the curve y-coordinates.
     * @return - the curve y-coordinate values.
     */
    std::vector<double> getValuesY() const;

    /**
     * Returns true if the identifier passed in matches this curve's identifier;
     * false otherwise.
     * @param name - an identifier for a curve.
     * @return - true if the identifiers match; false otherwise.
     */
    bool isMatch( const QString& name ) const;

    /**
     * Return whether or not the curve has been fit.
     * @return - true if the curve has been fit with one or more Gaussian/polynomials, etc;
     *      false otherwise.
     */
    bool isFitted() const;

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
     * Set the y- data values that comprise the curve.
     * @param valsY - the y-coordinate values of the curve.
     */
    void setDataY( const std::vector<double>& valsY );

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
     * Set the name of the layer that is the source of profile.
     * @param imageName - an identifier for the layer that is the source of
     *  the profile.
     */
    QString setImageName( const QString& imageName );

    /**
     * Set the line style (outline,solid, etc) for drawing the curve.
     * @param lineStyle - the style to be used for connecting the curve points.
     */
    QString setLineStyle( const QString& lineStyle );

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
     * @return - an error message if the rest frequency units could not be set;
     *      otherwise, and empty string.
     */
    QString setRestUnits( const QString& restUnits, int significantDigits, double errorMargin);

    /**
     * Set whether the rest units are frequency or wavelength.
     * @param restUnitsFreq - true if rest frequency is specified as frequency; false,
     *      if it is specified as wavelength.
     * @param significantDigits - the number of significant digits to store.
     * @param errorMargin - required difference before deciding a value is significantly different.
     */
    void setRestUnitType( bool restUnitsFreq, int significantDigits, double errorMargin );


    /**
     * Sets whether or not the curve should be fit.
     * @param selected - true if the curve should be fit; false, otherwise.
     */
    void setSelectedFit( bool selected );

    /**
     * Set the method used to summarize profile points.
     * @param stat - the method used to summarize profile points.
     * @return - an error message if the method was unrecognized; an empty string otherwise.
     */
    QString setStatistic( const QString& stat );

    /**
     * Set the image that was used to generate the curve.
     * @param imageSource - the image that was used to generate the curve.
     */
    void setSource( std::shared_ptr<Carta::Lib::Image::ImageInterface> imageSource );

    virtual ~CurveData();
    const static QString CLASS_NAME;

private:

    const static QString COLOR;
    const static QString FIT;
    const static QString FIT_SELECT;
    const static QString STYLE;
    const static QString PLOT_STYLE;
    const static QString STATISTIC;
    const static QString REGION_NAME;
    const static QString IMAGE_NAME;
    const static QString REST_FREQUENCY;
    const static QString REST_FREQUENCY_UNITS;
    const static QString REST_UNIT_FREQ;
    const static QString REST_UNIT_WAVE;

    double _calculateRelativeError( double minValue, double maxValue ) const;
    void _calculateRelativeErrors( double& errorX, double& errorY ) const;
    void _convertRestFrequency( const QString& oldUnits, const QString& newUnits,
            int significantDigits, double errorMargin );


    void _initializeDefaultState();
    void _initializeStatics();

    void _saveCurve();

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
    std::shared_ptr<Region> m_region;

    double m_restFrequency;
    QString m_restUnits;

    std::shared_ptr<Carta::Lib::Image::ImageInterface> m_imageSource;

    Carta::State::StateInterface m_stateFit;

	CurveData( const CurveData& other);
	CurveData& operator=( const CurveData& other );
};
}
}
