/***
 * Represents a single contour.
 */

#pragma once

#include <QString>
#include <State/StateInterface.h>
#include <cmath>

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
     * Returns whether or not the contour should be drawn.
     * @return true if the contour is visible; false otherwise.
     */
    bool isVisible() const;

    /**
     * Comparator for contours based on level.
     * @param other - the comparison Contour.
     */
    bool operator<( const Contour& other ) const;

    /**
     * Set the transparency level of the contour.
     * @param alphaAmount - a number between 0 and 255 representing the transparency.
     * @param changedState -set to true if the thickness of the contour is changed; false otherwise.
     * @return an error message if the transparency could not be set; and empty string otherwise.
     */
    QString setAlpha( int alphaAmount, bool* changedState );

    /**
     * Set the color and transparency of the contour.
     * @param redAmount - a number between 0 and 255 representing the amount of red.
     * @param greenAmount - a number between 0 and 255 representing the amount of green.
     * @param blueAmount - a number between 0 and 255 representing the amount of blue.
     * @param changedState -set to true if the thickness of the contour is changed; false otherwise.
     * @return a list of errors if there was a problem setting the color; otherwise a zero
     *  length list.
     */
    QStringList setColor( int redAmount, int greenAmount, int blueAmount, bool* changedState );

    /**
     * Set the contour level.
     * @param level - the contour level.
     * @param changedState -set to true if the thickness of the contour is changed; false otherwise.
     */
    void setLevel( double level,  bool* changedState );

    void setStateString( const QString& stateStr );

    /**
     * Set the contour style.
     * @param style - the style of the contour (dashed, etc).
     * @param changedState -set to true if the thickness of the contour is changed; false otherwise.
     */
    QString setStyle( const QString& style, bool* changedState );

    /**
     * Set the visibility of the contour level.
     * @param visible - true if the contour level should be visible; false otherwise.
     * @return true if the visibility changed; false otherwise.
     */
    bool setVisible( bool visible );

    /**
     * Set the width of the contour.
     * @param width - set the thickness of the pen used to draw the contour.
     * @param changedState -set to true if the thickness of the contour is changed; false otherwise.
     */
    QString setWidth( double width, bool* changedState );

    static const QString CLASS_NAME;
    static const QString LEVEL;
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
