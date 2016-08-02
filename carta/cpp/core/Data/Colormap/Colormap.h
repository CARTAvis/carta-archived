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
class Gamma;
class Settings;
class UnitsIntensity;

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

    /**
     * Return the units used to set the intensity range on the colormap.
     * @return - units used to set the colormap intensity range.
     */
    QString getImageUnits() const;

    /**
     * Return the number of significant digits to use in the display.
     * @return - the number of significant digits to retain.
     */
    int getSignificantDigits() const;

    /**
     * Return the current settings tab index.
     * @return - the current settings tab index.
     */
    int getTabIndex() const;

    /**
     * Return a string representing the colormap state of a particular type.
     * @param sessionId - an identifier for the user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding colormap state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Returns true if changes to the color map should apply to all color maps;
     * false if they only apply to the current color map.
     * @return - true if changes to the color map apply globally; false if they apply
     *      to only the current map.
     */
    bool isGlobal() const;

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
     * @return - true if the default nan color should be used; false, otherwise.
     */
    bool isNanDefault() const;

    /**
     * Returns true if the default border color should be used; false otherwise.
     * @return - true if the default border/transparency is used; false for a user specified one.
     */
    bool isBorderDefault() const;

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
     * Set the color of the plot border.
     * @param redValue - the amount of red [0,255].
     * @param greenValue - the amount of green [0,255].
     * @param blueValue - the amount of blue [0,255].
     */
    QString setBorderColor( int redValue, int greenValue, int blueValue );

    /**
     * Set the transparency of the plot border.
     * @param alphaValue - how transparent the border should be (255 complete opaque,
     *      0 transparent).
     * @return - an error message if the border transparency could not be set; an empty
     *      string otherwise.
     */
    QString setBorderAlpha( int alphaValue );

    /**
     * Set whether to use a default border around the plot.
     * @param useDefault - true if the default border should be used; false, if it is a
     *      user settable default border.
     */
    QString setBorderDefault( bool useDefault );

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
     * Set the units to use for intensity bounds.
     * @param unitsStr - an identifier for intensity units.
     * @return - an error message if the intensity units could not be set; otherwise,
     *      an empty string.
     */
    QString setImageUnits( const QString& unitsStr );

    /**
     * Set the colormap intensity range.
     * @param minValue - the minimum intensity.
     * @param maxValue - the maximum intensity.
     */
    QString setIntensityRange( double minValue, double maxValue );

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

    /**
     * Set the index of the tab that should be selected.
     * @param index - the index of the user selected settings tab.
     * @return an error message if there was a problem setting the tab index; an
     *      empty string otherwise.
     */
    QString setTabIndex( int index );


    virtual ~Colormap();
    const static QString CLASS_NAME;

signals:
    /**
     * Notification that the color map has changed.
     */
    void colorMapChanged();

private slots:
    void _updateIntensityBounds( double minIntensity, double maxIntensity );
    void _colorStateChanged();
    void _dataChanged( Controller* controller );
    /**
     * Set the color states managed by this color map.
     */
    void _setColorStates( Controller* target );

private:
    void _calculateColorStops();
    QString _commandSetColorMap( const QString& params );
    QString _commandInvertColorMap( const QString& params );
    QString _commandReverseColorMap( const QString& params );
    QString _commandSetColorMix( const QString& params );

    std::pair<double,double>  _convertIntensity( const QString& oldUnit, const QString& newUnit );
    std::pair<double,double> _convertIntensity( const QString& oldUnit, const QString& newUnit,
            double minValue, double maxValue );

    Controller* _getControllerSelected() const;
    std::vector<std::pair<int,double> > _getIntensityForPercents( std::vector<double>& percent ) const;

    /**
     * Return the server side id of the preferences for this colormap.
     * @return the server side id of this colormap's preferences.
     */
    QString _getPreferencesId() const;

    void _initializeDefaultState();
    void _initializeCallbacks();
    void _initializeStatics();

    void _setErrorMargin( );

    void _updateImageClips();

    static bool m_registered;

    const static QString COLOR_STOPS;
    const static QString GLOBAL;
    const static QString IMAGE_UNITS;
    const static QString INTENSITY_MIN;
    const static QString INTENSITY_MAX;
    const static QString INTENSITY_MIN_INDEX;
    const static QString INTENSITY_MAX_INDEX;
    const static QString SIGNIFICANT_DIGITS;
    const static QString TAB_INDEX;


    Colormap( const QString& path, const QString& id );

    class Factory;

    double m_errorMargin;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    std::unique_ptr<Settings> m_settings;

    //Image units
    static UnitsIntensity* m_intensityUnits;
    //Gamma
    static Gamma* m_gammaTransform;

    Carta::State::StateInterface m_stateData;

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;

    //Holds the current color state.  Note that it is a vector because
    //multiple images can be selected for applying a color state change.
    std::vector< std::shared_ptr<ColorState> > m_stateColors;

	Colormap( const Colormap& other);
	Colormap& operator=( const Colormap& other );
};
}
}
