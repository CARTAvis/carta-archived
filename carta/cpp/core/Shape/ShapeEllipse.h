/**
 * An elliptical shape.
 **/
#include "ShapeBase.h"
#include "CartaLib/VectorGraphics/VGList.h"

namespace vge = Carta::Lib::VectorGraphics::Entries;

namespace Carta {

namespace Lib {
namespace Regions {
class Ellipse;
}
}

namespace Shape {

class ControlPointEditable;

class ShapeEllipse : public ShapeBase {


   CLASS_BOILERPLATE( ShapeEllipse );

public:

    /**
     * Constructor.
     */
    ShapeEllipse( );

    /**
     * Return the center of the ellipse.
     * @return - the center of the ellipse.
     */
    virtual QPointF getCenter() const;

    /**
     * Return the size of the bounding box containing the ellipse.
     * @return - the size of the bounding box containing the ellipse.
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

    void _controlPointCB( int index, bool final );

    virtual void _moveShadow( const QPointF& pt ) override;

    virtual void _syncShadowToCPs() override;

    void _updateEllipseFromShadow();

    virtual void _editShadow( const QPointF & pt ) override;

    std::shared_ptr<Carta::Lib::Regions::Ellipse> m_ellipseRegion;


    const int CORNER_COUNT = 4;

    // the shadow rectangle
    QRectF m_shadowRect;



};
}
}
