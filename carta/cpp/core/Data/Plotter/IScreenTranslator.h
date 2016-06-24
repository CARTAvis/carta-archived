/*
 * Interface implemented by classes that can translate between data and
 * screen coordinates.
 */

#pragma once

namespace Carta {

namespace Data {

class IScreenTranslator {
public:

    /**
     * Returns the screen point corresponding to the data point.
     * @param dataPoint - the point in world units.
     * @param valid - set to false if there is no corresponding pixel coordinate.
     * @return - the pixel point corresponding to the data point.
     */
    virtual QPointF getScreenPoint( const QPointF& dataPoint, bool* valid ) const = 0;


};
}
}
