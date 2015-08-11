/***
 * A rectangular region..
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionRectangle : public Region {

public:

    virtual ~RegionRectangle();
    const static QString CLASS_NAME;
protected:
    /**
     * Resets the internal state of this rectangle based on the information passed in.
     * @param params a QString describing the internal rectangle state.
     */
    virtual void resetStateData( const QString & params ) override;

    /**
     * Returns a string identifying the region type.
     * @return an identifier for the region type.
    */
    //virtual QString getType() const;

private:
    void _initializeState();
    void _resetCoord( const QString& name, const QString& value );
    /**
     * Constructor.
     * @param the base path for state identification.
     * @param id the particular id for this object.
     */
    RegionRectangle(const QString& path, const QString& id );

    class Factory : public Carta::State::CartaObjectFactory {

    public:

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new RegionRectangle (path, id);
        }
    };

    static bool m_registered;

    const QString TOP_LEFT_X;
    const QString TOP_LEFT_Y;
    const QString BOTTOM_RIGHT_X;
    const QString BOTTOM_RIGHT_Y;

};
}
}
