#include "Data/Histogram.h"
#include "Data/Clips.h"
#include "Data/Util.h"
#include "Globals.h"
#include "ImageView.h"
#include "PluginManager.h"
#include "ErrorManager.h"
#include "CartaLib/Hooks/Histogram.h"
#include "Util.h"
#include <set>
#include "HistogramGenerator.h"
#include <QDebug>
#include <qwt_samples.h>
#include <qwt_plot_histogram.h>


namespace Carta {

namespace Data {

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
const QString Histogram::TEST_DATA = "testData";
const QString Histogram::DATA = "data";

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
    
   // _generateHistogram("Orion.methanol","/scratch/Images/Orion.methanol.cbc.contsub.image.fits");
}

void Histogram::_initializeDefaultState(){
    m_state.insertValue<int>(CLIP_INDEX, 0 );
    m_state.insertValue<double>( CLIP_MIN, 0 );
    m_state.insertValue<double>(CLIP_MAX, 1);
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
    m_state.insertValue<QString>(TEST_DATA, DATA);
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
    double oldMin = m_state.getValue<double>(CLIP_MIN);
    double oldMax = m_state.getValue<double>(CLIP_MAX);
    if ( validRangeMin && validRangeMax ){
        if ( clipMin <= clipMax ){
            bool changedState = false;
            if ( clipMin != oldMin){
                m_state.setValue<double>(CLIP_MIN, clipMin );
                changedState = true;
            }
            if ( clipMax != oldMax ){
                m_state.setValue<double>(CLIP_MAX, clipMax );
                changedState = true;
            }
            if ( changedState ){
                m_state.flushState();
            }
        }
        else {
            result = "The histogram clip minimum must be less than the maximum: "+params;
        }
    }
    else {
        result = "Invalid Histogram clip range parameters: "+ params;
    }
    QString origValues = QString::number(oldMin) +","+QString::number(oldMax);
    result = Util::commandPostProcess( result, origValues );
    return result;
}

QString Histogram::_setColored( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_COLORED};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString coloredStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool colored = Util::toBool( coloredStr, &validBool );
    bool oldColored = m_state.getValue<bool>(GRAPH_COLORED);
    if ( validBool ){
        if ( colored != oldColored){
            m_state.setValue<bool>(GRAPH_COLORED, colored );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid Histogram colored parameter; must be a bool: "+ params;
    }
    result = Util::commandPostProcess( result, Util::toString( oldColored ));
    return result;
}

QString Histogram::_setBinCount( const QString& params ){
    QString result;
    std::set<QString> keys = {BIN_COUNT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString binCountStr = dataValues[*keys.begin()];
    bool validInt = false;
    int binCount = binCountStr.toInt( &validInt );
    int oldBinCount = m_state.getValue<int>(BIN_COUNT );
    if ( validInt ){
        if ( binCount != oldBinCount ){
           m_state.setValue<int>(BIN_COUNT, binCount );
           m_state.flushState();
           qDebug()<<"ploting with binCount = "<<binCount;
           _generateHistogram("Orion.methanol","/scratch/Images/Orion.methanol.cbc.contsub.image.fits");

        }

    }
    else {
        result = "Invalid Histogram bin count parameters: "+ params;
    }
    result = Util::commandPostProcess( result, QString::number( oldBinCount) );
    return result;
}


QString Histogram::_setLogCount( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_LOG_COUNT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString logCountStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool logCount = Util::toBool( logCountStr, &validBool );
    bool oldLogCount = m_state.getValue<bool>(GRAPH_LOG_COUNT);
    if ( validBool ){
        if ( logCount != oldLogCount ){
           m_state.setValue<bool>(GRAPH_LOG_COUNT, logCount );
           m_state.flushState();
        }
    }
    else {
        result = "Invalid Histogram log count must be a bool: "+ params;
    }
    result = Util::commandPostProcess( result, Util::toString( oldLogCount ));
    return result;
}

QString Histogram::_setPlaneMode( const QString& params ){
    QString result;
    std::set<QString> keys = {PLANE_MODE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString planeModeStr = dataValues[*keys.begin()];
    QString oldPlaneMode = m_state.getValue<QString>(PLANE_MODE);
    if ( planeModeStr == PLANE_MODE_ALL || planeModeStr == PLANE_MODE_SINGLE ||
            planeModeStr == PLANE_MODE_RANGE ){
        if ( planeModeStr != oldPlaneMode ){
           m_state.setValue<QString>(PLANE_MODE, planeModeStr );
           m_state.flushState();
        }
    }
    else {
        result = "Unrecognized histogram cube mode params: "+ params;
    }
    result = Util::commandPostProcess( result, oldPlaneMode );
    return result;
}

QString Histogram::_setPlaneSingle( const QString& params ){
    QString result;
    std::set<QString> keys = {PLANE_SINGLE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString planeStr = dataValues[*keys.begin()];
    bool validInt = false;
    int plane = planeStr.toInt( &validInt );
    int oldPlane = m_state.getValue<int>(PLANE_SINGLE);
    if ( validInt ){
        if ( plane != oldPlane ){
           m_state.setValue<int>(PLANE_SINGLE, plane );
           m_state.flushState();
        }
    }
    else {
        result = "Invalid Histogram single plane: "+ params;
    }
    result = Util::commandPostProcess( result, QString::number(oldPlane) );
    return result;
}

QString Histogram::_set2DFootPrint( const QString& params ){
    QString result;
    std::set<QString> keys = {FOOT_PRINT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString footPrintStr = dataValues[*keys.begin()];
    QString oldFootPrint = m_state.getValue<QString>(FOOT_PRINT);
    if ( footPrintStr == FOOT_PRINT_IMAGE || footPrintStr == FOOT_PRINT_REGION ||
            footPrintStr == FOOT_PRINT_REGION_ALL ){
        if ( footPrintStr != oldFootPrint){
            m_state.setValue<QString>(FOOT_PRINT, footPrintStr );
            m_state.flushState();
        }
    }
    else {
        result = "Unsupported Histogram footprint: "+ params;
    }
    result = Util::commandPostProcess( result, oldFootPrint );
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
    int storedMin = m_state.getValue<int>(PLANE_MIN);
    int storedMax = m_state.getValue<int>(PLANE_MAX);
    if ( validRangeMin && validRangeMax ){
        if ( planeMin <= planeMax ){
            bool changedState = false;
            if ( planeMin != storedMin){
                m_state.setValue<int>(PLANE_MIN, planeMin );
                changedState = true;
            }

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
        }
    }
    else {
        result = "Invalid Histogram plane range parameters: "+ params;
    }
    QString oldVals = QString::number(storedMin)+","+QString::number( storedMax);
    result = Util::commandPostProcess( result, oldVals );
    return result;
}

QString Histogram::_setGraphStyle( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_STYLE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString styleStr = dataValues[*keys.begin()];
    QString oldStyle = m_state.getValue<QString>(GRAPH_STYLE);
    if ( styleStr == GRAPH_STYLE_LINE || styleStr == GRAPH_STYLE_OUTLINE ||
            styleStr == GRAPH_STYLE_FILL ){
        if ( styleStr != oldStyle ){
            m_state.setValue<QString>(GRAPH_STYLE, styleStr );
            m_state.flushState();
        }
    }
    else {
        result = "Unrecognized histogram graph style: "+ params;
    }
    result = Util::commandPostProcess( result, oldStyle );
    return result;
}

QString Histogram::_setClipPercent( const QString& params ){
    QString result;
    std::set<QString> keys = {"clipPercent"};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipStr = dataValues[*keys.begin()];
    int oldClipIndex = m_state.getValue<int>(CLIP_INDEX);
    if ( m_clips != nullptr ){
       int index = m_clips->getIndex( clipStr );
       if ( index >= 0 ){
           if ( index != oldClipIndex ){
              m_state.setValue<int>(CLIP_INDEX, index );
              m_state.flushState();
           }
        }
       else {
           result = "Invalid clip percent: " + clipStr;
       }
    }
    else {
        result = "Invalid clip params: "+params;
    }
    result = Util::commandPostProcess( result, QString::number(oldClipIndex ));
    return result;
}

QString Histogram::_setClipToImage( const QString& params ){
    QString result;
    std::set<QString> keys = {CLIP_APPLY};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipApplyStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool clipApply = Util::toBool( clipApplyStr, &validBool );
    bool oldClipApply = m_state.getValue<bool>(CLIP_APPLY);
    if ( validBool ){
       if ( clipApply != oldClipApply ){
          m_state.setValue<bool>(CLIP_APPLY, clipApply );
          m_state.flushState();
       }
    }
    else {
       result = "Invalid apply clip to image; must be true/false: " + clipApplyStr;
    }
    result = Util::commandPostProcess( result, Util::toString( oldClipApply ));
    return result;
}



void Histogram::_generateHistogram(QString filename, QString filepath){

    int binCount = m_state.getValue<int>(BIN_COUNT);
    int minChannel = m_state.getValue<int>(PLANE_MIN);
    int maxChannel = m_state.getValue<int>(PLANE_MAX);
    int spectralIndex = m_state.getValue<int>(PLANE_SINGLE);
    double minIntensity = m_state.getValue<double>(CLIP_MIN);
    double maxIntensity = m_state.getValue<double>(CLIP_MAX);

    /*qDebug() << "binCount = "<<_binCount;
    qDebug() << "minchannel = "<<_minChannel;
    qDebug() << "maxchannel = "<<_maxChannel;
    qDebug() << "spectralIndex = "<<_spectralIndex;
    qDebug() << "minIntensity = "<<_minIntensity;
    qDebug() << "maxIntensity = "<<_maxIntensity;*/

    auto result = Globals::instance()-> pluginManager()
                          -> prepare <Carta::Lib::Hooks::HistogramHook>(filepath, binCount,
                            minChannel, maxChannel, spectralIndex, minIntensity, maxIntensity);
    auto lam = [=] ( const Carta::Lib::Hooks::HistogramHook::ResultType &data ) {
            int dataCount = data.size();
            QVector<QwtIntervalSample> samples(0);

            for ( int i = 0; i < dataCount-1; i++ ){
               // qDebug() << "x = "<<data[i].first<<"y = "<< data[i].second;
                QwtIntervalSample sample(data[i].second, data[i].first, data[i+1].first);
                samples.append(sample);
            }

            HistogramGenerator * histogram = new HistogramGenerator(QString(filename));
            histogram->setStyle();
            histogram->setData(samples);
            QImage * histogramImage = histogram->toImage();
            //histogramImage.save("/scratch/Images/test2.jpg");
            m_view->resetImage(*histogramImage);
            refreshView(m_view.get());
           

        };
    try {
        result.forEach( lam );
    }
    catch( char*& error ){
        QString errorStr( error );
        ErrorManager* hr = dynamic_cast<ErrorManager*>(Util::findSingletonObject( ErrorManager::CLASS_NAME ));
        hr->registerError( errorStr );
    }
}

Histogram::~Histogram(){

}
}
}
