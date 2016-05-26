#include "Gamma.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString Gamma::CLASS_NAME = "Gamma";
const QString Gamma::LEVEL_CURVES = "levelCurves";

const int Gamma::MAX_N_DIG = 10;
const int Gamma::LOWER_BOUND = -1;
const int Gamma::UPPER_BOUND = 1;
const int Gamma::TABLE_SIZE = 20;

class Gamma::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Gamma (path, id);
        }
    };


bool Gamma::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Gamma::Factory());


Gamma::Gamma( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    _initializeDefaultState();
}

std::pair<double,double> Gamma::find( double gamma ) const {
    //Look at positive x values only
    std::pair<double,double> result( 0, 0 );
    double target = gamma;
    if ( gamma < 1 ){
        target = 1/gamma;
    }
    if ( gamma != 1 ){
        //Binary search on main diagonal
       double midX = 0;
       double midY = 0;
       double minX = 0;
       double maxX = 1;
       double minY = 0;
       double maxY = 1;
       double maxGamma = getGamma( maxX, maxY);
       if ( target > maxGamma ){
           midX = maxX;
           midY = maxY;
       }
       else {
           double diff = fabs( 1 - target );
           const double ERROR = 0.001;
           while( diff > ERROR ){
               midX = (minX + maxX)/2;
               midY = (minY + maxY)/2;
               double midGamma = getGamma( midX, midY );

               if ( midGamma < target ){
                   minX = midX;
                   minY = midY;
               }
               else {
                   maxX = midX;
                   maxY = midY;
               }

               diff = fabs( midGamma - target );
           }
           if ( gamma < 1 ){
               midX = midX * -1;
           }
       }
       result = std::pair<double,double>( midX, midY );
    }
    return result;
}


double Gamma::getGamma( double x, double y ) const{
    double ndig = ( y + 1) / 2 * MAX_N_DIG;
    double expo = std::pow( 2.0, ndig);
    double xx = std::pow(x, 3);
    double gamma = fabs(xx) * expo + 1;
    if( x < 0){
        gamma = 1 / gamma;
    }
    return gamma;
}

double Gamma::getY( double x, double gamma ) const {
    double val = (gamma - 1) / std::pow(x, 3 );
    double valY = 1;
    if ( val > 0 ){
        valY = log( val ) / log( 2 ) / 5 - 1;
    }
    return valY;
}

double Gamma::getX( double y, double gamma ) const {
    double val = (gamma - 1) / std::pow( 2, 5*(y+1));
    val = std::pow( val, 0.33333 );
    return val;
}

void Gamma::_initializeDefaultState(){
    const int GAMMA_COUNT = 11;
    double gammas[GAMMA_COUNT] = {1.1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
    m_state.insertArray( LEVEL_CURVES, GAMMA_COUNT * 2 );
    for ( int i = 0; i < GAMMA_COUNT; i++ ){
        int power = -10;
        int base = 10;
        double step = std::pow( base, power );
        const int POINT_COUNT = 10;
        std::pair<double,double> points[POINT_COUNT];
        int plotCount = 0;
        int plotStartIndex = -1;
        for ( int j = 1; j <= POINT_COUNT; j++ ){
            double valX = step+j*.1;
            if ( valX > 1 ){
                valX = 1;
            }
            double valY = getY( valX, gammas[i]);
            if ( j == 1 ){
                valY = 1;
                valX = getX( valY, gammas[i] );
            }
            points[j-1]=std::pair<double,double>( valX, valY );
            if ( valY <= 1 && valY >= -1 && valX<=1 && valX>= -1 ){
                plotCount++;
                if ( plotStartIndex < 0 ){
                    plotStartIndex = j - 1;
                }
            }
        }

        QString curveLookup = Carta::State::UtilState::getLookup( LEVEL_CURVES, 2*i);
        QString curveLookup2 = Carta::State::UtilState::getLookup( LEVEL_CURVES, 2*i+1 );
        if ( plotCount > 0 ){
            m_state.setArray( curveLookup, plotCount );
            m_state.setArray( curveLookup2, plotCount );
            int pointIndex = 0;
            for ( int j = plotStartIndex; j < POINT_COUNT; j++ ){
                if ( points[j].second <= 1 && points[j].second >=-1 &&
                        points[j].first<= 1 && points[j].first >= -1){

                    //Positive x
                    QString pLookup = Carta::State::UtilState::getLookup( curveLookup, pointIndex );
                    QString xValKey = Carta::State::UtilState::getLookup( pLookup, Util::XCOORD);
                    m_state.insertValue<double>( xValKey, points[j].first );
                    QString yValKey = Carta::State::UtilState::getLookup( pLookup, Util::YCOORD);
                    m_state.insertValue<double>( yValKey, points[j].second );

                    //Negative x
                    QString pLookup2 = Carta::State::UtilState::getLookup( curveLookup2, pointIndex );
                    xValKey = Carta::State::UtilState::getLookup( pLookup2, Util::XCOORD );
                    m_state.insertValue<double>( xValKey, -1 * points[j].first );
                    yValKey = Carta::State::UtilState::getLookup( pLookup2, Util::YCOORD );
                    m_state.insertValue<double>( yValKey, points[j].second );

                    pointIndex++;
                }
            }
        }
    }
    m_state.flushState();
}


Gamma::~Gamma(){

}
}
}
