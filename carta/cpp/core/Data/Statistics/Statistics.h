/***
 * Manages image and region statistic state.
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

class Statistics : public QObject, public Carta::State::CartaObject, public ILinkable {

    Q_OBJECT

public:

    //ILinkable
    QString addLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    QString removeLink( CartaObject* cartaObject) Q_DECL_OVERRIDE;
    virtual QList<QString> getLinks() const Q_DECL_OVERRIDE;

    /**
     * Return a string representation of the statistics state.
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
     * Move a statistics up or down in the view.
     * @param fromIndex - the current index of the statistic in the display.
     * @param toIndex - the new location of the statistic in the display.
     * @param statType - an identifier for the type of statistics (image, region, etc).
     */
    QString moveStat( int fromIndex, int toIndex, const QString& statType );

    /**
     * Reset the state of statistics.
     * @param state - the new statistics state.
     */
    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

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

    /**
     * Show/hide a particular statistic.
     * @param showState - true to show the statistic; false otherwise.
     * @param statName - an identifier for the statistic.
     * @param statType - the type of statistic (image, region, etc).
     */
    QString setStatVisible( bool showStat, const QString& statName,
            const QString& statType );

    virtual ~Statistics();
    const static QString CLASS_NAME;


private slots:
    /**
     * Recompute the statistics.
     * @param controller - the controller to use for statistics generation.
     */
    void _updateStatistics( Controller* controller, Carta::Lib::AxisInfo::KnownType type = Carta::Lib::AxisInfo::KnownType::SPECTRAL );

private:
    const static QString FROM;
    const static QString LABEL;
    const static QString SELECTED_INDEX;
    const static QString SHOW_STATS_IMAGE;
    const static QString SHOW_STATS_REGION;
    const static QString STATS;
    const static QString STATS_IMAGE;
    const static QString STATS_REGION;
    const static QString TO;

    void _clearLinks();

    QString _getPreferencesId() const;
    QString _getStatType( const QString& typeStr ) const;

    void _initializeCallbacks();
    void _initializeDefaultState();
    void _initializeLabel( const QString& arrayName, int arrayIndex, const QString& label, bool visible);


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
