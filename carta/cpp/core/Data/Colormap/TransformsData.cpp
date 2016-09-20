#include "TransformsData.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString TransformsData::DATA_TRANSFORMS = "dataTransforms";
const QString TransformsData::CLASS_NAME = "TransformsData";
const QString TransformsData::TRANSFORM_NONE = "Linear";
const QString TransformsData::TRANSFORM_ROOT = "Square Root";
const QString TransformsData::TRANSFORM_SQUARE = "Square";
const QString TransformsData::TRANSFORM_LOG = "Logarithm";
const QString TransformsData::TRANSFORM_POLY = "Polynomial";

class TransformsData::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new TransformsData (path, id);
        }
    };


bool TransformsData::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new TransformsData::Factory());


TransformsData::TransformsData( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    _initializeDefaultState();
}

QString TransformsData::getDefault() const {
    return TRANSFORM_NONE;
}


QStringList TransformsData::getTransformsData() const {
    QStringList buff;
    int transformCount = m_transforms.size();
    for ( int i = 0; i < transformCount; i++ ){
        buff << m_transforms[i];
    }
    return buff;
}

void TransformsData::_initializeDefaultState(){

    // get all TransformsData provided by core
    //hard-code the possible transforms until Pavol's code is available.
    m_transforms.push_back(TRANSFORM_NONE);
    m_transforms.push_back(TRANSFORM_ROOT);
    m_transforms.push_back(TRANSFORM_SQUARE);
    m_transforms.push_back(TRANSFORM_LOG);
    m_transforms.push_back(TRANSFORM_POLY );

    int transformCount = m_transforms.size();
    m_state.insertArray( DATA_TRANSFORMS, transformCount );
    for ( int i = 0; i < transformCount; i++ ){
        QString arrayIndexStr = Carta::State::UtilState::getLookup(DATA_TRANSFORMS, QString::number(i));
        m_state.setValue<QString>(arrayIndexStr, m_transforms[i]);
    }
    m_state.flushState();
}


bool TransformsData::isTransform( const QString& name, QString& actualName ) const {
    int transformCount = m_transforms.size();
    bool validTransform = false;
    for ( int i = 0; i < transformCount; i++ ){
        int result = QString::compare( name, m_transforms[i], Qt::CaseInsensitive );
        if ( result == 0 ){
           actualName = m_transforms[i];
           validTransform = true;
           break;
        }
    }
    return validTransform;
}

Carta::Lib::PixelPipeline::ScaleType TransformsData::getScaleType( const QString& name ) const {
    Carta::Lib::PixelPipeline::ScaleType scaleType = Carta::Lib::PixelPipeline::ScaleType::Linear;
    if ( name == TRANSFORM_LOG ){
        scaleType = Carta::Lib::PixelPipeline::ScaleType::Log;
    }
    else if ( name == TRANSFORM_POLY ){
        scaleType = Carta::Lib::PixelPipeline::ScaleType::Polynomial;
    }
    else if ( name == TRANSFORM_SQUARE ){
        scaleType = Carta::Lib::PixelPipeline::ScaleType::Sqr;
    }
    else if ( name == TRANSFORM_ROOT ){
        scaleType = Carta::Lib::PixelPipeline::ScaleType::Sqrt;
    }
    return scaleType;
}


TransformsData::~TransformsData(){

}
}
}
