#include "CurveData.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString CurveData::CLASS_NAME = "CurveData";


class CurveData::Factory : public Carta::State::CartaObjectFactory {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new CurveData (path, id);
    }
};

bool CurveData::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new CurveData::Factory());


using Carta::State::UtilState;
using Carta::State::StateInterface;

CurveData::CurveData( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
    _initializeCallbacks();
}

QString CurveData::getName() const {
    return m_state.getValue<QString>( Util::NAME );
}


std::shared_ptr<Carta::Lib::Image::ImageInterface> CurveData::getSource() const {
    return m_imageSource;
}

QString CurveData::getStateString() const{
    return m_state.toString();
}

std::vector<double> CurveData::getValuesX() const {
    return m_plotDataX;
}

std::vector<double> CurveData::getValuesY() const {
    return m_plotDataY;
}

void CurveData::_initializeCallbacks(){

}

void CurveData::_initializeDefaultState(){
    m_state.insertValue<QString>( Util::NAME, "");
    m_state.flushState();
}


void CurveData::setData( const std::vector<double>& valsX, const std::vector<double>& valsY  ){
    CARTA_ASSERT( valsX.size() == valsY.size() );
    m_plotDataX = valsX;
    m_plotDataY = valsY;
}


void CurveData::setName( const QString& curveName ){
    QString oldName = m_state.getValue<QString>( Util::NAME );
    if ( oldName != curveName ){
        m_state.setValue<QString>( Util::NAME, curveName );
    }
}


void CurveData::setSource( std::shared_ptr<Carta::Lib::Image::ImageInterface> imageSource ){
    m_imageSource = imageSource;
}


CurveData::~CurveData(){
}
}
}
