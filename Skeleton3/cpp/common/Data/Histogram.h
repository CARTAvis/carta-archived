/***
 * Manages histogram settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "Data/LinkableImpl.h"

#include <QObject>

namespace Image {
class ImageInterface;
}

class ImageView;
namespace Carta {

namespace Data {

class Clips;
class Controller;

class Histogram : public QObject, public CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    bool addLink( const std::shared_ptr<Controller> & controller) Q_DECL_OVERRIDE;
    bool removeLink( const std::shared_ptr<Controller>& controller) Q_DECL_OVERRIDE;

    /**
     * Clear the state of the histogram.
     */
    void clear();

    /**
     * Set the clip min and max percent of the histogram.
     * @param clipMinPercent percentage to clip from the left.
     * @param clipMaxPercent percentage to clip from the right.
     * @param link the server-side id of the controller whose data was used to generate the histogram.
     * @return an error message if there was a problem setting the percentages; an empty QString otherwise.
     */
    QString setClipPercent( double clipMinPercent, double clipMaxPercent, const QString& link );

    /**
     * Set the clip min and max of the histogram.
     * @param clipMin the minimum intensity.
     * @param clipMax the maximum intensity.
     * @param link the server-side id of the controller whose data was used to generate the histogram.
     * @return an error message if there was a problem setting the range; an empty QString otherwise.
     */
    QString setClipRange( double clipMin, double clipMax, const QString& link );


    virtual ~Histogram();
    const static QString CLASS_NAME;

private slots:
    void  _generateHistogram();
    void _createHistogram();

private:
    NdArray::RawViewInterface* _findRawData( const QString& fileName, int frameIndex ) const;
    double _getPercentile( const QString& fileName, int frameIndex, double intensity ) const;
    bool _getIntensity( const QString& fileName, int frameIndex, double percentile, double* intensity ) const;
    int _getLinkInfo( const QString& link, QString& name ) const;
    //Set the state from commands.
    QString _setBinCount( const QString& params );
    QString _setGraphStyle( const QString& params );
<<<<<<< HEAD
    QString _setClipMin( const QString& params );
    QString _setClipMax( const QString& params );
    QString _setClipPercent( const QString& params );
    // QString _setClipMaxPercent( const QString& params );
    // QString _setClipMinPercent( const QString& params );
=======


>>>>>>> dd4500aa8fa1392af59a7ccfd13a56ca2591d5c1
    QString _setClipToImage( const QString& params );
    QString _setColored( const QString& params );
    QString _setLogCount( const QString& params );
    QString _setPlaneMode( const QString& params );
    QString _setPlaneSingle( const QString& params );
    QString _setPlaneRange( const QString& params );
    QString _set2DFootPrint( const QString& params );
    std::vector<std::shared_ptr<Image::ImageInterface>> _generateData();
    
    void _initializeDefaultState();
    void _initializeCallbacks();

    static bool m_registered;

    const static QString CLIP_INDEX;
    const static QString CLIP_MIN;
    const static QString CLIP_MAX;
    const static QString CLIP_APPLY;
    const static QString BIN_COUNT;
    const static QString GRAPH_STYLE;
    const static QString GRAPH_STYLE_LINE;
    const static QString GRAPH_STYLE_OUTLINE;
    const static QString GRAPH_STYLE_FILL;
    const static QString GRAPH_LOG_COUNT;
    const static QString GRAPH_COLORED;
    const static QString PLANE_SINGLE;
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
    const static QString LINK;
    
    const static double CLIP_ERROR_MARGIN;

    Histogram( const QString& path, const QString& id );
    class Factory;

    //Data View
    std::shared_ptr<ImageView> m_view;

    static std::shared_ptr<Clips> m_clips;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    StateInterface m_stateMouse;
	Histogram( const Histogram& other);
	Histogram operator=( const Histogram& other );
};
}
}
