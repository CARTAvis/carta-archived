/***
 * Manages a zoom view of an image.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include <QObject>

namespace Carta {

namespace Data {

class Controller;
class LinkableImpl;
class DrawStackSynchronizer;

class ImageZoom : public QObject, public Carta::State::CartaObject,  public ILinkable  {

    Q_OBJECT

public:

    //ILinkable
    virtual QList<QString> getLinks()  const Q_DECL_OVERRIDE;

    /**
     * Adds a link to this animator.
     * @param cartaObject the link that will be managed.
     * @return an error message if there was a problem adding the link; otherwise an empty string.
     */
    virtual QString addLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;

    /**
     * Removes a link to this animator.
     * @param cartaObject the link to remove.
     * @return an error message if the link could not be removed; an empty string otherwise.
     */
    virtual QString removeLink( Carta::State::CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    virtual ~ImageZoom();

    const static QString CLASS_NAME;

signals:

    /**
     * Notification that the mouse has moved on the plot.
     * @param x - the x-coordinate of the plot point.
     * @param y - the y-coordinate of the plot point.
     */
    void cursorMove( double x, double y );

private slots:

    void _viewResize();

private:

    class Factory;
    ImageZoom( const QString& path, const QString& id );
    Controller* _getControllerSelected() const;
    void _initializeDefaultState();
    void _initializeCallbacks();

    void _refreshView();

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;

    std::shared_ptr<DrawStackSynchronizer> m_zoomDraw;

    static bool m_registered;
    ImageZoom( const ImageZoom& other);
    ImageZoom& operator=( const ImageZoom& other );
};
}
}
