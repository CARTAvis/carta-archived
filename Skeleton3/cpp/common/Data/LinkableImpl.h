/*
 * Manages image view links for an enclosing class.
 */

#ifndef LINKABLEIMPL_H_
#define LINKABLEIMPL_H_

#include "Data/Controller.h"
#include "Data/ILinkable.h"
#include <QString>

class StateInterface;

namespace Carta {

namespace Data {

class LinkableImpl : public ILinkable {

public:
    /**
     * Constructor.
     */
    LinkableImpl( StateInterface* state );

    /**
     * Clear all links.
     */
    void clear();

    /**
     * Return the number of links.
     * @return the link count.
     */
    int getLinkCount() const;

    /**
     * Returns the server-side id of the image view with the given index.
     * @param linkIndex a zero-based link index.
     * @return the server-side id of the image view with the given index or an
     *      emtpy string is there is no such image view.
     */
    QString getLinkId( int linkIndex ) const;

    /**
     * Returns a list of server-side ids for all linked image views.
     * @return the server-side ids of linked views.
     */
    QList<QString> getLinks() const;

    std::shared_ptr<Controller> searchLinks(const QString& link);

    virtual bool removeLink( const std::shared_ptr<Controller>& controller ) Q_DECL_OVERRIDE;;
    virtual bool addLink( const std::shared_ptr<Controller>& controller ) Q_DECL_OVERRIDE;;
    virtual ~LinkableImpl();

    static const QString LINK;

    /**
     * Initialize default state.
     */
    void _initializeState();

    /**
     * Update the state when links change.
     */
    void _adjustStateController();

    /**
     * Returns the index of a specific linked image view.
     * @param controller an image view.
     * @return a nonnegative index if the image view is linked; -1 otherwise.
     */
    int _getIndex( const std::shared_ptr<Controller>& controller );
    /// List of controllers managed by this animator.
    QList<std::shared_ptr<Controller> > m_controllers;
    StateInterface* m_state; //Used
};
}
}


#endif /* LINKABLEIMPL_H_ */
