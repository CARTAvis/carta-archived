/***
 * Stores the state for a color map.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"


namespace Carta {
namespace Lib {
namespace PixelPipeline {
class IColormapNamed;
}
}
}

namespace Carta {

namespace Data {

class Colormaps;
class Controller;
class TransformsData;

class ColorState : public QObject, public Carta::State::CartaObject {

    friend class Colormap;
    friend class ControllerData;

    Q_OBJECT

public:

    /**
     * Return a string representing the colormap state of a particular type.
     * @param sessionId - an identifier for the user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding colormap state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;


    virtual ~ColorState();
    const static QString CLASS_NAME;

signals:
    /*
     * Emitted when the state of the color map has changed.
     */
    void colorStateChanged();


private:

    /**
     * Return the name of the color map.
     * @return - the name of the color map.
     */
    QString _getColorMap() const;

    QString _getDataTransform() const;

    double _getGamma() const;

    double _getMixGreen() const;
    double _getMixRed() const;
    double _getMixBlue() const;

    int _getNanGreen() const;
    int _getNanRed() const;
    int _getNanBlue() const;

    void _initializeDefaultState( Carta::State::StateInterface& state );
    void _initializeStatics();


    bool _isGlobal() const;
    bool _isNanDefault() const;

    /**
     * Returns whether or not the colormap is reversed.
     * @return true if the colormap is reversed; false otherwise.
     */
    bool _isReversed() const;

    /**
     * Returns whether or not the colormap is inverted.
     * @return true if the colormap is inverted; false, otherwise.
     */
    bool _isInverted() const;

    /**
     * Copy the state of this ColorState into the other state.
     * @param other - the StateInterface that should be a copy of this one.
     */
    void _replicateTo( Carta::State::StateInterface& other );


    /**
     * Copy the state of this ColorState into the other state.
     * @param other - the ColorState that should be a copy of this one.
     */
    void _replicateTo( ColorState* cState );


    /**
     * Set the name of the current color map.
     * @param colorMapName a unique identifier for the color map.
     * @return error information if the color map was not successfully set.
     */
    QString _setColorMap( const QString& colorMapName );

    /**
     * Set a color mix.
     * @param redValue a number in [0,1] representing the amount of red in the mix.
     * @param greenValue a number in [0,1] representing the amount of green in the mix.
     * @param blueValue a number in [0,1] representing the amount of blue in the mix.
     * @return error information if the color mix was not successfully set.
     */
    QString _setColorMix( double redValue, double greenValue, double blueValue );

    /**
     * Helper function that sets an individual color in the mix.
     * @param key - an identifier for the individual color.
     * @param colorPercent - the color value in [0,1].
     * @param errorMsg - information about the problem if the color was not set.
     * @return true if the color changed value; false otherwise.
     */
    bool _setColorMix( const QString& key, double colorPercent, QString& errorMsg );

    /**
     * Set the name of the data transform.
     * @param transformString a unique identifier for a data transform.
     * @return error information if the data transfrom was not set.
     */
    QString _setDataTransform( const QString& transformString);


    /**
     * Set the error margin used to determine when two doubles are equal.
     */
    void _setErrorMargin();

    /**
     * Set the gamma color map parameter.
     * @param gamma a parameter for color mapping.
     * @return error information if gamma could not be set.
     */
    QString _setGamma( double gamma );

    /**
     * Set whether or not this is the global color state.
     * @param global - true if it is the global color state; false, if it just applies
     *      to a single layer on the stack.
     */
    void _setGlobal( bool global );

    /**
     * Invert the current colormap.
     * @param invert - true if the color map should be inverted; false otherwise..
     * @return error information if the color map was not successfully inverted.
     */
    void _setInvert( bool invert );

    QString _setNanColor( int redValue, int greenValue, int blueValue );

    bool _setNanColor( const QString& key, int colorAmount, QString& errorMsg );

    void _setNanDefault( bool defaultNan );
    /**
     * Reverse the current colormap.
     * @param reverse - true if the colormap should be reversed; false otherwise.
     * @return error information if the color map was not successfully reversed.
     */
    void _setReverse( bool reverse );


    /**
     * Set the number of significant digits to use/display in colormap calculations.
     * @param digits - the number of significant digits to use in calculations.
     * @return an error message if the significant digits could not be sent; an
     *      empty string otherwise.
     */
    QString _setSignificantDigits( int digits );


    static bool m_registered;
    const static QString COLOR_MAP_NAME;
    const static QString REVERSE;
    const static QString INVERT;
    const static QString GLOBAL;
    const static QString COLORED_OBJECT;
    const static QString COLOR_MIX;

    const static QString INTENSITY_MIN;
    const static QString INTENSITY_MAX;
    const static QString SCALE_1;
    const static QString SCALE_2;
    const static QString GAMMA;
    const static QString NAN_COLOR;
    const static QString NAN_DEFAULT;
    const static QString SIGNIFICANT_DIGITS;
    const static QString TRANSFORM_IMAGE;
    const static QString TRANSFORM_DATA;

    ColorState( const QString& path, const QString& id );

    class Factory;


    //Supported color maps
    static Colormaps* m_colors;

    //Supported data transforms
    static TransformsData* m_dataTransforms;


    double m_errorMargin;

	ColorState( const ColorState& other);
	ColorState& operator=( const ColorState& other );
};
}
}
