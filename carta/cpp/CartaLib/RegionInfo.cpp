#include "RegionInfo.h"

namespace Carta {
namespace Lib {

RegionInfo::RegionInfo(){
    m_regionType = RegionType::Unknown;
}

void RegionInfo::addCorner( double xPixel, double yPixel ){
    corners.push_back( std::pair<double,double>(xPixel, yPixel) );
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


