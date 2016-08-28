/***
 * A elliptical region of an image.
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionEllipse : public Region {

    friend class DataFactory;

public:

    /**
     * Return the model associated with this region.
     * @return - a model for the region.
     */
    virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getModel() const Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the point is inside the shape or not.
     * @return - true if the point is inside the shape; false otherwise.
     */
    virtual bool isPointInside( const QPointF & pt ) const;

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
