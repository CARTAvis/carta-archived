#include "TransformsImage.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString TransformsImage::IMAGE_TRANSFORMS = "imageTransforms";
const QString TransformsImage::CLASS_NAME = "TransformsImage";
const QString TransformsImage::GAMMA = "Gamma";

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
    m_transforms.push_back( GAMMA );

    int transformCount = m_transforms.size();
    m_state.insertArray( IMAGE_TRANSFORMS, transformCount );
    for ( int i = 0; i < transformCount; i++ ){
        QString arrayIndexStr = Carta::State::UtilState::getLookup(IMAGE_TRANSFORMS, QString::number(i));
        m_state.setValue<QString>(arrayIndexStr, m_transforms[i]);
    }
    m_state.flushState();
}


QString TransformsImage::getDefault() const {
    return GAMMA;
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
