/***
 * Manages image and region statistic state.
 *
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

class Statistics : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    QString addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    QString removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    virtual QList<QString> getLinks() const Q_DECL_OVERRIDE;


    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;


    virtual ~Statistics();
    const static QString CLASS_NAME;
    const static QString STATS;

private slots:
    void _updateStatistics( Controller* controller );

private:

    void _initializeDefaultState();


    static bool m_registered;

    //For right now we are supporting only one linked controller.
    bool m_controllerLinked;

    Statistics( const QString& path, const QString& id );
    class Factory;


    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;


	Statistics( const Statistics& other);
	Statistics operator=( const Statistics& other );
};
}
}
