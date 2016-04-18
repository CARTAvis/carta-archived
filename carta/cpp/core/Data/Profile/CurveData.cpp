#include "CurveData.h"
#include "Data/Util.h"
#include "Data/Plotter/LineStyles.h"
#include "Data/Profile/ProfileStatistics.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString CurveData::CLASS_NAME = "CurveData";
const QString CurveData::COLOR = "color";
const QString CurveData::STYLE = "style";
const QString CurveData::STATISTIC = "stat";
const QString CurveData::REGION_NAME = "region";
const QString CurveData::IMAGE_NAME = "image";
const QString CurveData::REST_FREQUENCY = "restFrequency";


class CurveData::Factory : public Carta::State::CartaObjectFactory {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new CurveData (path, id);
    }
};

bool CurveData::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new CurveData::Factory());
LineStyles* CurveData::m_lineStyles = nullptr;
ProfileStatistics* CurveData::m_stats = nullptr;

using Carta::State::UtilState;
using Carta::State::StateInterface;

CurveData::CurveData( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ){
    _initializeStatics();
    _initializeDefaultState();
}

void CurveData::copy( const std::shared_ptr<CurveData> & other ){
    if ( other ){
        m_plotDataX = other->m_plotDataX;
        m_plotDataY = other->m_plotDataY;
        m_region = other->m_region;
        m_imageSource = other->m_imageSource;

        m_state.setValue<QString>( Util::NAME, other->getName());
        QColor otherColor = other->getColor();
        m_state.setValue<int>( Util::RED, otherColor.red() );
        m_state.setValue<int>( Util::GREEN, otherColor.green() );
        m_state.setValue<int>( Util::BLUE, otherColor.blue() );
        m_state.setValue<QString>( STYLE, other->getLineStyle() );
        m_state.setValue<QString>( STATISTIC, other->getStatistic());
        m_state.setValue<double>(REST_FREQUENCY, other->getRestFrequency() );
        m_state.setValue<QString>(IMAGE_NAME, other->getNameImage());
        m_state.setValue<QString>(REGION_NAME, other->getNameRegion());
    }
}

QColor CurveData::getColor() const {
    int red = m_state.getValue<int>( Util::RED );
    int green = m_state.getValue<int>( Util::GREEN );
    int blue = m_state.getValue<int>( Util::BLUE );
    return QColor( red, green, blue );
}


QString CurveData::getLineStyle() const {
    return m_state.getValue<QString>( STYLE );
}


QString CurveData::getName() const {
    return m_state.getValue<QString>( Util::NAME );
}


QString CurveData::getNameImage() const {
    return m_state.getValue<QString>( IMAGE_NAME );
}


QString CurveData::getNameRegion() const {
    return m_state.getValue<QString>( REGION_NAME );
}


double CurveData::getRestFrequency() const {
    return m_state.getValue<double>( REST_FREQUENCY );
}


std::shared_ptr<Carta::Lib::Image::ImageInterface> CurveData::getSource() const {
    return m_imageSource;
}

QString CurveData::getStateString() const{
    return m_state.toString();
}

QString CurveData::getStatistic() const {
    return m_state.getValue<QString>( STATISTIC );
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

    m_state.insertValue<QString>( STATISTIC, m_stats->getDefault());
    m_state.insertValue<double>(REST_FREQUENCY, 0 );
    m_state.insertValue<QString>(IMAGE_NAME, "");
    m_state.insertValue<QString>(REGION_NAME, "");
}


void CurveData::_initializeStatics(){
    if ( m_lineStyles == nullptr ){
       m_lineStyles = Util::findSingletonObject<LineStyles>();
    }
    if ( m_stats == nullptr ){
        m_stats = Util::findSingletonObject<ProfileStatistics>();
    }
}

bool CurveData::isMatch( const QString& name ) const {
    bool match = false;
    if ( m_state.getValue<QString>(Util::NAME) == name ){
        match = true;
    }
    return match;
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

QString CurveData::setImageName( const QString& imageName ){
    QString result;
    if ( imageName.trimmed().length() > 0 ){
        QString oldImageName = m_state.getValue<QString>(IMAGE_NAME );
        if ( oldImageName != imageName ){
            m_state.setValue<QString>(IMAGE_NAME, imageName );
        }
    }
    else {
        result = "Please specify a non-trivial profile image name.";
    }
    return result;
}


QString CurveData::setName( const QString& curveName ){
    QString result;
    if ( curveName.length() > 0 ){
        QString oldName = m_state.getValue<QString>( Util::NAME );
        if ( oldName != curveName ){
            m_state.setValue<QString>( Util::NAME, curveName );
        }
    }
    else {
        result = "Please specify a non-blank name for the curve.";
    }
    return result;
}


void CurveData::setSource( std::shared_ptr<Carta::Lib::Image::ImageInterface> imageSource ){
    m_imageSource = imageSource;
}


CurveData::~CurveData(){
}
}
}
