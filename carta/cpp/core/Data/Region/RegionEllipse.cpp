#include "RegionEllipse.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/Ellipse.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString RegionEllipse::CLASS_NAME = "RegionEllipse";


class RegionEllipse::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new RegionEllipse (path, id);
    }
};

bool RegionEllipse::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new RegionEllipse::Factory());


RegionEllipse::RegionEllipse(const QString& path, const QString& id )
    :Region( CLASS_NAME, path, id ){
    _initializeState();
}


std::shared_ptr<Carta::Lib::Regions::RegionBase> RegionEllipse::getModel() const {
    std::shared_ptr<Carta::Lib::Regions::RegionBase> info( new Carta::Lib::Regions::Ellipse() );
    return info;
}

void RegionEllipse::_initializeState(){
    m_state.setValue<QString>( REGION_TYPE, Carta::Lib::Regions::Ellipse::TypeName );
    m_state.flushState();
}

bool RegionEllipse::isPointInside( const QPointF & /*pt*/ ) const {
	return false;
}

void RegionEllipse::_restoreState( const QString& stateStr ){
    Region::_restoreState( stateStr );
    m_state.flushState();
}


QJsonObject RegionEllipse::toJSON() const {
    QJsonObject descript = Region::toJSON();
    return descript;
}

RegionEllipse::~RegionEllipse(){
}
}
}
