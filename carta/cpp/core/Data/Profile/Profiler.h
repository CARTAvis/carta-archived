/***
 * Manages Profiler settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "CartaLib/IImage.h"
#include "ProfileExtractor.h"

#include <QObject>

namespace Carta {
namespace Lib {

namespace Image {
class ImageInterface;
}
}
}


class ImageView;


namespace Carta {
namespace Plot2D {
class Plot2DGenerator;
}

namespace Data {

class Plot2DManager;
class Controller;
class CurveData;
class IntensityUnits;
class LinkableImpl;
class Settings;
class SpectralUnits;

class Profiler : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    QString addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    QString removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    virtual QList<QString> getLinks() const Q_DECL_OVERRIDE;


    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;


    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Set the bottom axis units.
     * @param unitStr - set the label to use for the bottom axis of the plot.
     * @return - an error message if the units could not be set; otherwise, an
     *      empty string.
     */
    QString setAxisUnitsBottom( const QString& unitStr );

    /**
     * Set the left axis units.
     * @param unitStr - set the label to use for the left axis of the plot.
     * @return - an error message if the units could not be set; otherwise, an
     *      empty string.
     */
    QString setAxisUnitsLeft( const QString& unitStr );

    QString setClipBuffer( int bufferAmount );
    QString setClipMax( double clipMaxClient );
    QString setClipMin( double clipMinClient );
    QString setClipMaxPercent( double clipMaxClient );
    QString setClipMinPercent( double clipMinClient );
    QString setClipRange( double clipMin, double clipMax );
    QString setClipRangePercent( double clipMinPercent, double clipMaxPercent );


    /**
     * Set the drawing style for the Profiler (outline, filled, etc).
     * @param style a unique identifier for a Profiler drawing style.
     * @return an error message if there was a problem setting the draw style; an empty string otherwise.
     */
    QString setGraphStyle( const QString& style );

    QString setUseClipBuffer( bool useBuffer );

    virtual ~Profiler();
    const static QString CLASS_NAME;


private slots:
    void _updateChannel( Controller* controller );
    void _generateProfile( Controller* controller = nullptr );

private:
    const static QString AXIS_UNITS_BOTTOM;
    const static QString AXIS_UNITS_LEFT;
    const static QString CLIP_BUFFER;
    const static QString CLIP_BUFFER_SIZE;
    const static QString CLIP_MIN;
    const static QString CLIP_MAX;
    const static QString CLIP_MIN_CLIENT;
    const static QString CLIP_MAX_CLIENT;
    const static QString CLIP_MIN_PERCENT;
    const static QString CLIP_MAX_PERCENT;
    const static QString CURVES;

    //Convert axis units.
    std::vector<double> _convertUnitsX( std::shared_ptr<CurveData> curveData,
            const QString& newUnit = QString() ) const;
    std::vector<double> _convertUnitsY( std::shared_ptr<CurveData> curveData ) const;


    Controller* _getControllerSelected() const;
    void _loadProfile( Controller* controller);
    
    /**
     * Returns the server side id of the Profiler user preferences.
     * @return the unique server side id of the user preferences.
     */
    QString _getPreferencesId() const;

    void _initializeDefaultState();
    void _initializeCallbacks();
    void _initializeStatics();

    //Notify the plot to redraw.
    void _updatePlotData();
    QString _zoomToSelection();

    //Breaks a string of the form "Frequency (GHz)" into a type "Frequency"
    //and units "GHz".
    static QString _getUnitType( const QString& unitStr );
    static QString _getUnitUnits( const QString& unitStr );
    static bool m_registered;

    //For right now we are supporting only one linked controller.
    bool m_controllerLinked;

    Profiler( const QString& path, const QString& id );
    class Factory;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Preferences
    std::unique_ptr<Settings> m_preferences;

    std::unique_ptr<Plot2DManager> m_plotManager;

    //Plot data
    QList< std::shared_ptr<CurveData> > m_plotCurves;
    QString m_leftUnit;
    QString m_bottomUnit;

    //State specific to the data that is loaded.
    Carta::State::StateInterface m_stateData;

    static SpectralUnits* m_spectralUnits;
    static IntensityUnits* m_intensityUnits;

	Profiler( const Profiler& other);
	Profiler operator=( const Profiler& other );
};
}
}
