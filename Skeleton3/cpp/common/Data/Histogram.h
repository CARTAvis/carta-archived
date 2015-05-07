/***
 * Manages histogram settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "CartaLib/IImage.h"

#include <QObject>

namespace Carta {
namespace Lib {
namespace PixelPipeline {
class IColormapNamed;
}
}
}

namespace Image {
class ImageInterface;

}

class ImageView;


namespace Carta {
namespace Histogram {
class HistogramGenerator;
}

namespace Data {

class ChannelUnits;
class Clips;
class Colormap;
class Controller;
class LinkableImpl;

class Histogram : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    QString addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    QString removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;

    /**
     * Applies clips to image.
     */
    void applyClips();

    /**
     * Clear the state of the histogram.
     */
    void clear();

    /**
     * Return a string representing the histogram state of a particular type.
     * @param type - the type of state needed.
     * @param sessionId - an identifier for a user's session.
     * @return a QString representing the corresponding histogram state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Send a new state update to the client.
     */
    void refreshState();

    /**
     * Reset the data dependent state of the histogram.
     * @param state - information such as clipping that depends on the data loaded.
     */
    virtual void resetStateData( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Set the amount of extra space on each side of the clip bounds.
     * @param bufferAmount a percentage in [0,100) representing the amount of extra space.
     * @return an error message if the clip buffer was not successfully set; an empty string otherwise.
     */
    QString setClipBuffer( int bufferAmount );

    /**
     * Set whether or not to show extra space on each side of the clip bounds.
     * @param useBuffer true if extra space should be shown; false otherwise.
     * @return an error message if there was a problem; an empty string if the flag was set successfully.
     */
    QString setUseClipBuffer( bool useBuffer );

    /**
     * Set the maximum clip value.
     * @param clipMax the upper bound for the histogram.
     * @param complete true if the change should propagate to the client and redraw the histogram;
     *      false otherwise.
     * @return an error message if there was a problem setting the upper bound; an empty string otherwise.
     */
    QString setClipMax( double clipMax, bool complete = true );

    /**
     * Set the minimum clip value.
     * @param clipMin the lower bound for the histogram.
     * @param complete true if the change should propagate to the client and redraw the histogram;
     *      false otherwise.
     * @return an error message if there was a problem setting the lower bound; an empty string otherwise.
    */
    QString setClipMin( double clipMin, bool complete = true );

    /**
      * Set the percent to clip from the left side of the histogram.
      * @param clipMinPercent  a number in [0,100) representing the amount to clip from the left side.
      * @param complete true if the change should propagate to the client and redraw the histogram;
      *      false otherwise.
      * @return an error message if there was a problem setting the minimum percent; an empty string otherwise.
    */
    QString setClipMinPercent( double clipMinPercent, bool complete = true );

    /**
     * Set the percent to clip from the right side of the histogram.
     * @param clipMaxPercent  a number in [0,100) representing the amount to clip from the right side.
     * @param complete true if the change should propagate to the client and redraw the histogram;
     *      false otherwise.
     * @return an error message if there was a problem setting the maximum percent; an empty string otherwise.
     */
    QString setClipMaxPercent( double clipMaxPercent, bool complete = true );

    /**
     * Set the lower and upper bounds for the histogram horizontal axis.
     * @param minRange a lower bound for the histogram horizontal axis.
     * @param maxRange an upper bound for the histogram horizontal axis.
     * @return an error message if there was a problem setting the range; an empty string otherwise.
     */
    QString setClipRange( double minRange, double maxRange );

    /**
     * Set the lower and upper bounds for the histogram as percentages of the entire range.
     * @param minPercent a number in [0,100) representing the amount to leave off on the left.
     * @param maxPercent a number in [0,100) representing the amount to leave off on the right.
     * @return an error message if there was a problem setting the range; an empty string otherwise.
     */
    QString setClipRangePercent( double minPercent, double maxPercent );

    /**
     * Set the number of bins in the histogram.
     * @param binCount the number of histogram bins.
     * @return an error message if there was a problem setting the bin count; an empty string otherwise.
     */
    QString setBinCount( int binCount );

    /**
     * Set the width of the histogram bins.
     * @param binWidth the histogram bin width.
     * @return an error message if there was a problem setting the bin width; an empty string otherwise.
     */
    QString setBinWidth( double binWidth );

    /**
     * Set where or not the histogram should be colored by intensity.
     * @param colored true if the histogram should be colored by intensity; false otherwise.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QString setColored( bool colored );

    /**
     * Set the drawing style for the histogram (outline, filled, etc).
     * @param style a unique identifier for a histogram drawing style.
     * @return an error message if there was a problem setting the draw style; an empty string otherwise.
     */
    QString setGraphStyle( const QString& style );

    /**
     * Set whether or not the vertical axis should use a log scale.
     * @param logCount true if the vertical axis should be logarithmic; false otherwise.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QString setLogCount( bool logCount );

    /**
     * Set whether the histogram should be based on a single plane, a range of planes, or the entire cube.
     * @param planeMode a unique identifier for the 3D data range.
     * @return an error message if there was a problem setting the 3D data range; an empty string otherwise.
     */
    QString setPlaneMode( const QString& planeMode );

    /**
     * Set the range of channels to include as data in generating the histogram.
     * @param minPlane the minimum frequency (GHz) to include.
     * @param maxPlane the maximum frequency (GHz) to include.
     * @return an error message if there was a problem setting the frame range; an empty string otherwise.
     */
    QString setPlaneRange( double minPlane, double maxPlane);

    /**
     * Set whether or not to use a custom clip or just clip based on zoom.
     * @param customClip true to use a seperate clip from the zoom; false otherwise.
     * @return an error message if there was a problem; an empty string otherwise.
     */
    QString setCustomClip( bool customClip );

    /**
     * Set the range in intensity units for the custom clip.
     * @param colorMin a lower bound for the custom clip in real units.
     * @param colorMax an upper bound for the dustom clip in real units.
     * @return an error message if there was a problem setting the custom clip range;
     *      false otherwise.
     */
    QString setRangeColor( double colorMin, double colorMax );

    /**
     * Set the unit used to specify a channel range, for example, "GHz".
     * @param units the channel units used to specify a range.
     * @return an error message if there was a problem setting the channel units;
     *      otherwise and empty string.
     */
    QString setChannelUnit( const QString& units );

    /**
     * Set the lower boundary in intensity units of the custom clip.
     * @param colorMin the lower boundary of a custom clip.
     * @param finish true if a state change should be applied; false if other
     *      state parameters will be set first before propagating the change.
     * @return an error message if there was a problem setting the minimum custom clip value;
     *      an empty string otherwise.
     */
    QString setColorMin( double colorMin, bool finish );

    /**
      * Set the upper boundary in intensity units of the custom clip.
      * @param colorMax the upper boundary of a custom clip.
      * @param finish true if a state change should be applied; false if other
      *      state parameters will be set first before propagating the change.
      * @return an error message if there was a problem setting the maximum custom clip value;
      *      an empty string otherwise.
      */
    QString setColorMax( double colorMax, bool finish );

    /**
      * Set the upper boundary of the custom clip as a percentage.
      * @param colorMaxPercent the upper boundary of a custom clip (100=no clip).
      * @param finish true if a state change should be applied; false if other
      *      state parameters will be set first before propagating the change.
      * @return an error message if there was a problem setting the maximum clip percentage;
      *      an empty string otherwise.
      */
    QString setColorMaxPercent( double colorMaxPercent, bool complete );

    /**
      * Set the lower boundary of the custom clip as a percentage.
      * @param colorMinPercent the lower boundary of a custom clip (0=no clip).
      * @param finish true if a state change should be applied; false if other
      *      state parameters will be set first before propagating the change.
      * @return an error message if there was a problem setting the minimum clip percentage;
      *      an empty string otherwise.
      */
    QString setColorMinPercent( double colorMinPercent, bool complete );

    /**
     * Set the number of significant digits to use in calculations.
     * @param digits a positive number indicating the number of significant digits to use in calculations.
     * @return an error message if there was a problem setting the number of significant digits;
     *      an empty string otherwise.
     */
    QString setSignificantDigits( int digits );

    virtual ~Histogram();
    const static QString CLASS_NAME;
    const static QString GRAPH_STYLE_LINE;
    const static QString GRAPH_STYLE_OUTLINE;
    const static QString GRAPH_STYLE_FILL;

protected:
    virtual QString getType(CartaObject::SnapshotType snapType) const Q_DECL_OVERRIDE;

private slots:
    void  _generateHistogram( bool newDataNeeded, Controller* controller=nullptr);
    void _createHistogram( Controller* );
    void _updateColorMap( Colormap* );
    void _updateSize( const QSize& size );
    void _updateChannel( Controller* controller );
    

private:

    void _finishClips();
    void _finishColor();

    double _getBufferedIntensity( const QString& clipKey, const QString& percentKey );
    std::pair<int,int> _getFrameBounds() const;
    double _getPercentile( const QString& fileName, int frameIndex, double intensity ) const;
    bool _getIntensity( const QString& fileName, int frameIndex, double percentile, double* intensity ) const;
    Controller* _getControllerSelected() const;
    void _loadData( Controller* controller);

    QString _set2DFootPrint( const QString& params );
    void _setErrorMargin();

    /**
    * Set the single plane that should be used for data when the histogram is in single plane mode.
    * @param channel the single frame to use for histogram data.
    * @return an error message if there was a problem setting the channel; an empty string otherwise.
    */
   QString setCubeChannel( int channel );




    std::vector<std::shared_ptr<Image::ImageInterface>> _generateData(Controller* controller);
    
    //Bin count <-> Bin width conversion.
    double _toBinWidth( int count ) const;
    int _toBinCount( double width ) const;

    //User range selection
    void _startSelection(const QString& params );
    void _startSelectionColor( const QString& params );
    void _updateSelection(int x );
    void  _updateColorSelection();
    void _endSelection(const QString& params );
    void _endSelectionColor(const QString& params );

    void _initializeDefaultState();
    void _initializeCallbacks();
    void _initializeStatics();

    void _refreshView();
    void _resetBinCountBasedOnWidth();

    void _zoomToSelection();

    static bool m_registered;

    bool m_selectionEnabled;
    double m_selectionStart;
    double m_selectionEnd;
    bool m_selectionEnabledColor;

    const static QString CLIP_BUFFER;
    const static QString CLIP_BUFFER_SIZE;
    const static QString CLIP_MIN;
    const static QString CLIP_MAX;
    const static QString CLIP_APPLY;
    const static QString CUSTOM_CLIP;
    const static QString BIN_COUNT;
    const static QString BIN_WIDTH;
    const static QString COLOR_MIN;
    const static QString COLOR_MAX;
    const static QString COLOR_MIN_PERCENT;
    const static QString COLOR_MAX_PERCENT;
    const static QString FREQUENCY_UNIT;
    const static QString GRAPH_STYLE;
    const static QString GRAPH_LOG_COUNT;
    const static QString GRAPH_COLORED;
    const static QString PLANE_MODE;
    const static QString PLANE_MODE_SINGLE;
    const static QString PLANE_MODE_RANGE;
    const static QString PLANE_MODE_ALL;
    const static QString PLANE_MIN;
    const static QString PLANE_MAX;
    const static QString FOOT_PRINT;
    const static QString FOOT_PRINT_IMAGE;
    const static QString FOOT_PRINT_REGION;
    const static QString FOOT_PRINT_REGION_ALL;
    const static QString CLIP_MIN_PERCENT;
    const static QString CLIP_MAX_PERCENT;
    const static QString X_COORDINATE;
    const static QString POINTER_MOVE;
    const static QString SIGNIFICANT_DIGITS;
    
    static ChannelUnits* m_channelUnits;

    double m_errorMargin;

    //For right now we are supporting only one linked controller.
    bool m_controllerLinked;

    Histogram( const QString& path, const QString& id );
    class Factory;

    int m_cubeChannel;
    //Data View
    std::shared_ptr<ImageView> m_view;

    static Clips*  m_clips;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    Carta::Histogram::HistogramGenerator* m_histogram;

    //State specific to the data that is loaded.
    Carta::State::StateInterface m_stateData;
    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;
	Histogram( const Histogram& other);
	Histogram operator=( const Histogram& other );
};
}
}
