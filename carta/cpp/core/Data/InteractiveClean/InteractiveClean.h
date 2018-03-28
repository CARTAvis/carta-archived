/***
 * Manages interactive clean state
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ILinkable.h"
#include "CartaLib/AxisInfo.h"

#include <QObject>


namespace Carta {


namespace Data {

class Controller;
class LinkableImpl;
class Settings;

class InteractiveClean : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    QString addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    QString removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    virtual QList<QString> getLinks() const Q_DECL_OVERRIDE;

    /**
      Read the clean parameters from CASA clean process
     */
    QString getInitialParameters( );

    /**
     * Return a string representation of the interactive clean state.
     * @param sessionId - an identifier for a user session.
     * @param type - the type of state requested.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const Q_DECL_OVERRIDE;

    /**
     * Reset the state of interactive clean.
     * @param state - the new interactive clean state.
     */
    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

    virtual ~InteractiveClean();
    const static QString CLASS_NAME;


private slots:
    /**
     * Recompute the interactive clean.
     * @param controller - the controller to use for interactive clean generation.
     */
    void _updateInteractiveClean( QString cleanParameters );

    void _readInitialParameters();

private:
    const static QString PARAMETERS;
    const static QString SELECTED_INDEX;

    void _clearLinks();

    QString _getPreferencesId() const;
    QString _getStatType( const QString& typeStr ) const;

    void _initializeCallbacks();
    void _initializeDefaultState();
    void _initializeLabel( const QString& arrayName, int arrayIndex, const QString& label, bool visible);


    static bool m_registered;

    //For right now we are supporting only one linked controller.
    bool m_controllerLinked;

    InteractiveClean( const QString& path, const QString& id );
    class Factory;


    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Preference settings
    std::unique_ptr<Settings> m_settings;


    Carta::State::StateInterface m_stateData;

	InteractiveClean( const InteractiveClean& other);
	InteractiveClean& operator=( const InteractiveClean& other );
};
}
}
