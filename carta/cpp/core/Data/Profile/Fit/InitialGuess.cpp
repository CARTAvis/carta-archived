#include "InitialGuess.h"
#include "State/UtilState.h"
#include "Data/Plotter/IScreenTranslator.h"

namespace Carta {

namespace Data {

const QString InitialGuess::CLASS_NAME = "InitialGuess";
const QString InitialGuess::CENTER = "center";
const QString InitialGuess::FBHW = "fbhw";
const QString InitialGuess::PEAK = "peak";
const double InitialGuess::ERROR_MARGIN = 0.00000001;


class InitialGuess::Factory : public Carta::State::CartaObjectFactory {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new InitialGuess (path, id);
    }
};

bool InitialGuess::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new InitialGuess::Factory());


using Carta::State::UtilState;
using Carta::State::StateInterface;

InitialGuess::InitialGuess( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ),
            m_screenTranslator( nullptr){
    m_center = 0;
    m_peak = 0;
    m_fbhw = 1;
    _initializeDefaultState();
}



void InitialGuess::_initializeDefaultState(){
    m_state.insertValue<double>( CENTER, m_center );
    m_state.insertValue<double>( PEAK, m_peak );
    m_state.insertValue<double>( FBHW, m_fbhw );
}

void InitialGuess::setPeak( double peak ) {
    if ( qAbs( peak - m_peak) > ERROR_MARGIN ){
        m_peak = peak;
        pixelsChanged();
    }
}
void InitialGuess::setFBHW( double fbhw ) {
    CARTA_ASSERT( fbhw > 0 );
    if (qAbs( fbhw - m_fbhw ) > ERROR_MARGIN ){
        m_fbhw = fbhw;
        pixelsChanged();
    }
}
void InitialGuess::setCenter( double center ) {
    m_center = center;
    if ( qAbs( m_center - center) > ERROR_MARGIN ){
        pixelsChanged();
    }
}

void InitialGuess::setScreenTranslator( std::shared_ptr<IScreenTranslator> trans ){
    m_screenTranslator = trans;
}

void InitialGuess::pixelsChanged(){
    QPointF main(m_center, m_peak);
    QPointF side( m_center - m_fbhw, m_peak );
    bool mainValid = false;
    bool sideValid = false;
    if ( m_screenTranslator ){
        QPointF mainTranslate = m_screenTranslator->getScreenPoint( main, &mainValid );
        QPointF sideTranslate = m_screenTranslator->getScreenPoint( side, &sideValid );
        if ( mainValid && sideValid ){
            m_state.setValue<double>( CENTER, mainTranslate.x());
            m_state.setValue<double>( PEAK, mainTranslate.y());
            double screenFBHW = qAbs( mainTranslate.x() - sideTranslate.x());
            m_state.setValue<double>( FBHW, screenFBHW );
        }
        else {
            qDebug() << "Did not translate mainValid="<<mainValid<<" screenValid="<<sideValid;
        }
    }
    else {
        qDebug() << "Could not translate points, translator was null";
    }
}

InitialGuess::~InitialGuess(){
}
}
}
