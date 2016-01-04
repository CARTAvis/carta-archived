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
class Settings;

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

    /**
     * Set whether or not to show image statistics.
     * @param showStats - true if image statistics should be shown; false otherwise.
     */
    void setShowStatsImage( bool showStats );

    /**
     * Set whether or not to show region statistics.
     * @param showStats - true if region statistics should be shown; false otherwise.
     */
    void setShowStatsRegion( bool showStats );

    virtual ~Statistics();
    const static QString CLASS_NAME;
    const static QString SELECTED_INDEX;
    const static QString SHOW_STATS_IMAGE;
    const static QString SHOW_STATS_REGION;
    const static QString STATS;

private slots:
    void _updateStatistics( Controller* controller );

private:

    QString _getPreferencesId() const;

    void _initializeCallbacks();
    void _initializeDefaultState();


    static bool m_registered;

    //For right now we are supporting only one linked controller.
    bool m_controllerLinked;

    Statistics( const QString& path, const QString& id );
    class Factory;


    //Link management
    std::unique_ptr<LinkableImpl> m_linkImpl;

    //Preference settings
    std::unique_ptr<Settings> m_settings;


    Carta::State::StateInterface m_stateData;

	Statistics( const Statistics& other);
	Statistics operator=( const Statistics& other );
};
}
}
