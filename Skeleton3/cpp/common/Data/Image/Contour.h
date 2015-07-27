/***
 * Represents a single contour.
 */

#pragma once

#include <QString>
#include <State/StateInterface.h>
#include <State/ObjectManager.h>

#include <QPen>

namespace Carta {

namespace Data {

class Contour : public Carta::State::CartaObject {

public:

    QString getStateString() const;
    /**
     * Returns the contour level.
     * @return the contour level.
     */
    double getLevel() const;

    /**
     * Returns the pen that should be used to draw the contour.
     * @return - the QPen for drawing the contour.
     */
    QPen getPen() const;

    /**
     * Set the color and transparency of the contour.
     * @param redAmount - a number between 0 and 255 representing the amount of red.
     * @param greenAmount - a number between 0 and 255 representing the amount of green.
     * @param blueAmount - a number between 0 and 255 representing the amount of blue.
     * @param alphaAmount - a number between 0 and 255 representing the transparency.
     * @return a list of errors if there was a problem setting the color; otherwise a zero
     *  length list.
     */
    QStringList setColor( int redAmount, int greenAmount, int blueAmount, int alphaAmount );

    /**
     * Set the contour level.
     * @param level - the contour level.
     */
    void setLevel( double level );

    /**
     * Set the width of the contour.
     * @param width - set the thickness of the pen used to draw the contour.
     */
    void setWidth( double width );

    virtual ~Contour();


    static const QString CLASS_NAME;
    static const QString LEVEL;


    static bool m_registered;


private:

    static const double ERROR_MARGIN;

    void _initializeState();
    class Factory;
    Contour( const QString& prefix, const QString& identifier);




    Contour( const Contour& other);
    Contour& operator=( const Contour& other );
};
}
}
