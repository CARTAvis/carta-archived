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
    friend class Stack;
public:

    /**
     * Add corners to the region.
     * @param corners - a list of corners to add to the region.
     */
    void addCorners( const std::vector< std::pair<double,double> >& corners );

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
    Carta::Lib::RegionInfo::RegionType getRegionType() const;

    /**
     * Return a string representation of the region shape.
     * @return - a string representation of the type of region.
     */
    QString getTypeString() const;


    /**
     * Set the type of region.
     * @param regionType - an enumerated region type.
     */
    void setRegionType( Carta::Lib::RegionInfo::RegionType regionType );



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

    const static QString XCOORD;
    const static QString YCOORD;
    void _initializeCallbacks();
    void _initializeState();

    /**
     * Return the region state as a string.
     * @return - the region state as a string.
     */
    QString _getStateString() const;

    /**
     * Returns true if the user id of this region starts with the
     * id passed in; false otherwise.
     * @param id - the user id of a region.
     * @return - true if the id matches this region's user id; false otherwise.
     */
    bool _isMatch( const QString& id ) const;

    /**
     * Restore the region state.
     * @param state - a string representation of the state to restore.
     */
    void _restoreState( const QString& state );

    //The user id for a region loaded from a file will be the file name plus an
    //index, if there is more than one region loaded from the file.  If the region
    // is created graphically, the id will be just an index.
    void _setUserId( const QString& fileName, int index );

    Region( const Region& other);
    Region& operator=( const Region& other );

};
}
}
