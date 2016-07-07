#include "CurveData.h"
#include "CartaLib/Hooks/ConversionSpectralHook.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Plotter/LineStyles.h"
#include "Data/Profile/ProfileStatistics.h"
#include "Data/Profile/ProfilePlotStyles.h"
#include "Data/Units/UnitsFrequency.h"
#include "Data/Units/UnitsWavelength.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include "Globals.h"
#include <QtCore/qmath.h>
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString CurveData::CLASS_NAME = "CurveData";
const QString CurveData::COLOR = "color";
const QString CurveData::FIT = "fit";
const QString CurveData::FIT_SELECT = "fitSelect";
const QString CurveData::PLOT_STYLE = "plotStyle";
const QString CurveData::STYLE = "style";
const QString CurveData::STYLE_FIT = "styleFit";
const QString CurveData::STATISTIC = "stat";
const QString CurveData::REGION_NAME = "region";
const QString CurveData::IMAGE_NAME = "image";
const QString CurveData::REST_FREQUENCY = "restFrequency";
const QString CurveData::REST_FREQUENCY_UNITS = "restFrequencyUnits";
const QString CurveData::REST_UNIT_FREQ = "restUnitFreq";
const QString CurveData::REST_UNIT_WAVE = "restUnitWave";


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
UnitsFrequency* CurveData::m_frequencyUnits = nullptr;
UnitsWavelength* CurveData::m_wavelengthUnits = nullptr;

using Carta::State::UtilState;
using Carta::State::StateInterface;

CurveData::CurveData( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ),
            m_stateFit( UtilState::getLookup( path, CurveData::FIT)){
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
    getMinMax( &dataMinX, &dataMaxX, &dataMinY, &dataMaxY );
    errorX = _calculateRelativeError( dataMinX, dataMaxX );
    errorY = _calculateRelativeError( dataMinY, dataMaxY );
}

void CurveData::_convertRestFrequency( const QString& oldUnits, const QString& newUnits,
        int significantDigits, double errorMargin ){
    //Do conversion
    if ( m_imageSource ){
        std::vector<double> converted(1);
        converted[0] = m_state.getValue<double>( REST_FREQUENCY );
        auto result = Globals::instance()-> pluginManager()
                                     -> prepare <Carta::Lib::Hooks::ConversionSpectralHook>(m_imageSource, oldUnits, newUnits, converted );
        auto lam = [&converted] ( const Carta::Lib::Hooks::ConversionSpectralHook::ResultType &data ) {
            converted = data;
        };
        try {
            result.forEach( lam );
            if ( converted.size() > 0 ){
                double roundedFreq = Util::roundToDigits( converted[0], significantDigits );
                double oldFreq = m_state.getValue<double>( REST_FREQUENCY );
                if ( qAbs( oldFreq - roundedFreq ) > errorMargin ){
                    m_state.setValue<double>(REST_FREQUENCY, roundedFreq );
                }
            }
        }
        catch( char*& error ){
            QString errorStr( error );
            ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
            hr->registerError( errorStr );
        }
    }
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
        m_state.setValue<QString>( STYLE_FIT, other->getLineStyleFit());
        m_state.setValue<QString>( STATISTIC, other->getStatistic());
        m_state.setValue<double>(REST_FREQUENCY, other->getRestFrequency() );
        m_state.setValue<QString>(IMAGE_NAME, other->getNameImage());
        m_state.setValue<QString>(REGION_NAME, other->getNameRegion());
        m_state.setValue<bool>(FIT_SELECT, other->isSelectedFit());
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


std::vector< std::pair<double,double> > CurveData::getFitData() const {
    int fitCount = m_fitDataX.size();
    std::vector< std::pair<double,double> > fitData( fitCount );
    for ( int i = 0; i < fitCount; i++ ){
        fitData[i].first = m_fitDataX[i];
        fitData[i].second = m_fitDataY[i];
    }
    return fitData;
}

std::vector<std::pair<double,double> > CurveData::getFitResiduals() const {
    int fitCount = m_fitDataY.size();
    int curveCount = m_plotDataY.size();
    CARTA_ASSERT( fitCount = 2 * curveCount );
    std::vector< std::pair<double,double> > residuals( curveCount );
    for ( int i = 0; i < curveCount; i++ ){
        residuals[i].first = m_plotDataX[i];
        residuals[i].second = m_plotDataY[i] - m_fitDataY[2*i];
    }
    return residuals;
}

QString CurveData::getFitParams() const {
    return m_state.toString( FIT );
}

std::vector<std::tuple<double,double,double> > CurveData::getGaussParams() const {
    return m_gaussParams;
}


std::shared_ptr<Carta::Lib::Image::ImageInterface> CurveData::getImage() const {
    return m_imageSource;
}


void CurveData::getMinMax(double* xmin, double* xmax, double* ymin,
        double* ymax) const {
    *xmin = std::numeric_limits<double>::max();
    *ymin = std::numeric_limits<double>::max();
    *xmax = -1 * (*xmin);
    *ymax = -1 * (*ymin);
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

QString CurveData::getLineStyleFit() const {
    return m_state.getValue<QString>( STYLE_FIT );
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


std::vector< std::pair<double, double> > CurveData::getPlotData() const {
    int dataCount = m_plotDataX.size();
    std::vector<std::pair<double,double> > data( dataCount );
    for ( int i = 0; i < dataCount; i++ ){
        data[i] = std::pair<double,double>( m_plotDataX[i], m_plotDataY[i] );
    }
    return data;
}


Carta::Lib::ProfileInfo CurveData::getProfileInfo() const {
    Carta::Lib::ProfileInfo profInfo;
    profInfo.setRestFrequency( getRestFrequency() );
    profInfo.setRestUnit( getRestUnits() );
    QString stat = getStatistic();
    Carta::Lib::ProfileInfo::AggregateType agType = m_stats ->getTypeFor( stat );
    profInfo.setAggregateType( agType );
    return profInfo;
}


double CurveData::getRestFrequency() const {
    return m_state.getValue<double>( REST_FREQUENCY );
}

QString CurveData::getRestUnits() const {
    bool freqUnits = m_state.getValue<bool>( REST_FREQUENCY_UNITS );
    QString units;
    if ( freqUnits ){
        units = m_state.getValue<QString>( REST_UNIT_FREQ );
    }
    else {
        units = m_state.getValue<QString>( REST_UNIT_WAVE );
    }
    return units;
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
    QString defaultLineStyleFit = m_lineStyles->getDefaultSecondary();
    m_state.insertValue<QString>( STYLE_FIT, defaultLineStyleFit );
    QString defaultPlotStyle = m_plotStyles->getDefault();
    m_state.insertValue<QString>( PLOT_STYLE, defaultPlotStyle );


    m_state.insertValue<QString>( STATISTIC, m_stats->getDefault());
    m_state.insertValue<double>(REST_FREQUENCY, 0 );
    m_state.insertValue<bool>(REST_FREQUENCY_UNITS, true );
    m_state.insertValue<QString>(REST_UNIT_FREQ, m_frequencyUnits->getDefault());
    m_state.insertValue<QString>(REST_UNIT_WAVE, m_wavelengthUnits->getDefault());

    m_state.insertValue<QString>(IMAGE_NAME, "");
    m_state.insertValue<QString>(REGION_NAME, "");

    m_state.insertValue<bool>(FIT_SELECT, true );
    m_stateFit.insertObject( FIT );
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
    if ( m_frequencyUnits == nullptr ){
        m_frequencyUnits = Util::findSingletonObject<UnitsFrequency>();
    }
    if ( m_wavelengthUnits == nullptr ){
        m_wavelengthUnits = Util::findSingletonObject<UnitsWavelength>();
    }
}

bool CurveData::isFitted() const {
    bool fitted = false;
    if ( m_fitDataX.size() > 0 && m_fitDataY.size() > 0 ){
        fitted = true;
    }
    return fitted;
}

bool CurveData::isMatch( const QString& name ) const {
    bool match = false;
    if ( m_state.getValue<QString>(Util::NAME) == name ){
        match = true;
    }
    return match;
}

bool CurveData::isSelectedFit() const {
    return m_state.getValue<bool>( FIT_SELECT );
}


void CurveData::resetRestFrequency(){
    bool restFreqSet = false;
    setRestFrequency( m_restFrequency, 0, &restFreqSet );
    QString freqUnits = m_frequencyUnits->getActualUnits( m_restUnits );
    if ( !freqUnits.isEmpty() ){
        m_state.setValue<bool>(REST_FREQUENCY_UNITS, true );
        m_state.setValue<QString>( REST_UNIT_FREQ, freqUnits);
    }
    else {
        QString waveUnits = m_wavelengthUnits->getActualUnits( m_restUnits );
        if ( !waveUnits.isEmpty()){
            m_state.setValue<bool>(REST_FREQUENCY_UNITS, false );
            m_state.setValue<QString>( REST_UNIT_WAVE, waveUnits );
        }
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



void CurveData::setDataX( const std::vector<double>& valsX ){
    CARTA_ASSERT( m_plotDataY.size() == valsX.size());
    m_plotDataX = valsX;
}

void CurveData::setDataY( const std::vector<double>& valsY ){
    CARTA_ASSERT( m_plotDataX.size() == valsY.size());
    m_plotDataY = valsY;
}

void CurveData::setGaussParams( const std::vector<std::tuple<double,double,double> >& params ){
    m_gaussParams = params;
}

void CurveData::setFit( const std::vector<double>& valsX, const std::vector<double>& valsY  ){
    CARTA_ASSERT( valsX.size() == valsY.size() );
    m_fitDataX = valsX;
    m_fitDataY = valsY;
}

void CurveData::setFitParams( const QString& fitParams ){
    m_stateFit.setObject( FIT, fitParams );
}


QString CurveData::setRestFrequency( double freq, double errorMargin, bool* valueChanged ){
    QString result;
    *valueChanged = false;
    if ( freq >= 0 ){
        double oldRestFrequency = m_state.getValue<double>( REST_FREQUENCY );
        if ( qAbs( freq - oldRestFrequency ) > errorMargin ){
            *valueChanged = true;
            m_state.setValue<double>( REST_FREQUENCY, freq );
        }
    }
    else {
        result = "Rest frequency must be nonnegative.";
    }
    return result;
}


void CurveData::setSelectedFit( bool selected ){
    m_state.setValue<bool>( FIT_SELECT, selected );
}

QString CurveData::setStatistic( const QString& stat ){
    QString result;
    QString actualStat = m_stats->getActualStatistic( stat );
    if ( !actualStat.isEmpty() ){
        QString oldStat = m_state.getValue<QString>( STATISTIC );
        if ( oldStat != actualStat ){
            m_state.setValue<QString>( STATISTIC, actualStat );
        }
    }
    else {
        result = "Unrecognized profile statistic: "+stat;
    }
    return result;
}

void CurveData::setRestQuantity( double restFrequency, const QString& restUnit ){
    if ( restFrequency >= 0 ){
        m_restFrequency = restFrequency;
        m_restUnits = restUnit;
        QString actualUnit = m_frequencyUnits->getActualUnits( restUnit );
        bool restUnitType = true;
        if ( actualUnit.isEmpty()){
            //Not a valid frequency unit, try a wavelength unit.
            actualUnit = m_wavelengthUnits->getActualUnits( restUnit );
            restUnitType = false;
        }
        if ( !actualUnit.isEmpty() ){
            m_state.setValue<double>(REST_FREQUENCY, restFrequency );
            m_state.setValue<bool>(REST_FREQUENCY_UNITS, restUnitType );
            if ( restUnitType ){
                m_state.setValue<QString>(REST_UNIT_FREQ, actualUnit);
            }
            else {
                m_state.setValue<QString>(REST_UNIT_WAVE, actualUnit );
            }
        }
    }
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

QString CurveData::setLineStyleFit( const QString& lineStyle ){
    QString result;
    QString oldStyle = m_state.getValue<QString>( STYLE_FIT );
    QString actualStyle = m_lineStyles->getActualLineStyle( lineStyle );
    if ( !actualStyle.isEmpty() ){
        if ( actualStyle != oldStyle ){
            m_state.setValue<QString>( STYLE_FIT, actualStyle );
        }
    }
    else {
        result = "Unrecognized fit line style: " + lineStyle;
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

QString CurveData::setRestUnits( const QString& restUnits, int significantDigits, double errorMargin){
    bool freqUnits = m_state.getValue<bool>( REST_FREQUENCY_UNITS );
    QString result;
    QString actualUnits;
    if ( freqUnits ){
        actualUnits = m_frequencyUnits->getActualUnits( restUnits );
    }
    else {
        actualUnits = m_wavelengthUnits ->getActualUnits( restUnits );
    }
    if ( !actualUnits.isEmpty() ){
        QString oldUnits = getRestUnits();
        if ( actualUnits != oldUnits ){
            _convertRestFrequency( oldUnits, actualUnits, significantDigits, errorMargin );
            if ( freqUnits ){
                m_state.setValue<QString>( REST_UNIT_FREQ, actualUnits );
            }
            else {
                m_state.setValue<QString>( REST_UNIT_WAVE, actualUnits );
            }
        }
    }
    else {
        result = "Unrecognized rest units: " + restUnits;
    }
    return result;
}

void CurveData::setRestUnitType( bool restUnitsFreq, int significantDigits, double errorMargin ){
    bool oldType = m_state.getValue<bool>( REST_FREQUENCY_UNITS );
    if ( oldType != restUnitsFreq ){
        m_state.setValue<bool>(REST_FREQUENCY_UNITS, restUnitsFreq );
        //Need to translate the old frequency, unit pair to a new one.
        QString oldUnits = m_state.getValue<QString>( REST_UNIT_FREQ );
        QString newUnits = m_state.getValue<QString>( REST_UNIT_WAVE );
        if ( restUnitsFreq ){
            QString tmpUnits = oldUnits;
            oldUnits = newUnits;
            newUnits = tmpUnits;
        }
        _convertRestFrequency( oldUnits, newUnits, significantDigits, errorMargin );
    }
}



void CurveData::setSource( std::shared_ptr<Carta::Lib::Image::ImageInterface> imageSource ){
    m_imageSource = imageSource;
}


CurveData::~CurveData(){
}
}
}
