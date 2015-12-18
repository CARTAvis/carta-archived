/***
 * A region of an image.
 */

#pragma once

#include "State/StateInterface.h"
#include "State/ObjectManager.h"
#include "CartaLib/RegionInfo.h"

namespace Carta {

namespace Data {

class Region : public Carta::State::CartaObject {

public:

    /**
     * Return the RegionType corresponding to the given string representation.
     * @param regionTypeStr - a string representation of a region shape such as "ellipse".
     * @return - the corresponding RegionType.
     */
    static Carta::Lib::RegionInfo::RegionType getRegionType( const QString& regionTypeStr );

    /**
     * Factory for making regions of various types.
     * @param regionType the type of region to make.
     * @return the unique path of the region produced.
     */
    static QString makeRegion( const QString& regionType );

    /**
     * Return the type of region, which corresponds to its shape.
     * @return - the RegionType.
     */
    virtual Carta::Lib::RegionInfo::RegionType getType() const = 0;

    /**
     * Return a string representation of the region shape.
     * @return - a string representation of the type of region.
     */
    virtual QString getTypeString() const = 0;

    virtual ~Region();

protected:

    const static QString POLYGON_REGION;
    const static QString ELLIPSE_REGION;

    /**
     * Construct a region.
     */
    Region( const QString& className, const QString& path, const QString& id );

private:
    void _initializeCallbacks();

};
}
}
