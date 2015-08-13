/***
 * Represents a single contour.
 */

#pragma once

#include <QString>
#include <State/StateInterface.h>

#include <QPen>

namespace Carta {

namespace Data {

class ContourStyles;

class Contour{

public:


    Contour();

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
     * Comparator for contours based on level.
     * @param other - the comparison Contour.
     */
    bool operator<( const Contour& other ) const;

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
     * Set the contour style.
     * @param style - the style of the contour (dashed, etc).
     */
    QString setStyle( const QString& style );

    /**
     * Set the visibility of the contour level.
     * @param visible - true if the contour level should be visible; false otherwise.
     * @return true if the visibility changed; false otherwise.
     */
    bool setVisible( bool visible );

    /**
     * Set the width of the contour.
     * @param width - set the thickness of the pen used to draw the contour.
     */
    QString setWidth( double width );

    static const QString CLASS_NAME;
    static const QString LEVEL;
    static const QString STYLE;
    static const QString VISIBLE;
    static const double ERROR_MARGIN;

    virtual ~Contour();

private:

    static ContourStyles* m_contourStyles;
    Carta::State::StateInterface m_state;

    void _initializeSingletons();
    void _initializeState();


    //Contour( const Contour& other);
    Contour& operator=( const Contour& other );
};
}
}
