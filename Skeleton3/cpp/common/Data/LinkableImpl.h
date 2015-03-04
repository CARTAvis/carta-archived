/*
 * Manages image view links for an enclosing class.
 */

#ifndef LINKABLEIMPL_H_
#define LINKABLEIMPL_H_

#include "State/ObjectManager.h"
#include <QString>

class StateInterface;

namespace Carta {

namespace Data {

class LinkableImpl {

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

    CartaObject* getLink( int index ) const;

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
    QList<QString> getLinkIds() const;

    CartaObject* searchLinks(const QString& link);

    bool removeLink( CartaObject* cartaObj );
    bool addLink( CartaObject* cartaObj );
    virtual ~LinkableImpl();
private:

    const static QString LINK;

    /**
     * Initialize default state.
     */
    void _initializeState();

    /**
     * Update the state when links change.
     */
    void _adjustState();

    /**
     * Returns the index of a specific linked image view.
     * @param cartaObj an image view.
     * @return a nonnegative index if the image view is linked; -1 otherwise.
     */
    int _getIndex( CartaObject* cartaObj );
    /// List of cartaObjs managed by this animator.
    QList<CartaObject* > m_cartaObjs;
    StateInterface* m_state; //Used

    LinkableImpl( const LinkableImpl& other);
    LinkableImpl operator=( const LinkableImpl& other );
};

}
}


#endif /* LINKABLEIMPL_H_ */
