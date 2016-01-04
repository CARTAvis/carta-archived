/***
 * Factory for generating regions
 */

#pragma once

#include "CartaLib/RegionInfo.h"

namespace Carta {

namespace Data {

class RegionFactory {

public:
    /**
     * Make a region based on region information.
     * @param regionInfo - information for drawing the region such as corner points.
     */
    static std::shared_ptr<Region>
    makeRegion( std::shared_ptr<Carta::Lib::RegionInfo> regionInfo );

    /**
     * Make a region based on a desired type.
     * @param regionType - the type of region to make such as Polygon, Ellipse, etc.
     */
    static std::shared_ptr<Region>
    makeRegion( Carta::Lib::RegionInfo::RegionType regionType );

    virtual ~RegionFactory();

private:

    /**
     * Construct a region factory.
     */
    RegionFactory();

};
}
}
