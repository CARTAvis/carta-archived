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

class Colormaps;
class IColoredView;
class Controller;
class TransformsData;
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

    /**
     * Return the server side id of the preferences for this colormap.
     * @return the server side id of this colormap's preferences.
     */
    QString getPreferencesId() const;
    /**
     * Return a string representing the colormap state of a particular type.
     * @param sessionId - an identifier for the user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding colormap state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the colormap is reversed.
     * @return true if the colormap is reversed; false otherwise.
     */
    bool isReversed() const;

    /**
     * Returns whether or not the colormap is inverted.
     * @return true if the colormap is inverted; false, otherwise.
     */
    bool isInverted() const;


    /**
     * Set the name of the current color map.
     * @param colorMapName a unique identifier for the color map.
     * @return error information if the color map was not successfully set.
     */
    QString setColorMap( const QString& colorMapName );

    /**
     * Force a state reload.
     */
    void refreshState() Q_DECL_OVERRIDE;

    /**
     * Reverse the current colormap.
     * @param reverse - true if the colormap should be reversed; false otherwise.
     * @return error information if the color map was not successfully reversed.
     */
    QString reverseColorMap( bool reverse );


    /**
     * Invert the current colormap.
     * @param invert - true if the color map should be inverted; false otherwise..
     * @return error information if the color map was not successfully inverted.
     */
    QString invertColorMap( bool invert );

    /**
     * Restore the state from a string representation.
     * @param state- a json representation of state.
     */
    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

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

    std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> getColorMap( ) const;

    /**
     * Set the number of significant digits to use/display in colormap calculations.
     * @param digits - the number of significant digits to use in calculations.
     * @return an error message if the significant digits could not be sent; an
     *      empty string otherwise.
     */
    QString setSignificantDigits( int digits );


    virtual ~Colormap();
    const static QString CLASS_NAME;

signals:
    void colorMapChanged( Colormap* map );

public slots:
        void setColorProperties( Controller* target );

private:
    QString _commandSetColorMap( const QString& params );
    QString _commandInvertColorMap( const QString& params );
    QString _commandReverseColorMap( const QString& params );
    QString _commandSetColorMix( const QString& params );

    bool _setColorMix( const QString& key, double colorPercent, QString& errorMsg );
    void _initializeDefaultState();
    void _initializeCallbacks();
    void _initializeStatics();

    void _setErrorMargin();

    static bool m_registered;
    const static QString COLOR_MAP_NAME;
    const static QString REVERSE;
    const static QString INVERT;
    const static QString RED_PERCENT;
    const static QString GREEN_PERCENT;
    const static QString BLUE_PERCENT;
    const static QString COLORED_OBJECT;
    const static QString COLOR_MIX;
    const static QString COLOR_MIX_RED;
    const static QString COLOR_MIX_GREEN;
    const static QString COLOR_MIX_BLUE;
    const static QString SCALE_1;
    const static QString SCALE_2;
    const static QString GAMMA;
    const static QString SIGNIFICANT_DIGITS;
    const static QString TRANSFORM_IMAGE;
    const static QString TRANSFORM_DATA;

    Colormap( const QString& path, const QString& id );

    class Factory;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    std::unique_ptr<Settings> m_settings;

    //Supported color maps
    static Colormaps* m_colors;

    //Supported data transforms
    static TransformsData* m_dataTransforms;


    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;

    double m_errorMargin;

	Colormap( const Colormap& other);
	Colormap& operator=( const Colormap& other );
};
}
}
