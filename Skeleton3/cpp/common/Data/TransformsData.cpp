#include "Data/TransformsData.h"
#include "Data/Util.h"

#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString TransformsData::DATA_TRANSFORMS = "dataTransforms";
const QString TransformsData::CLASS_NAME = "TransformsData";
const QString TransformsData::TRANSFORM_COUNT = "dataTransformCount";

class TransformsData::Factory : public CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        CartaObject * create (const QString & path, const QString & id)
        {
            return new TransformsData (path, id);
        }
    };


bool TransformsData::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME, new TransformsData::Factory());


TransformsData::TransformsData( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    _initializeDefaultState();
    _initializeCallbacks();
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
    m_transforms.push_back("None");
    m_transforms.push_back("x^.5");
    m_transforms.push_back("x^2");
    m_transforms.push_back("logarithm");

    int transformCount = m_transforms.size();
    m_state.insertValue<int>( TRANSFORM_COUNT, transformCount );
    m_state.insertArray( DATA_TRANSFORMS, transformCount );
    for ( int i = 0; i < transformCount; i++ ){
        QString arrayIndexStr = DATA_TRANSFORMS + StateInterface::DELIMITER + QString::number(i);
        m_state.setValue<QString>(arrayIndexStr, m_transforms[i]);
    }
    m_state.flushState();
}


void TransformsData::_initializeCallbacks(){
    addCommandCallback( "getTransformsData", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QStringList dataTransformList = getTransformsData();
        QString result = dataTransformList.join( ",");
        return result;
     });
}

bool TransformsData::isTransform( const QString& name ) const {
    int transformCount = m_transforms.size();
    bool validTransform = false;
    for ( int i = 0; i < transformCount; i++ ){
        if ( name == m_transforms[i] ){
           validTransform = true;
           break;
        }
    }
    return validTransform;
}



TransformsData::~TransformsData(){

}
}
}
