#include "RegionEllipse.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString RegionEllipse::CLASS_NAME = "RegionEllipse";

bool RegionEllipse::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new RegionEllipse::Factory());

RegionEllipse::RegionEllipse(const QString& path, const QString& id ):
    Region( CLASS_NAME, path, id ){
    _initializeState();
}

Carta::Lib::RegionInfo::RegionType RegionEllipse::getType() const {
    return Carta::Lib::RegionInfo::RegionType::Ellipse;
}

QString RegionEllipse::getTypeString() const {
    return Region::ELLIPSE_REGION;
}


void RegionEllipse::_initializeState(){

}

/*void RegionEllipse::resetStateData(const QString & params ){

}*/


RegionEllipse::~RegionEllipse(){

}
}
}
