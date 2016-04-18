/***
 * A set of data that comprises a curve.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/IImage.h"
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

class LineStyles;
class ProfileStatistics;
class Region;

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
     * Return an identifier for the style to use in drawing lines.
     * @return - an identifier for the style used to draw lines.
     */
    QString getLineStyle() const;


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
     * Set the image that was used to generate the curve.
     * @param imageSource - the image that was used to generate the curve.
     */
    void setSource( std::shared_ptr<Carta::Lib::Image::ImageInterface> imageSource );

    virtual ~CurveData();
    const static QString CLASS_NAME;

private:

    const static QString COLOR;
    const static QString STYLE;
    const static QString STATISTIC;
    const static QString REGION_NAME;
    const static QString IMAGE_NAME;
    const static QString REST_FREQUENCY;

    void _initializeDefaultState();
    void _initializeStatics();


    void _saveCurve();
    static bool m_registered;
    static LineStyles* m_lineStyles;
    static ProfileStatistics* m_stats;

    CurveData( const QString& path, const QString& id );
    class Factory;

    std::vector<double> m_plotDataX;
    std::vector<double> m_plotDataY;
    std::shared_ptr<Region> m_region;
    std::shared_ptr<Carta::Lib::Image::ImageInterface> m_imageSource;

	CurveData( const CurveData& other);
	CurveData operator=( const CurveData& other );
};
}
}
