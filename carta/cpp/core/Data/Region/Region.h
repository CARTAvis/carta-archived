/***
 * A region of an image.
 */

#pragma once

#include "CartaLib/Regions/IRegion.h"
#include "State/StateInterface.h"
#include "State/ObjectManager.h"

namespace Carta {

namespace Data {

class Region : public Carta::State::CartaObject {
    friend class RegionFactory;
    friend class DataFactory;
    friend class Stack;
public:


    /**
     * Return the information associated with this region.
     * @return - information about the region.
     */
    virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getInfo() const;

    /**
     * Return the name of the region.
     * @return - the name of the region.
     */
    QString getRegionName() const;

    /**
     * Return the case-sensitive region type corresponding to the given string representation.
     * @param regionTypeStr - a string representation of a region shape such as "ellipse".
     * @return - the corresponding case-sensitive region type or an empty string if there is no
     *      such type.
     */
    static QString getRegionType( const QString& regionTypeStr );

    /**
     * Return the type of region, which corresponds to its shape.
     * @return - the type of region.
     */
    QString getRegionType() const;

    /**
     * Set a user-customized name for the region.
     * @param name - a user name for the region.
     * @return - an error message if the region name was not correctly set; an empty string
     *  otherwise.
     */
    QString setRegionName( const QString& name );

    /**
     * Returns a representation of the region as a json object.
     * @return - a representation of the region as a json objeect.
     */
    virtual QJsonObject toJSON() const;

    virtual ~Region();

protected:

    /**
     * Restore the region state.
     * @param state - a string representation of the state to restore.
     */
    virtual void _restoreState( const QString& state );

    /**
     * Construct a region.
     */
    Region( const QString& className, const QString& path, const QString& id );

    const static QString REGION_TYPE;
    bool m_regionNameSet;

private:

    int _findRegionIndex( std::shared_ptr<Region> region ) const;

    /**
     * Return the region state as a string.
     * @return - the region state as a string.
     */
    QString _getStateString() const;

    void _initializeCallbacks();
    void _initializeState();

    Region( const Region& other);
    Region& operator=( const Region& other );
};
}
}
