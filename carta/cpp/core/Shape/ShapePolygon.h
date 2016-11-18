/**
 * A polygonal shape.
 **/
#include "ShapeBase.h"
#include "CartaLib/VectorGraphics/VGList.h"

namespace vge = Carta::Lib::VectorGraphics::Entries;

namespace Carta {

namespace Lib {
namespace Regions {
class Polygon;
}
}

namespace Shape {

class ControlPointEditable;

class ShapePolygon : public ShapeBase
{
    CLASS_BOILERPLATE( ShapePolygon );

public:

    /**
     * Constructor.
     */
    ShapePolygon( );

    /**
     * Return the center of the bounding box containing the polygon.
     * @return - the center of the bounding box containing the polygon.
     */
    virtual QPointF getCenter() const;

    /**
     * Return the size of the bounding box containing the polygon.
     * @return - the size of the bounding box containing the polygon.
     */
    virtual QSizeF getSize() const;

    /**
     * Return the vector graphics for the shape.
     * @return - the shape vector graphics.
     */
    virtual Carta::Lib::VectorGraphics::VGList getVGList() const override;

    /**
     * Notification that a drag event has ended.
     * @param pt - the ending point of the drag.
     */
    virtual void handleDragDone( const QPointF & pt ) override;

    /**
     * Returns true if the polygon is closed; false otherwise.
     * @return - true if the polygon is closed; false, otherwise.
     */
    bool isClosed() const;

    /**
     * Returns true if the passed in point is a corner of the polygon; false otherwise.
     * @return- true if the point is a corner point of the polygon; false otherwise.
     */
    bool isCorner( const QPointF& pt ) const;

    /**
     * Returns whether or not the point is inside the shape or not.
     * @return - true if the point is inside the shape; false otherwise.
     */
    virtual bool isPointInside( const QPointF & pt ) const override;

    /**
     * Set a model for the shape.
     * @param json - the shape model (corner points, center, etc).
     */
    virtual void setModel( const QJsonObject& json ) override;

protected:

    virtual void _editShadow( const QPointF& pt ) override;

    virtual void _moveShadow( const QPointF& pt ) override;

    virtual void _syncShadowToCPs() override;

private:

    void _controlPointCB( int index, bool final );

    std::shared_ptr<Carta::Lib::Regions::Polygon> m_polygonRegion;

    // the shadow polygon
    QPolygonF m_shadowPolygon;
};
}
}
