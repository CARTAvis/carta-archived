#include "Contour.h"
#include "ContourStyles.h"
#include "Data/Util.h"
#include <QDebug>

namespace Carta {

namespace Data {


const QString Contour::CLASS_NAME = "Contour";
const QString Contour::LEVEL = "level";
const QString Contour::STYLE = "style";
const QString Contour::VISIBLE = "visible";
const double Contour::ERROR_MARGIN = 0.000001;
ContourStyles* Contour::m_contourStyles = nullptr;


Contour::Contour() :
    m_state(""){
    _initializeSingletons();
    _initializeState( );
}

double Contour::getLevel() const {
    return m_state.getValue<double>(LEVEL);
}

QString Contour::getStateString() const {
    return m_state.toString();
}

QPen Contour::getPen() const {
    QPen pen;
    pen.setCosmetic( true );
    pen.setWidthF( m_state.getValue<double>( Util::PEN_WIDTH) );
    int redAmount = m_state.getValue<int>( Util::RED );
    int greenAmount = m_state.getValue<int>( Util::GREEN );
    int blueAmount = m_state.getValue<int>( Util::BLUE );
    int alphaAmount = m_state.getValue<int>( Util::ALPHA );
    pen.setColor( QColor(redAmount, greenAmount, blueAmount, alphaAmount) );
    return pen;
}

void Contour::_initializeSingletons( ){
    //Load the available contour generate modes.
    if ( m_contourStyles  == nullptr){
        m_contourStyles = Util::findSingletonObject<ContourStyles>();
    }
}

void Contour::_initializeState(){
    m_state.insertValue<int>( Util::RED, 0 );
    m_state.insertValue<int>( Util::BLUE, 0 );
    m_state.insertValue<int>( Util::GREEN, 0 );
    m_state.insertValue<int>( Util::ALPHA, 255 );
    m_state.insertValue<double>(Util::PEN_WIDTH, 1 );
    m_state.insertValue<double>( LEVEL, 0 );
    QString lineStyle = m_contourStyles->getLineStyleDefault();
    m_state.insertValue<QString>( STYLE, lineStyle );
    m_state.insertValue<bool>( VISIBLE, true );
    //Don't need to flush the state since there isn't a view listening.
}

bool Contour::isVisible() const {
    return m_state.getValue<bool>( VISIBLE );
}

bool Contour::operator<( const Contour& other ) const {
    bool lessThan = false;
    double thisLevel = getLevel();
    double otherLevel = other.getLevel();
    if ( thisLevel < otherLevel && qAbs( thisLevel - otherLevel) > ERROR_MARGIN ){
        lessThan = true;
    }
    return lessThan;
}

/*bool Contour::operator==( const Contour& other ) const {
    bool equalContours = false;
    double thisLevel = getLevel();
    double otherLevel = other.getLevel();
    if ( qAbs( thisLevel - otherLevel ) < ERROR_MARGIN ){
        equalContours = true;
    }
    return equalContours;
}*/

QString Contour::setAlpha( int alphaAmount ){
    QString result;
    if ( 0 <= alphaAmount && alphaAmount <= Util::MAX_COLOR ){
        if ( m_state.getValue<int>( Util::ALPHA) != alphaAmount ){
            m_state.setValue<int>( Util::ALPHA, alphaAmount);
        }
    }
    else {
        result.append( "Alpha amount must be in [0,"+QString::number(Util::MAX_COLOR)+"]");
    }
    return result;
}

QStringList Contour::setColor( int redAmount, int greenAmount, int blueAmount/*, int alphaAmount*/ ){
    QStringList result;
    //Red amount
    if ( 0 <= redAmount && redAmount <= Util::MAX_COLOR ){
        if ( m_state.getValue<int>( Util::RED) != redAmount ){
            m_state.setValue<int>( Util::RED, redAmount);
        }
    }
    else {
        result.append( "Red amount must be in [0,"+QString::number(Util::MAX_COLOR)+"]");
    }

    //Green amount
    if ( 0 <= greenAmount && greenAmount <= Util::MAX_COLOR ){
        if ( m_state.getValue<int>( Util::GREEN) != greenAmount ){
            m_state.setValue<int>( Util::GREEN, greenAmount);
        }
    }
    else {
        result.append( "Green amount must be in [0,"+QString::number(Util::MAX_COLOR)+"]");
    }

    //Blue amount
    if ( 0 <= blueAmount && blueAmount <= Util::MAX_COLOR ){
        if ( m_state.getValue<int>( Util::BLUE) != blueAmount ){
            m_state.setValue<int>( Util::BLUE, blueAmount);
        }
    }
    else {
        result.append( "Blue amount must be in [0,"+QString::number(Util::MAX_COLOR)+"]");
    }
    return result;
}

void Contour::setLevel( double level ){
    if ( qAbs( level - m_state.getValue<double>( LEVEL) ) > ERROR_MARGIN ){
        m_state.setValue( LEVEL, level );
    }
}

QString Contour::setStyle( const QString& style ){
    QString result;
    QString actualStyle = m_contourStyles->getLineStyle( style );
    if ( !actualStyle.isEmpty() ){
        QString oldStyle = m_state.getValue<QString>( STYLE );
        if ( oldStyle != actualStyle ){
            m_state.setValue<QString>( STYLE, actualStyle );
        }
    }
    else {
        result = "Unrecognized contour line style: "+style;
    }
    return result;
}

bool Contour::setVisible( bool visible ){
    bool oldVisible = m_state.getValue<bool>(VISIBLE );
    bool stateChanged = false;
    if ( oldVisible != visible ){
        stateChanged = true;
        m_state.setValue<bool>( VISIBLE, visible );
    }
    return stateChanged;
}

QString Contour::setWidth( double width ){
    QString result;
    if ( width > 0 ){
        double oldWidth = m_state.getValue<double>( Util::PEN_WIDTH );
        if ( qAbs( width  - oldWidth ) > ERROR_MARGIN ){
            m_state.setValue( Util::PEN_WIDTH, width );
        }
    }
    else {
        result = "Contour width must be positive: "+QString::number( width );
    }
    return result;
}


Contour::~Contour() {

}
}
}
