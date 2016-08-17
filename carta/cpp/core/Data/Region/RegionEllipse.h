/***
 * A elliptical region of an image.
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionEllipse : public Region {

    friend class RegionEllipseFactory;
    friend class DataFactory;
    friend class Stack;

public:

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

    virtual ~RegionEllipse();

    const static QString CLASS_NAME;

protected:

    /**
     * Restore the region state.
     * @param state - a string representation of the state to restore.
     */
    virtual void _restoreState( const QString& state );

private:

    void _initializeState();

    static bool m_registered;

    /**
     * Construct a region.
     */
    RegionEllipse( const QString& path, const QString& id );
    class Factory;

    void _initializeCallbacks();

    RegionEllipse( const RegionEllipse& other);
    RegionEllipse& operator=( const RegionEllipse& other );
};
}
}
