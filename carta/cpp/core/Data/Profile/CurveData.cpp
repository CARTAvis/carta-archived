#include "CurveData.h"
#include "Data/Util.h"
#include "Data/Plotter/LineStyles.h"
#include "Data/Profile/ProfileStatistics.h"
#include "Data/Profile/ProfilePlotStyles.h"
#include "State/UtilState.h"
#include <QtCore/qmath.h>
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString CurveData::CLASS_NAME = "CurveData";
const QString CurveData::COLOR = "color";
const QString CurveData::PLOT_STYLE = "plotStyle";
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
ProfilePlotStyles* CurveData::m_plotStyles = nullptr;

using Carta::State::UtilState;
using Carta::State::StateInterface;

CurveData::CurveData( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ){
    _initializeStatics();
    _initializeDefaultState();
}

double CurveData::_calculateRelativeError( double minValue, double maxValue ) const {
    double range = qAbs( maxValue - minValue );
    double error = 0;
    if ( ! std::isnan( range ) ){
        //Divide by powers of 10 until we get something less than 1.
        if ( range > 1 ){
            int i = 0;
            double powerValue = qPow(10,i);
            while ( range / powerValue > 1 ){
                i++;
                powerValue = qPow(10,i);
            }
            error = powerValue;
        }
        //Multiply by powers of 10 until we get something larger than 1.
        else {
            int i = 0;
            while ( range * qPow(10,i) < 1 ){
                i++;
            }
            error = 1 / qPow(10,i);
        }
    }
    //Add in arbitrary scaling for more accuracy.
    error = error * .005;
    return error;
}

void CurveData::_calculateRelativeErrors( double& errorX, double& errorY ) const {
    double dataMinX = std::numeric_limits<double>::max();
    double dataMaxX = -1 * dataMinX;
    double dataMinY = std::numeric_limits<double>::max();;
    double dataMaxY = -1 * dataMinY;
    _getMinMax( &dataMinX, &dataMaxX, &dataMinY, &dataMaxY );
    errorX = _calculateRelativeError( dataMinX, dataMaxX );
    errorY = _calculateRelativeError( dataMinY, dataMaxY );
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

QString CurveData::getCursorText( double x, double y, double* error ) const {
    int dataCount = m_plotDataX.size();
    //Normalize the error by the size of the data.
    double targetErrorX = 0;
    double targetErrorY = 0;
    *error = 0;
    _calculateRelativeErrors( targetErrorX, targetErrorY );

    //Find the index that yields the smallest error
    //withen acceptable bounds.
    int selectedIndex = -1;
    double minErrorX = std::numeric_limits<double>::max();
    for ( int i = 0; i < dataCount; i++ ) {
        double curveX = m_plotDataX[i];
        double curveY = m_plotDataY[i];

        double errorX = fabs( curveX - x );
        double errorY = fabs( curveY - y );
        if ( errorX < targetErrorX && errorY < targetErrorY ) {
            if ( errorX < minErrorX ){
                selectedIndex = i;
                minErrorX = errorX;
            }
        }
    }

    //If we found an index with acceptable bounds, show the point
    //as a tooltip.
    QString toolTipStr;
    if ( selectedIndex >= 0 ){
        *error = qSqrt( qPow( m_plotDataX[selectedIndex] - x, 2 )+ qPow(m_plotDataY[selectedIndex] - y, 2) );
        toolTipStr.append( "(" );
        toolTipStr.append(QString::number( m_plotDataX[selectedIndex] ));
        //toolTipStr.append( " " +xUnit +", " );
        toolTipStr.append( ", ");
        toolTipStr.append(QString::number( m_plotDataY[selectedIndex],'g',4 ));
        //toolTipStr.append( " " + yUnit+ ")");
        toolTipStr.append( ")");
    }
    return toolTipStr;
}


double CurveData::getDataMax() const {
    return m_plotDataX.size();
}


void CurveData::_getMinMax(double* xmin, double* xmax, double* ymin,
        double* ymax) const {
    int maxPoints = m_plotDataX.size();
    for (int i = 0; i < maxPoints; ++i) {
        double dx = m_plotDataX[i];
        double dy = m_plotDataY[i];
        *xmin = (*xmin > dx) ? dx : *xmin;
        *xmax = (*xmax < dx) ? dx : *xmax;
        *ymin = (*ymin > dy) ? dy : *ymin;
        *ymax = (*ymax < dy) ? dy : *ymax;
    }
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
    QString defaultPlotStyle = m_plotStyles->getDefault();
    m_state.insertValue<QString>( PLOT_STYLE, defaultPlotStyle );

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
    if ( m_plotStyles == nullptr ){
        m_plotStyles = Util::findSingletonObject<ProfilePlotStyles>();
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

QString CurveData::setPlotStyle( const QString& plotStyle ){
    QString result;
    QString oldStyle = m_state.getValue<QString>( PLOT_STYLE );
    QString actualStyle = m_plotStyles->getActualStyle( plotStyle );
    if ( !actualStyle.isEmpty() ){
        if ( actualStyle != oldStyle ){
            m_state.setValue<QString>( PLOT_STYLE, actualStyle );
        }
    }
    else {
        result = "Unrecognized plot style: " + plotStyle;
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
