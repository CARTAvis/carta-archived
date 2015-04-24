/***
 * Manages image statistics.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"


class ImageView;

namespace Carta {

namespace Data {

class LinkableImpl;
class Controller;


class Statistics : public QObject, public CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    virtual QString addLink( CartaObject* cartaObject ) Q_DECL_OVERRIDE;
    virtual QString removeLink( CartaObject* cartaObject ) Q_DECL_OVERRIDE;

    /**
     * Clear existing state.
     */
    void clear();

    /**
     * Return a list of identifiers for all objects that are controlled by this statistics view.
     * @return a list of identifiers for objects under the control of this statistics view.
     */
    QList<QString> getLinks() const;

    /**
     * Return a string representing the statistics state of a particular type.
     * @param type - the type of state needed.
     * @param sessionId - an identifier for the user's session.
     * @return a QString representing the corresponding statistics state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;
    virtual ~Statistics();
    const static QString CLASS_NAME;


private:
    void _initializeCallbacks();
    void _initializeState();

    static bool m_registered;

    Statistics( const QString& path, const QString& id );

    class Factory;

    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;



	Statistics( const Statistics& other);
	Statistics operator=( const Statistics& other );
};
}
}
