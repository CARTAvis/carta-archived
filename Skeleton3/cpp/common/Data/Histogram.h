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

class Clips;
class Colormap;
class Controller;
class LinkableImpl;

class Histogram : public QObject, public CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    bool addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    bool removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;

    /**
     * Clear the state of the histogram.
     */
    void clear();

    void resetImage();

    /**
     * Send a new state update to the client.
     */
    void refreshState();

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
     * Set whether or not to apply histogram clips to linked images.
     * @param clipApply true if histogram clip settings should be applied to linked images; false
     *          otherwise.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QString setClipToImage( bool clipApply );

    /**
     * Set the clip min and max of the histogram.
     * @param clipMin the minimum intensity.
     * @param clipMax the maximum intensity.
     * @param link the server-side id of the controller whose data was used to generate the histogram.
     * @return an error message if there was a problem setting the range; an empty QString otherwise.
     */
    //QString setClipRange( double clipMin, double clipMax, const QString& link );

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
     * @param minPlane the minimum channel to include or -1 if there is no minimum.
     * @param maxPlane the maximum channel to include or -1 if there is no maximum
     * @return an error message if there was a problem setting the frame range; an empty string otherwise.
     */
    QString setPlaneRange( int minPlane, int maxPlane );

    /**
     * Set the largest plane value that the user should be allowed to set in the GUI.
     * @param bound an upper bound for the planes that can be included in the histogram.
     * @return an error message if there was a problem setting the plane range upper bound;
     *          an empty string otherwise.
     */
    QString setPlaneRangeUpperBound( int bound );



    QString setRangeColor( double colorMin, double colorMax );
    QString setColorMin( double colorMin, bool finish );
    QString setColorMax( double colorMax, bool finish );

    virtual ~Histogram();
    const static QString CLASS_NAME;
    const static QString GRAPH_STYLE_LINE;
    const static QString GRAPH_STYLE_OUTLINE;
    const static QString GRAPH_STYLE_FILL;

private slots:
    void  _generateHistogram( bool newDataNeeded, Controller* controller=nullptr);
    void _createHistogram( Controller* );
    void _updateColorMap( Colormap* );
    void _updateSize( const QSize& size );
    void _updateChannel( Controller* controller );
    

private:
    void _applyClips() const;

    void _finishClips();

    double _getBufferedIntensity( const QString& clipKey, const QString& percentKey );
    std::pair<int,int> _getFrameBounds() const;
    double _getPercentile( const QString& fileName, int frameIndex, double intensity ) const;
    bool _getIntensity( const QString& fileName, int frameIndex, double percentile, double* intensity ) const;
    Controller* _getControllerSelected() const;
    void _loadData( Controller* controller);

    QString _set2DFootPrint( const QString& params );

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
    const static QString BIN_COUNT;
    const static QString BIN_WIDTH;
    const static QString COLOR_MIN;
    const static QString COLOR_MAX;
    const static QString COLOR_MIN_PERCENT;
    const static QString COLOR_MAX_PERCENT;
    const static QString GRAPH_STYLE;

    const static QString GRAPH_LOG_COUNT;
    const static QString GRAPH_COLORED;
    const static QString PLANE_MODE;
    const static QString PLANE_MODE_SINGLE;
    const static QString PLANE_MODE_RANGE;
    const static QString PLANE_MODE_ALL;
    const static QString PLANE_MIN;
    const static QString PLANE_MAX;
    const static QString PLANE_RANGE_UPPER_BOUND;
    const static QString FOOT_PRINT;
    const static QString FOOT_PRINT_IMAGE;
    const static QString FOOT_PRINT_REGION;
    const static QString FOOT_PRINT_REGION_ALL;
    const static QString CLIP_MIN_PERCENT;
    const static QString CLIP_MAX_PERCENT;
    const static QString LINK;
    const static QString X_COORDINATE;
    const static QString POINTER_MOVE;
    
    int m_significantDigits;
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
    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    StateInterface m_stateMouse;
	Histogram( const Histogram& other);
	Histogram operator=( const Histogram& other );
};
}
}
