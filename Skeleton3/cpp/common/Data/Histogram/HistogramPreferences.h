/***
 * Stores user preferences concerning what configuration settings are visible.
 *
 */

#pragma once

#include <State/ObjectManager.h>
#include <State/StateInterface.h>

namespace Carta {

namespace Data {

class HistogramPreferences : public Carta::State::CartaObject {

public:



    /**
     * Return a string representing the histogram state of a particular type.
     * @param sessionId - an identifier for a user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding histogram state.
     */
    virtual QString getStateString( const QString& sessionId,
            SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Show/hide the histogram bin count configuration settings.
     * @param visible true if the bin count configuration settings should be visible;
     *      false otherwise.
     */
    void setVisibleHistogramBinCount( bool visible );

    /**
     * Show/hide the histogram clip configuration settings.
     * @param visible true if the clip configuration settings should be visible;
     *      false otherwise.
     */
    void setVisibleHistogramClips( bool visible );

    /**
     * Show/hide the histogram cube configuration settings.
     * @param visible true if the cube configuration settings should be visible;
     *      false otherwise.
     */
    void setVisibleHistogramCube( bool visible );

    /**
     * Show/hide the histogram two-dimensional configuration settings.
     * @param visible true if the 2D configuration settings should be visible;
     *      false otherwise.
     */
    void setVisibleHistogram2D( bool visible );

    /**
     * Show/hide the histogram range configuration settings.
     * @param visible true if the range configuration settings should be visible;
     *      false otherwise.
     */
    void setVisibleHistogramRange( bool visible );

    /**
     * Show/hide the histogram display configuration settings.
     * @param visible true if the display configuration settings should be visible;
     *      false otherwise.
     */
    void setVisibleHistogramDisplay( bool visible );


    const static QString CLASS_NAME;

    virtual ~HistogramPreferences();

private:
    void _initializeDefaultState();
    void _initializeCallbacks();

    class Factory;
    static bool m_registered;

    bool _processParams( const QString& params, bool* value ) const;
    void _setVisibility( const QString& key, bool visible );

    HistogramPreferences( const QString& path, const QString& id);

    const static QString PREFERENCES;
    const static QString HIST_BIN_COUNT;
    const static QString HIST_CLIPS;
    const static QString HIST_CUBE;
    const static QString HIST_2D;
    const static QString HIST_RANGE;
    const static QString HIST_DISPLAY;
    const static QString VISIBLE;


	HistogramPreferences( const HistogramPreferences& other);
	HistogramPreferences operator=( const HistogramPreferences& other );
};
}
}
