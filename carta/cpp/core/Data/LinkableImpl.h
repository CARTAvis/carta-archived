/*
 * Manages image view links for an enclosing class.
 */

#ifndef LINKABLEIMPL_H_
#define LINKABLEIMPL_H_

#include "core/State/ObjectManager.h"
#include <QString>

class StateInterface;

namespace Carta {

namespace Data {

class LinkableImpl {

public:
    /**
     * Constructor.
     * @param parentPath - server-side id of the link source.
     */
    LinkableImpl( const QString& parentPath );

    /**
     * Clear all links.
     */
    void clear();

    /**
     * Return the number of links.
     * @return the link count.
     */
    int getLinkCount() const;

    Carta::State::CartaObject* getLink( int index ) const;

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

    /**
     * Return a string representing the link destinations that have been added.
     * @param index - the index of the parent.
     * @param typeStr - the object type of the parent.
     * @return a QString representing the corresponding linkages.
     */
    QString getStateString( int index, const QString& typeStr  ) const;

    Carta::State::CartaObject* searchLinks(const QString& link);
    void refreshState();
    bool removeLink( Carta::State::CartaObject* cartaObj );
    bool addLink( Carta::State::CartaObject* cartaObj );
    virtual ~LinkableImpl();

    const static QString LINK;
    const static QString PARENT_ID;

private:

    /**
     * Initialize default state.
     * @param parentPath - the server side id of the parent object for which the links are
     *      being maintained.
     */
    void _initializeState( const QString& parentPath);

    /**
     * Update the state when links change.
     */
    void _adjustState();

    /**
     * Returns the index of a specific linked image view.
     * @param cartaObj an image view.
     * @return a nonnegative index if the image view is linked; -1 otherwise.
     */
    int _getIndex( Carta::State::CartaObject* cartaObj );

    /// List of cartaObjs managed by this animator.
    QList<Carta::State::CartaObject* > m_cartaObjs;
    Carta::State::StateInterface m_state;

    LinkableImpl( const LinkableImpl& other);
    LinkableImpl& operator=( const LinkableImpl& other );
};

}
}


#endif /* LINKABLEIMPL_H_ */
