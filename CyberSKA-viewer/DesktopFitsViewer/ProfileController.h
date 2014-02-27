/**
 * This is a profile controller class. The main controller tells it the current
 * cursor location, and the controller then asks for profiles to be recomputed. When
 * they are recomputed, it sets the appropriate pureweb state with the new data.
 */

#ifndef ProfileController_H
#define ProfileController_H

#include <QObject>
#include <memory>
#include "PureWeb.h"
#include "FitsViewerLib/common.h"
#include "FitsViewerLib/RaiCache.h"
#include "FitsViewerLib/ProfileService.h"
#include "PureWeb.h"
#include "FitsViewerLib/RegionStatsService.h"
#include "QUProfileController.h"
#include "GlobalState.h"
#include "CoordinatesLib/WcsHero.h"

class Gauss1DFitterController;

class ProfileController : public QObject {
    Q_OBJECT
public:


    ProfileController( QObject * parent);

    /**
     * @brief Tells the controller where to get the data.
     * @param ccinit
     * @param ifloc
     * @param qfloc
     * @param ufloc
     */
    void setLocations( RaiLib::Cache::Controller::InitParameters ccinit,
                       const FitsFileLocation & ifloc,
                       const FitsFileLocation & qfloc,
                       const FitsFileLocation & ufloc );
    /**
     * @brief Tells the controller updated cursor location in 0-based fits coordinates
     */
    void setCursor( int x, int y, int z);

    /**
     * @brief The profile controller is interested to know when the visible data of the
     *        image changes so that it can set the zoom of some profiles (currently
     *        only the Ix/Iy profiles).
     * @param minx
     * @param maxx
     * @param miny
     * @param maxy
     */
    void setVisibleRectFits( double minx, double maxx, double miny, double maxy);

    /**
     * @brief Sets the date for corresponding profiles from the region stats results.
     * @param res
     */
    void setRegionStatsResults( RegionStatsService::ResultsRS & res);

signals:

    void requestFrame(int frame);

public slots:

    void updateWcs( WcsHero::Hero::SharedPtr wcsHero);

protected slots:

    // callbacks for profile extraction results
    void onPFManagerIx( ProfileService::Results);
    void onPFManagerIy( ProfileService::Results);
    void onPFManagerIz( ProfileService::Results);
    void onPFManagerQz( ProfileService::Results);
    void onPFManagerUz( ProfileService::Results);

    /// set all Z profiles to the same cursor
    void syncCursorInZprofiles(qint64 pos);
    void syncXzoomInZprofiles(quint64 min, quint64 max);
    void syncVisibleInQzChanged( double min, double max);
    void syncVisibleInUzChanged( double min, double max);
    void syncYzoomInQUzChanged( double min, double max);
    void syncAutoYinQUzChanged( bool val);
    /// set all Z profiles to the same auto X flag
    void syncAutoXinZchanged( bool val);
    /// requests given frame
    void rightClickZCB(int);

protected:


    /**
     * @brief The actual classes that will do the extractions (asynchronously)
     */
    ProfileService::Manager m_xProfileService, m_yProfileService, m_zProfileService,
        m_qProfileService, m_uProfileService;

    /**
     * @brief copy of cache initialization parameters. We need to give these
     *        to the extraction workers so that they can construct their own
     *        instances of cache controllers (because they run in separate threads)
     */
    RaiLib::Cache::Controller::InitParameters m_ccinit;

    /**
     * @brief Locations of the input files
     */
    FitsFileLocation m_ifLocation, m_qfLocation, m_ufLocation;
    //    FitsParser m_qparser, m_uparser;

    /**
     * @brief current stamp (we increment this every time a new position is set
     */
    qint64 m_stamp;

    /**
     * @brief we save the results of profile computation
     */
    ProfileService::Results m_IxResults, m_IyResults, m_IzResults, m_QzResults, m_UzResults;

    /**
     * @brief Callback for invokeG1Dfit command from PureWeb client
     */
    void invokeG1Dfit(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);

    /**
     * @brief Callback for relaseG1Dfit command from PureWeb client
     */
    void releaseG1Dfit(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);

    /**
     * @brief get an unused fitter (or create one)
     * @return
     */
    Gauss1DFitterController * getUnusedFitter();

    /// poniter to wcs hero
    WcsHero::Hero::SharedPtr m_wcsHero;

    // pointers to profile renderers
    Gauss1DFitterController * m_IxProfileRenderer;
    Gauss1DFitterController * m_IyProfileRenderer;
    Gauss1DFitterController * m_IzProfileRenderer;
    Gauss1DFitterController * m_QzProfileRenderer;
    Gauss1DFitterController * m_UzProfileRenderer;
    Gauss1DFitterController * m_PiProfileRenderer;
    Gauss1DFitterController * m_PaProfileRenderer;
    Gauss1DFitterController * m_RSnanProfileRenderer;
    Gauss1DFitterController * m_RSminProfileRenderer;
    Gauss1DFitterController * m_RSmaxProfileRenderer;
    Gauss1DFitterController * m_RSavgProfileRenderer;
    Gauss1DFitterController * m_RSsumProfileRenderer;
    Gauss1DFitterController * m_RSstdProfileRenderer;
    Gauss1DFitterController * m_RSbkgProfileRenderer;
    Gauss1DFitterController * m_RSsumbkgProfileRenderer;
    Gauss1DFitterController * m_RSmaxbkgProfileRenderer;
    std::shared_ptr< QUProfileController > m_QUprofileController;

    /// list of fitters
    std::vector< Gauss1DFitterController * > m_g1dfitters;

    /// boolean map indicating if the given fitter in m_g1dfitters is in use
    std::vector< bool > m_g1dfittersUsedFlags;

    /// fractional polarization mode
    bool m_fractionalPolarizationMode;

    /// list of all z profiles as QObjects
    std::vector<QObject *> m_zProfileObjects;

    /// list of all 1D z profiles as 1D profiles
    std::vector<Gauss1DFitterController *> m_1D_zProfiles;

    /**
     * save the last requested coordinates
     */
    int m_x, m_y, m_z;

    /**
     * @brief Will recalculate profiles that are functions of the extracted profiles
     */
    void recalculateByproducts();

    // global variables
    struct {
        GlobalState::StringVar::UniquePtr loadedImageType;
        GlobalState::BoolVar::UniquePtr fractionalPolarization;
    } m_vars;

    // guard the callbacks from global variables until everything is constructed
    bool m_ignoreVarCallbacks /*= true*/;

    // global variable callback (generic)
    void globalVariableCB( const QString & name);

    // helper function to help us create bindings for global variables
    template <class T>
    void binder(
            std::unique_ptr<GlobalState::TypedVariable<T> > & ptr,
            const QString & prefix,
            const QString & name)
    {
        ptr.reset( new GlobalState::TypedVariable<T>(prefix + name));
        std::function<void()> cb = std::bind(&ProfileController::globalVariableCB, this, name);
        ptr->addCallback( cb);
    }


    Q_DISABLE_COPY( ProfileController)
};

#endif // ProfileController_H
