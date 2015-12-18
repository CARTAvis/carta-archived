/***
 * A rectangular region.
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionPolygon : public Region {

public:

    /**
     * Return the type of region, which corresponds to its shape.
     * @return - the RegionType.
     */
    virtual Carta::Lib::RegionInfo::RegionType getType() const Q_DECL_OVERRIDE;

    /**
     * Return a string representation of the region shape.
     * @return - a string representation of the type of region.
     */
    virtual QString getTypeString() const Q_DECL_OVERRIDE;
    virtual ~RegionPolygon();
    const static QString CLASS_NAME;

//protected:
    /**
     * Resets the internal state of this rectangle based on the information passed in.
     * @param params a QString describing the internal rectangle state.
     */
    //virtual void resetStateData( const QString & params ) override;

private:
    void _initializeState();

    /**
     * Constructor.
     * @param the base path for state identification.
     * @param id the particular id for this object.
     */
    RegionPolygon(const QString& path, const QString& id );

    class Factory : public Carta::State::CartaObjectFactory {

    public:

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new RegionPolygon (path, id);
        }
    };

    static bool m_registered;


};
}
}
