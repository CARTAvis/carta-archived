/**
 * A point shape.
 **/
#include "ShapeBase.h"
#include "CartaLib/VectorGraphics/VGList.h"

namespace vge = Carta::Lib::VectorGraphics::Entries;

namespace Carta {

namespace Lib {
namespace Regions {
class Point;
}
}

namespace Shape {

class ControlPointEditable;

class ShapePoint : public ShapeBase
{
    CLASS_BOILERPLATE( ShapePoint );

public:

    /**
     * Constructor.
     */
    ShapePoint( );

    /**
     * Return the location of the point.
     * @return - the location of the point.
     */
    virtual QPointF getCenter() const;

    /**
     * Return the size of the outline box for the point.
     * @return - the size of the outline box for the point.
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

    virtual void _editShadow( const QPointF& pt );

    virtual void _moveShadow( const QPointF& pt );

    virtual void _syncShadowToCPs() override;

    std::shared_ptr<Carta::Lib::Regions::Point> m_pointRegion;

    // the shadow rectangle
    QRectF m_shadowRect;
};
}
}
