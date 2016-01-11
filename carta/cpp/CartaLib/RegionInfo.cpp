#include "RegionInfo.h"
#include <QDebug>
#include <QtCore/qmath.h>

namespace Carta {
namespace Lib {

RegionInfo::RegionInfo(){
    m_regionType = RegionType::Unknown;
}

void RegionInfo::addCorner( double xPixel, double yPixel ){
    std::pair<double,double> newCorner( xPixel, yPixel );
    if ( !isCorner( newCorner) ){
        m_corners.push_back( newCorner );
    }
    else {
        qDebug() << "Corner x="<<xPixel<<" y="<<yPixel<<" already exists";
    }
}

void RegionInfo::clearCorners(){
    m_corners.clear();
}

std::vector<std::pair<double,double> > RegionInfo::getCorners() const {
    return m_corners;
}

RegionInfo::RegionType RegionInfo::getRegionType() const {
    return m_regionType;
}

bool RegionInfo::isCorner( std::pair<double,double> pt ) const {
    //Since region corners are stored in pixel, a margin of error of 0.25
    //should work. We will use Pythagorean distance.
    bool existingCorner = false;
    for ( std::pair<double,double> corner : m_corners ){
        double dist = qSqrt( qPow( corner.first - pt.first, 2) + qPow( corner.second - pt.second, 2) );
        if ( dist < 0.25 ){
            existingCorner = true;
            break;
        }
    }
    return existingCorner;
}

void RegionInfo::setRegionType( RegionType type ){
    m_regionType = type;
}

void RegionInfo::setCorners( const std::vector< std::pair<double,double> > & corners ){
    m_corners = corners;
}


RegionInfo::~RegionInfo(){

}

}
}


