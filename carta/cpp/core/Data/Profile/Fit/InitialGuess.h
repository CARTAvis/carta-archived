/***
 * An initial of the parameters for a Gaussian curve fit to a set
 * of data.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/IImage.h"
#include "CartaLib/ProfileInfo.h"
#include <QColor>


namespace Carta {
namespace Data {

class IScreenTranslator;


class InitialGuess : public Carta::State::CartaObject {

public:

    /**
     * Notification that the mapping between pixel coordinates and
     * data coordinates has changed.
     */
    void pixelsChanged();

    /**
     * Set a guess for the amplitude of the Gaussian curve.
     * @param peak - a guess for the amplitude of the curve.
     */
    void setPeak( double peak );

    /*
     * Set a guess for the single beam half width of the Gaussian curve.
     * @param fbhw - a guess for the single beam half width of the Gaussian
     *      curve.
     */
    void setFBHW( double fbhw );

    /**
     * Set a guess for the center of the Gaussian curve.
     * @param center - a guess for the center of the Gaussian curve.
     */
    void setCenter( double center );

    /**
     * Set an object for translating between screen coordinates and data
     * coordinates.
     * @param trans - an object for translating between screen and data
     *      coordinates.
     */
    void setScreenTranslator( std::shared_ptr<IScreenTranslator> trans );

    virtual ~InitialGuess();
    const static QString CLASS_NAME;

private:

    const static QString CENTER;
    const static QString PEAK;
    const static QString FBHW;
    const static double ERROR_MARGIN;

    void _initializeDefaultState();

    InitialGuess( const QString& path, const QString& id );

    class Factory;
    static bool m_registered;

    double m_center;
    double m_peak;
    double m_fbhw;
    std::shared_ptr<IScreenTranslator> m_screenTranslator;

	InitialGuess( const InitialGuess& other);
	InitialGuess& operator=( const InitialGuess& other );
};
}
}
