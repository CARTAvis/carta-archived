#include <math.h>
#include "DataContours.h"
#include "Contour.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"
#include "CartaLib/IContourGeneratorService.h"
#include "DefaultContourGeneratorService.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString DataContours::CLASS_NAME = "DataContours";
const QString DataContours::CONTOURS = "contours";
const QString DataContours::CONTOUR_DRAW = "contourDraw";


class DataContours::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new DataContours (path, id);
        }
    };

bool DataContours::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new DataContours::Factory());

DataContours::DataContours( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    // create the contour calculation service and hook it up
    m_contourService.reset( new Carta::Core::DefaultContourGeneratorService( this ) );
    _initializeDefaultState();


}

std::vector<QPen> DataContours::getPens() const {
    int penCount = m_contours.size();
    std::vector<QPen> pens( penCount );
    for ( int i = 0; i < penCount; i++ ){
        pens[i] = m_contours[i]->getPen();
    }
    return pens;
}

std::shared_ptr<Carta::Lib::IContourGeneratorService> DataContours::_getRenderer(){
    return m_contourService;
}



void DataContours::_initializeDefaultState(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Contour* contour1 = objMan->createObject<Contour>();
    contour1->setColor( 255, 0, 0, 128 );
    contour1->setLevel( 0.25 );
    contour1->setWidth( 3 );
    m_contours.push_back( contour1 );

    Contour* contour2 = objMan->createObject<Contour>();
    contour2->setColor( 255, 0, 0, 128 );
    contour2->setLevel( 0.5 );
    contour2->setWidth( 3 );
    m_contours.push_back( contour2 );

    Contour* contour3 = objMan->createObject<Contour>();
    contour3->setColor( 255, 0, 0, 128 );
    contour3->setLevel( 0.75 );
    contour3->setWidth( 3 );
    m_contours.push_back( contour3 );

    int contourCount = m_contours.size();
    m_state.insertArray( CONTOURS, contourCount );
    std::vector<double> levels( contourCount );
    for ( int i = 0; i < contourCount; i++ ){
        QString indexLookup = Carta::State::UtilState::getLookup( CONTOURS, i );
        m_state.setValue<QString>( indexLookup, m_contours[i]->getStateString());
        levels[i] = m_contours[i]->getLevel();
    }
    m_contourService->setLevels( levels );
    m_state.insertValue<bool>( CONTOUR_DRAW, false );
    m_state.flushState();
}

bool DataContours::isContourDraw() const {
    return m_state.getValue<bool>( CONTOUR_DRAW );
}


DataContours::~DataContours(){

}
}
}
