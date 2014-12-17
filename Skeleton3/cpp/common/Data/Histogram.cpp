#include "Data/Histogram.h"
#include "Data/Clips.h"
#include "Data/Util.h"
#include "Globals.h"
#include "ImageView.h"
#include <set>

#include <QDebug>


const QString Histogram::CLASS_NAME = "Histogram";

const QString Histogram::CLIP_INDEX = "clipIndex";
const QString Histogram::CLIP_MIN = "clipMin";
const QString Histogram::CLIP_MAX = "clipMax";
const QString Histogram::CLIP_APPLY = "applyClipToImage";
const QString Histogram::BIN_COUNT = "binCount";
const QString Histogram::GRAPH_STYLE = "graphStyle";
const QString Histogram::GRAPH_STYLE_LINE = "Line";
const QString Histogram::GRAPH_STYLE_OUTLINE = "Outline";
const QString Histogram::GRAPH_STYLE_FILL = "Fill";
const QString Histogram::GRAPH_LOG_COUNT = "logCount";
const QString Histogram::GRAPH_COLORED = "colored";
const QString Histogram::PLANE_SINGLE = "selectedPlane";
const QString Histogram::PLANE_MODE="planeMode";
const QString Histogram::PLANE_MODE_SINGLE="Single";
const QString Histogram::PLANE_MODE_RANGE="Range";
const QString Histogram::PLANE_MODE_ALL="All";
const QString Histogram::PLANE_MIN = "planeMin";
const QString Histogram::PLANE_MAX = "planeMax";
const QString Histogram::FOOT_PRINT = "twoDFootPrint";
const QString Histogram::FOOT_PRINT_IMAGE = "Image";
const QString Histogram::FOOT_PRINT_REGION = "Selected Region";
const QString Histogram::FOOT_PRINT_REGION_ALL = "All Regions";

std::shared_ptr<Clips>  Histogram::m_clips = nullptr;

class Histogram::Factory : public CartaObjectFactory {
    public:

        CartaObject * create (const QString & path, const QString & id)
        {
            return new Histogram (path, id);
        }
    };

bool Histogram::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Histogram::Factory());



Histogram::Histogram( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_view(nullptr),
    m_stateMouse(path + StateInterface::DELIMITER+ImageView::VIEW){
    _initializeDefaultState();
    _initializeCallbacks();
    m_view.reset( new ImageView( path, QColor("yellow"), QImage(), &m_stateMouse));
    registerView(m_view.get());

    //Load the available clips.
    if ( m_clips == nullptr ){
        CartaObject* obj = Util::findSingletonObject( Clips::CLASS_NAME );
        m_clips.reset(dynamic_cast<Clips*>(obj));
    }
}

void Histogram::_initializeDefaultState(){
    m_state.insertValue<int>(CLIP_INDEX, 0 );
    m_state.insertValue<double>( CLIP_MIN, 0 );
    m_state.insertValue<double>(CLIP_MAX, 0);
    m_state.insertValue<bool>(CLIP_APPLY, false );
    m_state.insertValue<int>(BIN_COUNT, 25 );
    m_state.insertValue<QString>(GRAPH_STYLE, GRAPH_STYLE_LINE);
    m_state.insertValue<bool>(GRAPH_LOG_COUNT, false );
    m_state.insertValue<bool>(GRAPH_COLORED, true );
    m_state.insertValue<QString>(PLANE_MODE, PLANE_MODE_SINGLE );
    m_state.insertValue<int>(PLANE_SINGLE, 0 );
    m_state.insertValue<int>(PLANE_MIN, 0 );
    m_state.insertValue<int>(PLANE_MAX, 1 );
    m_state.insertValue<QString>(FOOT_PRINT, FOOT_PRINT_IMAGE );
    m_state.flushState();

    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.flushState();
}

void Histogram::_initializeCallbacks(){
    addCommandCallback( "setBinCount", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QString result = _setBinCount( params );
            return result;
        });

    addCommandCallback( "setClipPercent", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QString result = _setClipPercent( params );
            return result;
        });

    addCommandCallback( "setClipRange", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                QString result = _setClipRange( params );
                return result;
            });

    addCommandCallback( "setClipToImage", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result = _setClipToImage( params );
                    return result;
                });

    addCommandCallback( "setGraphStyle", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result = _setGraphStyle( params );
                    return result;
                });

    addCommandCallback( "setLogCount", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
                        QString result = _setLogCount( params );
                        return result;
                    });

    addCommandCallback( "setColored", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
                            QString result = _setColored( params );
                            return result;
                        });

    addCommandCallback( "setPlaneMode", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
                                QString result = _setPlaneMode( params );
                                return result;
                            });

    addCommandCallback( "setPlaneSingle", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
                                QString result = _setPlaneSingle( params );
                                return result;
                            });

    addCommandCallback( "setPlaneRange", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
                                QString result = _setPlaneRange( params );
                                return result;
                            });

    addCommandCallback( "set2DFootPrint", [=] (const QString & /*cmd*/,
                                            const QString & params, const QString & /*sessionId*/) -> QString {
                                    QString result = _set2DFootPrint( params );
                                    return result;
                                });

    addCommandCallback( "zoomFull", [=] (const QString & /*cmd*/,
                                            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                                    qDebug() << "Need to implement histogram zoom full";
                                    return "";
                                });

    addCommandCallback( "zoomRange", [=] (const QString & /*cmd*/,
                                                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                                        qDebug() << "Need to implement histogram zoom range";
                                        return "";
                                    });

}

QString Histogram::_setClipRange( const QString& params ){
    QString result;
    std::set<QString> keys = {CLIP_MIN, CLIP_MAX};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipMinStr = dataValues[CLIP_MIN];
    QString clipMaxStr = dataValues[CLIP_MAX];
    bool validRangeMin = false;
    double clipMin = clipMinStr.toDouble( &validRangeMin );
    bool validRangeMax = false;
    double clipMax = clipMaxStr.toDouble( &validRangeMax );
    if ( validRangeMin && validRangeMax ){
        if ( clipMin <= clipMax ){
            bool changedState = false;
            if ( clipMin != m_state.getValue<double>(CLIP_MIN)){
                m_state.setValue<double>(CLIP_MIN, clipMin );
                changedState = true;
            }
            if ( clipMax != m_state.getValue<double>(CLIP_MAX)){
                m_state.setValue<double>(CLIP_MAX, clipMax );
                changedState = true;
            }
            if ( changedState ){
                m_state.flushState();
            }
        }
        else {
            result = "The histogram clip minimum must be less than the maximum: "+params;
            qWarning() << result;
        }
    }
    else {
        result = "Invalid Histogram clip range parameters: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Histogram::_setColored( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_COLORED};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString coloredStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool colored = Util::toBool( coloredStr, &validBool );
    if ( validBool ){
        if ( colored != m_state.getValue<bool>(GRAPH_COLORED)){
            m_state.setValue<bool>(GRAPH_COLORED, colored );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid Histogram colored parameter; must be a bool: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Histogram::_setBinCount( const QString& params ){
    QString result;
    std::set<QString> keys = {BIN_COUNT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString binCountStr = dataValues[*keys.begin()];
    bool validInt = false;
    int binCount = binCountStr.toInt( &validInt );
    if ( validInt ){
        int oldBinCount = m_state.getValue<int>(BIN_COUNT );
        if ( binCount != oldBinCount ){
            m_state.setValue<int>(BIN_COUNT, binCount );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid Histogram bin count parameters: "+ params;
        qWarning() << result;
    }
    return result;
}


QString Histogram::_setLogCount( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_LOG_COUNT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString logCountStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool logCount = Util::toBool( logCountStr, &validBool );
    if ( validBool ){
        if ( logCount != m_state.getValue<bool>(GRAPH_LOG_COUNT)){
            m_state.setValue<bool>(GRAPH_LOG_COUNT, logCount );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid Histogram log count must be a bool: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Histogram::_setPlaneMode( const QString& params ){
    QString result;
    std::set<QString> keys = {PLANE_MODE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString planeModeStr = dataValues[*keys.begin()];
    if ( planeModeStr == PLANE_MODE_ALL || planeModeStr == PLANE_MODE_SINGLE ||
            planeModeStr == PLANE_MODE_RANGE ){
        if ( planeModeStr != m_state.getValue<QString>(PLANE_MODE)){
            m_state.setValue<QString>(PLANE_MODE, planeModeStr );
            m_state.flushState();
        }
    }
    else {
        result = "Unrecognized histogram cube mode params: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Histogram::_setPlaneSingle( const QString& params ){
    QString result;
    std::set<QString> keys = {PLANE_SINGLE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString planeStr = dataValues[*keys.begin()];
    bool validInt = false;
    int plane = planeStr.toInt( &validInt );
    if ( validInt ){
        if ( plane != m_state.getValue<int>(PLANE_SINGLE)){
            m_state.setValue<int>(PLANE_SINGLE, plane );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid Histogram single plane: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Histogram::_set2DFootPrint( const QString& params ){
    QString result;
    std::set<QString> keys = {FOOT_PRINT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString footPrintStr = dataValues[*keys.begin()];
    if ( footPrintStr == FOOT_PRINT_IMAGE || footPrintStr == FOOT_PRINT_REGION ||
            footPrintStr == FOOT_PRINT_REGION_ALL ){
        if ( footPrintStr != m_state.getValue<QString>(FOOT_PRINT)){
            m_state.setValue<QString>(FOOT_PRINT, footPrintStr );
            m_state.flushState();
        }
    }
    else {
        result = "Unsupported Histogram footprint: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Histogram::_setPlaneRange( const QString& params ){
    QString result;
    std::set<QString> keys = {PLANE_MIN, PLANE_MAX};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString planeMinStr = dataValues[PLANE_MIN];
    QString planeMaxStr = dataValues[PLANE_MAX];
    bool validRangeMin = false;
    int planeMin = planeMinStr.toInt( &validRangeMin );
    bool validRangeMax = false;
    int planeMax = planeMaxStr.toInt( &validRangeMax );
    if ( validRangeMin && validRangeMax ){
        if ( planeMin <= planeMax ){
            bool changedState = false;
            int storedMin = m_state.getValue<int>(PLANE_MIN);
            if ( planeMin != storedMin){
                m_state.setValue<int>(PLANE_MIN, planeMin );
                changedState = true;
            }
            int storedMax = m_state.getValue<int>(PLANE_MAX);
            if ( planeMax != storedMax){
                m_state.setValue<int>(PLANE_MAX, planeMax );
                changedState = true;
            }
            if ( changedState ){
                m_state.flushState();
            }
        }
        else {
            result = "The histogram plane range minimum must be less than the maximum: "+params;
            qWarning() << result;
        }
    }
    else {
        result = "Invalid Histogram plane range parameters: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Histogram::_setGraphStyle( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_STYLE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString styleStr = dataValues[*keys.begin()];
    if ( styleStr == GRAPH_STYLE_LINE || styleStr == GRAPH_STYLE_OUTLINE ||
            styleStr == GRAPH_STYLE_FILL ){
        if ( styleStr != m_state.getValue<QString>(GRAPH_STYLE)){
            m_state.setValue<QString>(GRAPH_STYLE, styleStr );
            m_state.flushState();
        }
    }
    else {
        result = "Unrecognized histogram graph style: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Histogram::_setClipPercent( const QString& params ){
    QString result;
    std::set<QString> keys = {"clipPercent"};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipStr = dataValues[*keys.begin()];
    if ( m_clips != nullptr ){
       int index = m_clips->getIndex( clipStr );
       if ( index >= 0 ){
           if ( index != m_state.getValue<int>(CLIP_INDEX) ){
              m_state.setValue<int>(CLIP_INDEX, index );
              m_state.flushState();
           }
        }
       else {
           result = "Invalid clip percent: " + clipStr;
           qWarning() << result;
       }
    }
    return result;
}

QString Histogram::_setClipToImage( const QString& params ){
    QString result;
    std::set<QString> keys = {CLIP_APPLY};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipApplyStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool clipApply = Util::toBool( clipApplyStr, &validBool );
    if ( validBool ){
       if ( clipApply != m_state.getValue<bool>(CLIP_APPLY) ){
          m_state.setValue<bool>(CLIP_APPLY, clipApply );
          m_state.flushState();
       }
       else {
          result = "Invalid apply clip to image; must be true/false: " + clipApplyStr;
          qWarning() << result;
       }
    }
    return result;
}

Histogram::~Histogram(){

}
