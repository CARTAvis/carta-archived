/***
 * Factory for generating regions
 */

#pragma once

#include "CartaLib/Regions/IRegion.h"

namespace Carta {

namespace Data {

class Region;

class RegionFactory {

public:
    /**
     * Make a region based on region information.
     * @param regionInfo - information for drawing the region such as corner points.
     */
    static std::shared_ptr<Region> makeRegion( Carta::Lib::Regions::RegionBase* regionInfo );



    /**
     * Make a region based on stored state.
     * @param regionState - a string representation of region state.
     */
    static std::shared_ptr<Region> makeRegion( const QString& regionState );

    /**
       * Make a region based on a desired type.
       * @param regionType - the type of region to make such as Polygon, Ellipse, etc.
       */
      static std::shared_ptr<Region> makeRegionType( const QString& regionType );

    virtual ~RegionFactory();

private:



    /**
     * Construct a region factory.
     */
    RegionFactory();

    RegionFactory( const RegionFactory& other);
    RegionFactory& operator=( const RegionFactory& other );

};
}
}
