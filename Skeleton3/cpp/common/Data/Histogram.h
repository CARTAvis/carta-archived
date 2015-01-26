/***
 * Manages histogram settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "common/HistogramGenerator.h"
#include "Controller.h"

namespace Image {
class ImageInterface;
}

class ImageView;
namespace Carta {

namespace Data {

class Clips;
class IColoredView;
class Controller;

class Histogram : public QObject, public CartaObject {

    Q_OBJECT

public:

    virtual ~Histogram();
    const static QString CLASS_NAME;
    bool addViewObject( std::shared_ptr<IColoredView> target );

private slots:
    void  _generateHistogram();

private:
    //Set the state from commands.
    QString _setBinCount( const QString& params );
    QString _setGraphStyle( const QString& params );
    QString _setClipPercent( const QString& params );
    QString _setClipRange( const QString& params );
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
    /**
     * Note: this will eventually be rewritten into a useful method.
     */


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
    Histogram( const QString& path, const QString& id );
    class Factory;

    QList<std::shared_ptr<IColoredView> > m_histogramViews;

    //Data View
    std::shared_ptr<ImageView> m_view;

    static std::shared_ptr<Clips> m_clips;

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    StateInterface m_stateMouse;
	Histogram( const Histogram& other);
	Histogram operator=( const Histogram& other );
};
}
}
