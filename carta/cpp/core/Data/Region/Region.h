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
     * Return the information associated with this region.
     * @return - information about the region.
     */
    std::shared_ptr<Carta::Lib::RegionInfo> getInfo() const;

    /**
     * Return the RegionType corresponding to the given string representation.
     * @param regionTypeStr - a string representation of a region shape such as "ellipse".
     * @return - the corresponding RegionType.
     */
    static Carta::Lib::RegionInfo::RegionType getRegionType( const QString& regionTypeStr );

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

    /**
     * Set region information (corner points, etc).
     * @param info - information on how to draw the region.
     */
    void setInfo( std::shared_ptr<Carta::Lib::RegionInfo> info );

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
    std::shared_ptr<Carta::Lib::RegionInfo> m_info;

};
}
}
