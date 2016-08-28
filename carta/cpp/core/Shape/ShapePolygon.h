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
     * @param polygonRegion - the polygon model.
     */
    ShapePolygon( std::shared_ptr<Carta::Lib::Regions::Polygon> polygonRegion );

    /**
     * Return the vector graphics for the shape.
     * @return - the shape vector graphics.
     */
    virtual Carta::Lib::VectorGraphics::VGList getVGList() const override;

    /**
     * Returns whether or not the point is inside the shape or not.
     * @return - true if the point is inside the shape; false otherwise.
     */
    virtual bool isPointInside( const QPointF & pt ) const override;

    /**
     * Notification that a drag event has started on the shape.
     * @param pt - the starting point of the drag.
     */
    virtual void handleDragStart( const QPointF & pt ) override;

    /**
     * Notification of a drag as it progresses.
     * @param pt - the current location of the drag.
     */
    virtual void handleDrag( const QPointF & pt ) override;

    /**
     * Notification that a drag event has ended.
     * @param pt - the ending point of the drag.
     */
    virtual void handleDragDone( const QPointF & pt ) override;

private:

    virtual void editableChanged() override;

    void controlPointCB( int index, bool final );

    void _syncShadowToCPs();

    std::shared_ptr<Carta::Lib::Regions::Polygon> m_polygonRegion;
    bool m_inDragMode = false;

    // the shadow polygon
    QPolygonF m_shadowPolygon;
    QPointF m_dragStart;

    std::vector < std::shared_ptr<ControlPointEditable> > m_cps;
};
}
}
