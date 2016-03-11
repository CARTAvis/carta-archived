#include "CurveData.h"
#include "Data/Util.h"
#include "Data/Plotter/LineStyles.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString CurveData::CLASS_NAME = "CurveData";
const QString CurveData::COLOR = "color";
const QString CurveData::STYLE = "style";


class CurveData::Factory : public Carta::State::CartaObjectFactory {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new CurveData (path, id);
    }
};

bool CurveData::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new CurveData::Factory());
LineStyles* CurveData::m_lineStyles = nullptr;

using Carta::State::UtilState;
using Carta::State::StateInterface;

CurveData::CurveData( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ){
    _initializeStatics();
    _initializeDefaultState();
}

QColor CurveData::getColor() const {
    int red = m_state.getValue<int>( Util::RED );
    int green = m_state.getValue<int>( Util::GREEN );
    int blue = m_state.getValue<int>( Util::BLUE );
    return QColor( red, green, blue );
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


void CurveData::_initializeDefaultState(){
    m_state.insertValue<QString>( Util::NAME, "");
    m_state.insertValue<int>( Util::RED, 255 );
    m_state.insertValue<int>( Util::GREEN, 0 );
    m_state.insertValue<int>( Util::BLUE, 0 );
    QString defaultLineStyle = m_lineStyles->getDefault();
    m_state.insertValue<QString>( STYLE, defaultLineStyle );
}


void CurveData::_initializeStatics(){
    if ( m_lineStyles == nullptr ){
       m_lineStyles = Util::findSingletonObject<LineStyles>();
    }
}


void CurveData::setColor( QColor color ){
    m_state.setValue<int>( Util::RED, color.red() );
    m_state.setValue<int>( Util::GREEN, color.green() );
    m_state.setValue<int>( Util::BLUE, color.blue() );
}


void CurveData::setData( const std::vector<double>& valsX, const std::vector<double>& valsY  ){
    CARTA_ASSERT( valsX.size() == valsY.size() );
    m_plotDataX = valsX;
    m_plotDataY = valsY;
}


QString CurveData::setLineStyle( const QString& lineStyle ){
    QString result;
    QString oldStyle = m_state.getValue<QString>( STYLE );
    QString actualStyle = m_lineStyles->getActualLineStyle( lineStyle );
    if ( !actualStyle.isEmpty() ){
        if ( actualStyle != oldStyle ){
            m_state.setValue<QString>( STYLE, actualStyle );
        }
    }
    else {
        result = "Unrecognized line style: " + lineStyle;
    }
    return result;
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
