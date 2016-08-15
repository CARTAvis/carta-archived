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
    friend class RegionFactory;
    friend class DataFactory;
    friend class Stack;
public:

    /**
     * Add corners to the region.
     * @param corners - a list of corners to add to the region.
     */
    void addCorners( const std::vector< std::pair<double,double> >& corners );

    /**
     * Return a list of region corners.
     * @return - a list of region corners.
     */
    std::vector<std::pair<double,double> > getCorners() const;

    /**
     * Return the information associated with this region.
     * @return - information about the region.
     */
    Carta::Lib::RegionInfo getInfo() const;

    /**
     * Return the name of the region.
     * @return - the name of the region.
     */
    QString getRegionName() const;

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
    Carta::Lib::RegionInfo::RegionType getRegionType() const;

    /**
     * Return a string representation of the region shape.
     * @return - a string representation of the type of region.
     */
    QString regionTypeToString( Carta::Lib::RegionInfo::RegionType regionType ) const;

    /**
     * Set a user-customized name for the region.
     * @param name - a user name for the region.
     * @return - an error message if the region name was not correctly set; an empty string
     *  otherwise.
     */
    QString setRegionName( const QString& name );

    /**
     * Set the type of region.
     * @param regionType - an enumerated region type.
     */
    void setRegionType( Carta::Lib::RegionInfo::RegionType regionType );

    /**
     * Returns a string representation of the region.
     * @return - a string representation of the region.
     */
    QString toString() const;

    virtual ~Region();

    const static QString CLASS_NAME;

private:

    static bool m_registered;


    /**
     * Construct a region.
     */
    Region( const QString& path, const QString& id );
    class Factory;

    const static QString REGION_TYPE;
    const static QString CORNERS;
    const static QString REGION_POLYGON;
    const static QString REGION_ELLIPSE;

    int _findRegionIndex( std::shared_ptr<Region> region ) const;

    /**
     * Return the region state as a string.
     * @return - the region state as a string.
     */
    QString _getStateString() const;

    void _initializeCallbacks();
    void _initializeState();

    /**
     * Restore the region state.
     * @param state - a string representation of the state to restore.
     */
    void _restoreState( const QString& state );

    Region( const Region& other);
    Region& operator=( const Region& other );

    bool m_regionNameSet;


};
}
}
