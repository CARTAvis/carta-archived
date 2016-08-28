/**
 * An elliptical shape.
 **/

#include "CartaLib/VectorGraphics/VGList.h"
#include "ShapeBase.h"

namespace vge = Carta::Lib::VectorGraphics::Entries;

namespace Carta {
namespace Lib {
namespace Regions {
	class Ellipse;
}
}
namespace Shape {

class ControlPoint;

class ShapeEllipse : public ShapeBase {

	CLASS_BOILERPLATE( ShapeEllipse );

public:

	/**
	 * Constructor.
	 * @param ellipseRegion - the elliptical model.
	 */
	ShapeEllipse(  std::shared_ptr<Carta::Lib::Regions::Ellipse> ellipseRegion );

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

	void setDragRect( const QRectF & r );

	virtual void editableChanged() override;

	void controlPointCB( bool movingCenter, bool final );

	std::shared_ptr<Carta::Lib::Regions::Ellipse> m_ellipseRegion;
	QPointF m_dragOffset;
	QRectF m_dragRect;
	bool m_inDragMode;

	std::shared_ptr<ControlPoint> m_centerCP;
	std::shared_ptr<ControlPoint> m_radiusYCP;
	std::shared_ptr<ControlPoint> m_radiusXCP;
};
}
}

