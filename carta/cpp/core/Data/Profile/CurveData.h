/***
 * A set of data that comprises a curve.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/IImage.h"
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

class CurveData : public Carta::State::CartaObject {
friend class Profiler;
public:

    /**
     * Return an identifier for the curve.
     * @return - a curve identifier.
     */
    QString getName() const;

    /**
     * Return the internal state of the curve as a string.
     * @return - the curve state.
     */
    QString getStateString() const;

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
     * Set the x- and y- data values that comprise the curve.
     * @param valsX - the x-coordinate values of the curve.
     * @param valsY - the y-coordinate values of the curve.
     */
    void setData( const std::vector<double>& valsX, const std::vector<double>& valsY  );

    /**
     * Set an identifier for the curve.
     * @param curveName - an identifier for the curve.
     */
    void setName( const QString& curveName );

    /**
     * Set the image that was used to generate the curve.
     * @param imageSource - the image that was used to generate the curve.
     */
    void setSource( std::shared_ptr<Carta::Lib::Image::ImageInterface> imageSource );

    virtual ~CurveData();
    const static QString CLASS_NAME;

private:

    void _initializeCallbacks();
    void _initializeDefaultState();


    void _saveCurve();
    static bool m_registered;

    CurveData( const QString& path, const QString& id );
    class Factory;

    std::vector<double> m_plotDataX;
    std::vector<double> m_plotDataY;
    std::shared_ptr<Carta::Lib::Image::ImageInterface> m_imageSource;

	CurveData( const CurveData& other);
	CurveData operator=( const CurveData& other );
};
}
}
