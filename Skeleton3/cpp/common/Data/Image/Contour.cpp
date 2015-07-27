#include "Contour.h"
#include "Data/Util.h"
#include <QDebug>

namespace Carta {

namespace Data {


const QString Contour::CLASS_NAME = "Contour";
const QString Contour::LEVEL = "level";
const double Contour::ERROR_MARGIN = 0.000001;


class Contour::Factory : public Carta::State::CartaObjectFactory {

    public:

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Contour(path, id);
        }
};

bool Contour::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new Contour::Factory());


Contour::Contour( const QString& prefix, const QString& identifier ):
    CartaObject( CLASS_NAME, prefix, identifier ){
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


void Contour::_initializeState(){
    m_state.insertValue<int>( Util::RED, 0 );
    m_state.insertValue<int>( Util::BLUE, 0 );
    m_state.insertValue<int>( Util::GREEN, 0 );
    m_state.insertValue<int>( Util::ALPHA, 0 );
    m_state.insertValue<double>(Util::PEN_WIDTH, 1 );
    m_state.insertValue<double>( LEVEL, 0 );
    //Don't need to flush the state since there isn't a view listening.
}

QStringList Contour::setColor( int redAmount, int greenAmount, int blueAmount, int alphaAmount ){
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

    //Alpha
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

void Contour::setLevel( double level ){
    if ( qAbs( level - m_state.getValue<double>( LEVEL) ) > ERROR_MARGIN ){
        m_state.setValue( LEVEL, level );
    }
}

void Contour::setWidth( double width ){
    double oldWidth = m_state.getValue<double>( Util::PEN_WIDTH );
    if ( qAbs( width  - oldWidth ) > ERROR_MARGIN ){
        m_state.setValue( Util::PEN_WIDTH, width );
    }
}


Contour::~Contour() {

}
}
}
