/***
 * Manages colormap settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "Data/LinkableImpl.h"

class ImageView;

namespace Carta {
namespace Lib {
namespace PixelPipeline {
class IColormapNamed;
}
}
}

namespace Carta {

namespace Data {

class ColorState;
class Controller;
class Settings;

class Colormap : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    virtual QString addLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;
    virtual QString removeLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;
    virtual QList<QString> getLinks() const Q_DECL_OVERRIDE;

    /**
     * Clear existing state.
     */
    void clear();


    std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> getColorMap( ) const;

    /**
     * Return a string representing the colormap state of a particular type.
     * @param sessionId - an identifier for the user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding colormap state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;


    /**
     * Returns whether or not the colormap is inverted.
     * @return true if the colormap is inverted; false, otherwise.
     */
    bool isInverted() const;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;

    /**
     * Returns true if the nan color will be derived from the bottom of the color map; false,
     * if the nan color is set be the user.
     */
    bool isNanDefault() const;

    /**
     * Returns whether or not the colormap is reversed.
     * @return true if the colormap is reversed; false otherwise.
     */
    bool isReversed() const;

    /**
     * Force a state reload.
     */
    void refreshState() Q_DECL_OVERRIDE;

    /**
     * Restore the state from a string representation.
     * @param state- a json representation of state.
     */
    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Set the name of the current color map.
     * @param colorMapName a unique identifier for the color map.
     * @return error information if the color map was not successfully set.
     */
    QString setColorMap( const QString& colorMapName );


    /**
     * Set a color mix.
     * @param redValue a number in [0,1] representing the amount of red in the mix.
     * @param greenValue a number in [0,1] representing the amount of green in the mix.
     * @param blueValue a number in [0,1] representing the amount of blue in the mix.
     * @return error information if the color mix was not successfully set.
     */
    QString setColorMix( double redValue, double greenValue, double blueValue );

    /**
     * Set the name of the data transform.
     * @param transformString a unique identifier for a data transform.
     * @return error information if the data transfrom was not set.
     */
    QString setDataTransform( const QString& transformString );

    /**
     * Set the gamma color map parameter.
     * @param gamma a parameter for color mapping.
     * @return error information if gamma could not be set.
     */
    QString setGamma( double gamma );

    /**
     * Set whether or not the color map is global.
     * @param global - true if the colormap is global; false otherwise.
     */
    void setGlobal( bool global );

    /**
     * Invert the current colormap.
     * @param invert - true if the color map should be inverted; false otherwise..
     * @return error information if the color map was not successfully inverted.
     */
    QString setInvert( bool invert );

    /**
     * Set the color used for nan values.
     * @param redValue - the amount of red [0,255].
     * @param greenValue - the amount of green [0,255].
     * @param blueValue - the amount of blue [0,255].
     */
    //Note:  This color will only be used if using the default nan color is
    //set to false.  If using the default nan is true, the nan color will be
    //ignored.
    QString setNanColor( int redValue, int greenValue, int blueValue );

    /**
     * Set whether or not to use the default nan color (bottom of the color
     * map).
     * @param nanDefault - true if the bottom color map value should be the
     *      nan color; false if the nan color should be user specified.
     * @return an error message if whether or not to use the default nan
     *      color could not be set.
     */
    QString setNanDefault( bool nanDefault );

    /**
     * Reverse the current colormap.
     * @param reverse - true if the colormap should be reversed; false otherwise.
     * @return error information if the color map was not successfully reversed.
     */
    QString setReverse( bool reverse );


    /**
     * Set the number of significant digits to use/display in colormap calculations.
     * @param digits - the number of significant digits to use in calculations.
     * @return an error message if the significant digits could not be sent; an
     *      empty string otherwise.
     */
    QString setSignificantDigits( int digits );


    virtual ~Colormap();
    const static QString CLASS_NAME;

private slots:
    void _updateIntensityBounds( double minIntensity, double maxIntensity );
    void _colorStateChanged();
    /**
     * Set the color states managed by this color map.
     */
    void _setColorStates( Controller* target );

private:
    QString _commandSetColorMap( const QString& params );
    QString _commandInvertColorMap( const QString& params );
    QString _commandReverseColorMap( const QString& params );
    QString _commandSetColorMix( const QString& params );

    /**
     * Return the server side id of the preferences for this colormap.
     * @return the server side id of this colormap's preferences.
     */
    QString _getPreferencesId() const;

    void _initializeDefaultState();
    void _initializeCallbacks();
    bool _isGlobal() const;


    static bool m_registered;

    const static QString INTENSITY_MIN;
    const static QString INTENSITY_MAX;


    Colormap( const QString& path, const QString& id );

    class Factory;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    std::unique_ptr<Settings> m_settings;

    Carta::State::StateInterface m_stateData;

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;

    //Holds the current color state
    std::vector< std::shared_ptr<ColorState> > m_stateColors;
    //Holds the global color state;
    std::shared_ptr<ColorState> m_stateColorGlobal;

	Colormap( const Colormap& other);
	Colormap& operator=( const Colormap& other );
};
}
}
