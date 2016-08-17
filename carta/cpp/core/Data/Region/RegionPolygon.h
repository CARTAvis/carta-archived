/***
 * A polygonal region of an image.
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionPolygon : public Region {

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
    virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getInfo() const Q_DECL_OVERRIDE;

    /**
     * Returns a json representation of the region.
     * @return - a json representation of the region.
     */
    virtual QJsonObject toJSON() const Q_DECL_OVERRIDE;

    virtual ~RegionPolygon();

    const static QString CLASS_NAME;

protected:

    /**
     * Restore the region state.
     * @param state - a string representation of the state to restore.
     */
    virtual void _restoreState( const QString& state ) Q_DECL_OVERRIDE;

private:

    void _initializeState();

    static bool m_registered;

    /**
     * Construct a region.
     */
    RegionPolygon( const QString& path, const QString& id );
    class Factory;

    void _initializeCallbacks();

    RegionPolygon( const RegionPolygon& other);
    RegionPolygon& operator=( const RegionPolygon& other );

};
}
}
