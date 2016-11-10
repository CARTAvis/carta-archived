/**
 * A point which can be dragged.
 **/

#pragma once
#include "ShapeBase.h"
#include "CartaLib/CartaLib.h"
#include <QPointF>

namespace vge = Carta::Lib::VectorGraphics::Entries;


namespace Carta {

namespace Shape {

class ControlPointEditable
    : public ShapeBase
{
    CLASS_BOILERPLATE( ControlPointEditable );

public:

    /**
     * Contructor.
     * @param cb - callback for updates when a drag is finished.
     */
    ControlPointEditable( std::function < void (bool) > cb );

    /**
     * Notification that the editable state of the shape has changed.
     */
    virtual void editModeChanged() override;

    /**
     * Return the location of the point.
     * @return - the location of the point.
     */
    QPointF getCenter() const;

    /**
     * Return the size of the point.
     * @return - the size of the point.
     */
    QSizeF getSize() const;

    /**
     * Returns the graphics for drawing the control point.
     * @return - the graphics for drawing the control point.
     */
    virtual Carta::Lib::VectorGraphics::VGList getVGList() const override;

    /**
     * Notification that a drag has started.
     * @param pt - the start point of the drag.
     */
    virtual void handleDragStart( const QPointF & pt ) override;

    /**
     * Notification of a drag in progress.
     * @param pt - the current point of the drag.
     */
    virtual void handleDrag( const QPointF & pt ) override;

    /**
     * Notification that a drag has ended.
     * @param pt - the end point of the drag.
     */
    virtual void handleDragDone( const QPointF & pt ) override;

    /**
     * Returns whether or not the point is inside the control point.
     * @param pt - the point to test.
     * @return - true if the point is inside this one; false, otherwise.
     */
    virtual bool isPointInside( const QPointF & pt ) const override;

    /**
     * Set the fill color of the point.
     * @param color - the fill color for the point.
     */
    void setFillColor( QColor color );

    /**
     * Set the underlying model for the region.
     * @param json - the region model.
     */
    virtual void setModel( const QJsonObject& json ) override;

    /**
     * Set the position of the point.
     * @param pt - the position of the point.
     */
    void setPosition( const QPointF & pt );

    virtual ~ControlPointEditable();

protected:
    virtual void _editShadow( const QPointF& pt ) override;
    virtual void _moveShadow( const QPointF& pt ) override;
    virtual void _syncShadowToCPs() override;

private:
    std::function < void (bool) > m_cb;
    QPointF m_pos;
    QColor m_fillColor;
    double m_size = 11;
};

}
}
