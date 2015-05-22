/***
 * A region of a display.
 */

#pragma once

#include "../State/StateInterface.h"
#include "../State/ObjectManager.h"

namespace Carta {

namespace Data {

class Region : public Carta::State::CartaObject {

public:
    virtual ~Region();
    /**
     * Factory for making regions of various types.
     * @param typeStr a QString containing the class name of the region to make.
     * @return the unique path of the region produced.
     */
    static QString makeRegion( const QString& typeStr );

    /**
     * Returns a string identifying the region type.
     * @return an identifier for the region type.
     */
    virtual QString getType() const = 0;

protected:
    /**
     * Reset the state of this region.
     * @param params a QString describing the new internal state of this region.
     */
//    virtual void resetState( const QString params ) = 0;
    /**
     * Construct a region.
     */
    Region( const QString& className, const QString& path, const QString& id );
private:
    void _initializeCallbacks();

};
}
}
