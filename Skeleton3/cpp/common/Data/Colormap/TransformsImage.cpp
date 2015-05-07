#include "TransformsImage.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString TransformsImage::IMAGE_TRANSFORMS = "imageTransforms";
const QString TransformsImage::CLASS_NAME = "TransformsImage";
const QString TransformsImage::TRANSFORM_COUNT = "imageTransformCount";

class TransformsImage::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new TransformsImage (path, id);
        }
    };


bool TransformsImage::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new TransformsImage::Factory());


TransformsImage::TransformsImage( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    _initializeDefaultState();
    _initializeCallbacks();
}


QStringList TransformsImage::getTransformsImage() const {
    QStringList buff;
    int transformCount = m_transforms.size();
    for ( int i = 0; i < transformCount; i++ ){
        buff << m_transforms[i];
    }
    return buff;
}

void TransformsImage::_initializeDefaultState(){

    // get all TransformsImage provided by core
    //hard-code the possible transforms until Pavol's code is available.
    m_transforms.push_back("Gamma");

    int transformCount = m_transforms.size();
    m_state.insertValue<int>( TRANSFORM_COUNT, transformCount );
    m_state.insertArray( IMAGE_TRANSFORMS, transformCount );
    for ( int i = 0; i < transformCount; i++ ){
        QString arrayIndexStr = Carta::State::UtilState::getLookup(IMAGE_TRANSFORMS, QString::number(i));
        m_state.setValue<QString>(arrayIndexStr, m_transforms[i]);
    }
    m_state.flushState();
}


void TransformsImage::_initializeCallbacks(){
    addCommandCallback( "getTransformsImage", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QStringList dataTransformList = getTransformsImage();
        QString result = dataTransformList.join( ",");
        return result;
     });
}

bool TransformsImage::isTransform( const QString& name ) const {
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



TransformsImage::~TransformsImage(){

}
}
}
