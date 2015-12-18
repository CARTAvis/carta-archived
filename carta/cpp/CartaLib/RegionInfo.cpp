#include "RegionInfo.h"

namespace Carta {
namespace Lib {

RegionInfo::RegionInfo(){
    m_regionType = RegionType::Unknown;
}

void RegionInfo::addCorner( std::pair<double,double> corner ){
    corners.push_back( corner );
}

void RegionInfo::clearCorners(){
    corners.clear();
}

std::vector<std::pair<double,double> > RegionInfo::getCorners() const {
    return corners;
}

RegionInfo::RegionType RegionInfo::getRegionType() const {
    return m_regionType;
}

void RegionInfo::setRegionType( RegionType type ){
    m_regionType = type;
}


RegionInfo::~RegionInfo(){

}

}
}


