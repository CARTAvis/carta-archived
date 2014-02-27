#include "ProfileController.h"
#include "Gauss1DFitterController.h"

#include <cmath>
#include "CoordinatesLib/UnitAnalyzer.h"



class VerticalLabeler : public Plot2dLabelers::BasicLabeler
{

public:

    typedef VerticalLabeler Me;
    typedef std::shared_ptr< Me > SharedPtr;

    VerticalLabeler( const QString & caption, const QString & unit)
        : Plot2dLabelers::BasicLabeler()
    {
        m_scale = 1.0;
        m_caption = caption;
        m_scaledUnit = unit;
        m_originalUnit = unit;

        m_au = UnitAnalyzer::analyze( unit);
        m_scaledUnit = m_au-> scaledUnit();
        m_scale = m_au-> scaleFactor();
    }

    // BasicLabeler interface
protected:
    virtual void zoomWasSet() CPP11_OVERRIDE {
//        m_scaledUnit = m_originalUnit;
//        getBestScaleForUnit( m_scaledUnit, m_scale, m_min, m_max);
        m_au-> computeScale( m_min, m_max);
        m_scale = m_au-> scaleFactor();
        m_scaledUnit = m_au-> scaledUnit();
    }

    virtual QString makeLabel1( double x) CPP11_OVERRIDE {
        x *= m_scale;
        return QString::number( x);
    }

    virtual QString doFormat(double val) CPP11_OVERRIDE {
        return QString::number( val) + m_originalUnit;
    }

    virtual QString doGetCaption1() CPP11_OVERRIDE {
        QString res = m_caption;
        if( ! m_scaledUnit.isEmpty()) {
            res += "[" + m_scaledUnit + "]";
        }
        return res;
    }

    AnalyzedUnit::SharedPtr m_au;

protected:

    QString m_originalUnit, m_caption;
    QString m_scaledUnit;
    double m_scale;
};


class HorizontalLabeler : public Plot2dLabelers::BasicLabeler
{

public:

    typedef HorizontalLabeler Me;
    typedef std::shared_ptr< Me > SharedPtr;

    HorizontalLabeler( WcsHero::Hero::SharedPtr hero, const std::vector<double> & cursor, size_t axis)
        : Plot2dLabelers::BasicLabeler()
    {

        if( cursor.size() <= axis) {
            LTHROW("curosr.size <= which");
        }

        setOffset( 1.0); // 1-based coordinates
        m_hero = hero;
        m_cursor = cursor;
        m_axis = axis;
        m_wcsGood = hero && hero-> axesInfos().size() > m_axis;
        m_scale2 = 1.0;
        if( m_wcsGood) {
            setDual(true);
            const std::vector<WcsHero::AxisInfo> & ai = hero-> axesInfos();
            m_caption2 = ai[m_axis].label;
            m_unit2 = ai[m_axis].unit;
//            m_scaledUnit2 = m_unit2;
            m_au = UnitAnalyzer::analyze( m_unit2);
            m_scaledUnit2 = m_au-> scaledUnit();
            m_scale2 = m_au-> scaleFactor();
        }
        else {
            setDual( false);
        }
    }

    // BasicLabeler interface
protected:

    double toWorld( double val)
    {
        if( ! m_wcsGood) {
            return val;
        }
        m_cursor[ m_axis] = val;
        auto world = m_hero-> fits2world( m_cursor);
        return world[m_axis];
    }

    virtual void zoomWasSet() CPP11_OVERRIDE
    {
        m_scale2 = 1.0;
        m_scaledUnit2 = m_unit2;
        // convert min/max to world coordinates
        double wmin = toWorld( m_min);
        double wmax = toWorld( m_max);
        // figure out scaled unit
//        getBestScaleForUnit( m_scaledUnit2, m_scale2, wmin, wmax);
        m_au-> computeScale( wmin, wmax);
        m_scaledUnit2 = m_au-> scaledUnit();
        m_scale2 = m_au-> scaleFactor();
    }

    virtual QString makeLabel2(double val) CPP11_OVERRIDE
    {
        if( ! m_wcsGood) {
            return QString();
        }
        // convert value to world
        double wVal = toWorld( val);
        // apply unit scaling
        wVal *= m_scale2;
        // format the world value
        QString res = m_hero-> formatWorldValue( wVal, m_axis);
        return res;
    }

    virtual FormattedDistance doDistance(double val1, double val2)
    {
        if( ! m_wcsGood) {
            return BasicLabeler::doDistance( val1, val2);
        }
        auto c1 = m_cursor;
        c1[m_axis] = val1;
        auto c2 = m_cursor;
        c2[m_axis] = val2;
        double dist = m_hero-> distance( c1, c2);
        dist *= m_scale2;

        FormattedDistance res;
        // special case for longitude/latitude axes
        if( m_hero-> axesInfos()[m_axis].isSky()) {
            res.v2.value.plain = UnitAlgorithms::formatAngle( dist);
        } else {
            res.v2.value.plain = m_hero-> formatWorldValue( dist, m_axis);
        }
        res.v2.unit.plain = m_scaledUnit2;
        res.v2.value.html = res.v2.value.plain;
        res.v2.unit.html = res.v2.unit.plain;
        res.v1.value.plain = QString::number( fabs( val1 - val2));
        res.v1.value.html = res.v1.value.plain;

        return res;
    }


    virtual QString doFormat(double val) CPP11_OVERRIDE
    {
        if( isDual()) {
            return QString::number(val) + "pix/" + makeLabel2(val) + m_scaledUnit2;
        }
        else {
            return QString::number(val);
        }
    }
    virtual QString doGetCaption2() CPP11_OVERRIDE
    {
        QString res;
        if( isDual()) {
            res = m_caption2;
            if( ! m_scaledUnit2.isEmpty()) {
                res += "[" + m_scaledUnit2 + "]";
            }
        }
        return res;
    }

    AnalyzedUnit::SharedPtr m_au;


    std::vector<double> m_cursor;
    size_t m_axis;
    WcsHero::Hero::SharedPtr m_hero;
    double m_scale2;
    QString m_scaledUnit2;
    QString m_caption2;
    QString m_unit2;
    bool m_wcsGood;

};




/**
 * @brief maximumum number of gaussian 1d fitters
 */
// TODO: make this configurable
static const quint64 MAX_G1D_FITTERS = 5;

ProfileController::ProfileController(QObject *parent)
    : QObject( parent)
{
    connect( & m_xProfileService, SIGNAL(done(ProfileService::Results)),
             this, SLOT(onPFManagerIx(ProfileService::Results)));
    connect( & m_xProfileService, SIGNAL(progress(ProfileService::Results)),
             this, SLOT(onPFManagerIx(ProfileService::Results)));

    connect( & m_yProfileService, SIGNAL(done(ProfileService::Results)),
             this, SLOT(onPFManagerIy(ProfileService::Results)));
    connect( & m_yProfileService, SIGNAL(progress(ProfileService::Results)),
             this, SLOT(onPFManagerIy(ProfileService::Results)));

    connect( & m_zProfileService, SIGNAL(done(ProfileService::Results)),
             this, SLOT(onPFManagerIz(ProfileService::Results)));
    connect( & m_zProfileService, SIGNAL(progress(ProfileService::Results)),
             this, SLOT(onPFManagerIz(ProfileService::Results)));

    connect( & m_qProfileService, SIGNAL(done(ProfileService::Results)),
             this, SLOT(onPFManagerQz(ProfileService::Results)));
    connect( & m_qProfileService, SIGNAL(progress(ProfileService::Results)),
             this, SLOT(onPFManagerQz(ProfileService::Results)));

    connect( & m_uProfileService, SIGNAL(done(ProfileService::Results)),
             this, SLOT(onPFManagerUz(ProfileService::Results)));
    connect( & m_uProfileService, SIGNAL(progress(ProfileService::Results)),
             this, SLOT(onPFManagerUz(ProfileService::Results)));

    m_stamp = 0;
    m_x = m_y = m_z = -1;
    m_fractionalPolarizationMode = false;

    // create individual profile controllers
    m_IxProfileRenderer = new Gauss1DFitterController(
                this, "ix", "Ix profile", false);
    m_IyProfileRenderer = new Gauss1DFitterController(
                this, "iy", "Iy profile", false);
    m_IzProfileRenderer = new Gauss1DFitterController(
                this, "iz", "Iz profile", false);
    m_QzProfileRenderer = new Gauss1DFitterController(
                this, "qz", "Qz profile", false);
    m_UzProfileRenderer = new Gauss1DFitterController(
                this, "uz", "Uz profile", false);
    m_PiProfileRenderer = new Gauss1DFitterController(
                this, "pi", "Polarized intensity", false);
    m_PaProfileRenderer = new Gauss1DFitterController(
                this, "pa", "Position angle", false);
    m_RSnanProfileRenderer = new Gauss1DFitterController(
                this, "rs-nan", "RS: NaN pixels", false);
    m_RSminProfileRenderer = new Gauss1DFitterController(
                this, "rs-min", "RS: Min", false);
    m_RSmaxProfileRenderer = new Gauss1DFitterController(
                this, "rs-max", "RS: Max", false);
    m_RSavgProfileRenderer = new Gauss1DFitterController(
                this, "rs-avg", "RS: Average", false);
    m_RSsumProfileRenderer = new Gauss1DFitterController(
                this, "rs-sum", "RS: Sum", false);
    m_RSstdProfileRenderer = new Gauss1DFitterController(
                this, "rs-std", "RS: Std. dev.", false);
    m_RSbkgProfileRenderer = new Gauss1DFitterController(
                this, "rs-bkg", "RS: Bkg. level", false);
    m_RSsumbkgProfileRenderer = new Gauss1DFitterController(
                this, "rs-sumbkg", "RS: Sum - bkg", false);
    m_RSmaxbkgProfileRenderer = new Gauss1DFitterController(
                this, "rs-maxbkg", "RS: Max - bkg", false);
    m_QUprofileController = std::make_shared<QUProfileController>( this, "1", "Q vs U");

    // list of all z profiles as QObjects
    m_zProfileObjects = std::vector< QObject *> {
                m_IzProfileRenderer,
                m_QzProfileRenderer,
                m_UzProfileRenderer,
                m_PiProfileRenderer,
                m_PaProfileRenderer,
                m_RSnanProfileRenderer,
                m_RSminProfileRenderer,
                m_RSmaxProfileRenderer,
                m_RSavgProfileRenderer,
                m_RSsumProfileRenderer,
                m_RSstdProfileRenderer,
                m_RSbkgProfileRenderer,
                m_RSsumbkgProfileRenderer,
                m_RSmaxbkgProfileRenderer,
                m_QUprofileController.get()
    };
    // list of all 1d z profiles as 1d profiles
    for( auto & zp : m_zProfileObjects) {
        auto d1 = dynamic_cast<Gauss1DFitterController *>( zp);
        if( d1 == nullptr) continue;
        m_1D_zProfiles.push_back( dynamic_cast<Gauss1DFitterController *>( d1));
    }


    // set up synchronization callbacks
    // ---------------------------------

    // sync qz/uz verticals in auto mode
    connect( m_QzProfileRenderer,
             SIGNAL(visibleRangeChanged(double,double)),
             SLOT(syncVisibleInQzChanged(double,double)));
    connect( m_UzProfileRenderer,
             SIGNAL(visibleRangeChanged(double,double)),
             SLOT(syncVisibleInUzChanged(double,double)));

    // sync qz/uz in manual mode
    connect( m_QzProfileRenderer,
             SIGNAL(yZoomChanged(double,double)),
             SLOT(syncYzoomInQUzChanged(double,double)));
    connect( m_UzProfileRenderer,
             SIGNAL(yZoomChanged(double,double)),
             SLOT(syncYzoomInQUzChanged(double,double)));

    // sync qz/uz auto mode
    connect( m_QzProfileRenderer,
             SIGNAL(autoYChanged(bool)),
             SLOT(syncAutoYinQUzChanged(bool)));
    connect( m_UzProfileRenderer,
             SIGNAL(autoYChanged(bool)),
             SLOT(syncAutoYinQUzChanged(bool)));

    // for all z-profiles (including qu):
    //   - sync cursor position
    //   - register right clicks on all z profiles
    // for 1d z-profiles only:
    //   - sync auto x range flag
    //   - sync x zoom range
    for( auto & zp : m_zProfileObjects) {
        connect( zp, SIGNAL(cursorChanged(qint64)), SLOT(syncCursorInZprofiles(qint64)));
        connect( zp, SIGNAL(rightClick(int)), SLOT(rightClickZCB(int)));
        if( dynamic_cast<Gauss1DFitterController *>( zp) == nullptr) continue;
        connect( zp, SIGNAL(autoXChanged(bool)), SLOT(syncAutoXinZchanged(bool)));
        connect( zp, SIGNAL(xZoomChanged(quint64,quint64)), SLOT(syncXzoomInZprofiles(quint64,quint64)));
    }

    // listen to invoke fit commands
    GetStateManager().CommandManager().AddUiHandler(
                "invokeG1Dfit",
                CSI::Bind(this, & ProfileController::invokeG1Dfit));
    // listen to relase fit commands
    GetStateManager().CommandManager().AddUiHandler(
                "releaseG1Dfit",
                CSI::Bind(this, & ProfileController::releaseG1Dfit));

    // listen to fractional polarization mode changes
//    GetStateManager().XmlStateManager().AddValueChangedHandler(
//                "/fractionalPolarization",
//                CSI::Bind(this, &ProfileController::fractionalPolarizationCB));

    // tell the clients the maximum number of fitters
    pwset( "/g1dFittersList/max", MAX_G1D_FITTERS);

    // create and bind global variables
    // ================================

    m_ignoreVarCallbacks = true;
    QString m_pwPrefix = "/Histogram/";
    binder( m_vars.loadedImageType, "/", "loadedImageType");
    binder( m_vars.fractionalPolarization, "/", "fractionalPolarization");
    m_ignoreVarCallbacks = false;

}

void ProfileController::setLocations(
        RaiLib::Cache::Controller::InitParameters ccinit,
        const FitsFileLocation &ifloc,
        const FitsFileLocation &qfloc,
        const FitsFileLocation &ufloc)
{
    m_ccinit = ccinit;
    m_ifLocation = ifloc;
    m_qfLocation = qfloc;
    m_ufLocation = ufloc;
    m_x = m_y = m_z = -1;

    // reset all profiles
    m_IxProfileRenderer-> setData();
    m_IyProfileRenderer-> setData();
    m_IzProfileRenderer-> setData();
    m_QzProfileRenderer-> setData();
    m_UzProfileRenderer-> setData();
    m_PiProfileRenderer-> setData();
    m_PaProfileRenderer-> setData();
    m_QUprofileController-> setData();
}

void ProfileController::setCursor(int x, int y, int z)
{
//    dbg(1) << "setImgPos " << x << " " << y << " " << z;

    // if this coordinates are the coordinates we are already working on right now,
    // do nothing at all
    if( x == m_x && y == m_y && z == m_z) return;

    // save the latest coordinates
    m_x = x;
    m_y = y;
    m_z = z;

    // update the stamp so that we can ignore old results
    m_stamp ++;

    // tell workers to start extracting profiles from the new positions
    ProfileService::InputParameters inp;
    inp.fitsLocation = m_qfLocation;
    inp.ccinit = m_ccinit;
    inp.position.push_back ( x);
    inp.position.push_back ( y);
    inp.position.push_back ( z);
    inp.direction = 2;
    inp.minPos = 0;
    inp.maxPos = -1;
    inp.stamp = m_stamp;
    if( ! m_qfLocation.getLocalFname().isNull())
        m_qProfileService.request ( inp);
    inp.fitsLocation = m_ufLocation;
    if( ! m_ufLocation.getLocalFname().isNull())
        m_uProfileService.request ( inp);

    if( ! m_ifLocation.getLocalFname().isNull()) {
        inp.fitsLocation = m_ifLocation;

        inp.direction = 0;
        inp.minPos = inp.maxPos = -1;
        m_xProfileService.request( inp);

        inp.direction = 1;
        inp.minPos = inp.maxPos = -1;
        m_yProfileService.request( inp);

        inp.direction = 2;
        inp.minPos = inp.maxPos = -1;
        m_zProfileService.request( inp);
    }

    // tell our profile renderers about the cursor update
    m_IxProfileRenderer-> setCursor1( m_x);
    m_IyProfileRenderer-> setCursor1( m_y);
    syncCursorInZprofiles( m_z);

    // TODO: hack to update labelers
    updateWcs( m_wcsHero);
}

// helper function to make it easier to create new label makers
//static Plot2dLabelers::LabelMakerFunc
//makeWcsLabelMaker(
//        const QVector<qint64> qcursor,
//        int which,
//        WcsHero::Hero::SharedPtr hero)
//{
//    std::vector<double> cursor;
//    for( auto x : qcursor) { cursor.push_back( x + 1); }
//    return [cursor, hero, which](double val) {
//        Plot2dLabelers::LabelEntry label;
//        label.txt1 = QString::number( val);
//        if( hero && which < int(cursor.size())) {
//            auto cursorCopy = cursor;
//            cursorCopy[which] = val;
//            QStringList fmtc = hero-> formatPixelCursor( cursorCopy);
//            if( fmtc.size() > which)
//                label.txt2 = fmtc[which];
//        }
//        return label;
//    };
//}

//class WcsLabelMaker {
//public:
//    Plot2dLabelers::LabelEntry operator ()( double val) {
//        Plot2dLabelers::LabelEntry label;
//        label.txt1 = QString::number(val);
//        return label;
//    }
//};

void ProfileController::onPFManagerIx(ProfileService::Results res)
{
    // save the results
    m_IxResults = res;

    // Occasionally we might receive profile for previous cursor location. If
    // this happens we just ignore the results. New profile will arrive shortly.
    if( res.input.stamp != m_stamp) {
        return;
    }

    Gauss1DFitterController * plot = m_IxProfileRenderer;
    std::vector<double> data( res.available());
    for( quint64 i = 0 ; i < res.available () ; i ++ ) {
        data[i] = res.rawData()[i];
    }
    plot-> setData( res.total(), data);

    // adjust the label maker of the horizontal plot labeler with the new cursor location
    // and then repaint the graph
//    plot-> horizontalLabeler()-> setLmf( makeWcsLabelMaker( res.input.position, 0, m_wcsHero));
}

void ProfileController::onPFManagerIy(ProfileService::Results res)
{
    // save the results
    m_IyResults = res;

    // Occasionally we might receive profile for previous cursor location. If
    // this happens we just ignore the results. New profile will arrive shortly.
    if( res.input.stamp != m_stamp) {
        return;
    }

    Gauss1DFitterController * plot = m_IyProfileRenderer;
    std::vector<double> data( res.available());
    for( quint64 i = 0 ; i < res.available () ; i ++ ) {
        data[i] = res.rawData()[i];
    }
    plot-> setData( res.total(), data);

    // adjust the label maker of the horizontal plot labeler with the new cursor location
    // and then repaint the graph
//    plot-> horizontalLabeler()-> setLmf( makeWcsLabelMaker( res.input.position, 1, m_wcsHero));

}

void ProfileController::onPFManagerIz(ProfileService::Results res)
{
    // save the results
    m_IzResults = res;

    // Occasionally we might receive profile for previous cursor location. If
    // this happens we just ignore the results. New profile will arrive shortly.
    if( res.input.stamp != m_stamp) {
        return;
    }

    Gauss1DFitterController * plot = m_IzProfileRenderer;
    std::vector<double> data( res.available());
    for( quint64 i = 0 ; i < res.available () ; i ++ ) {
        data[i] = res.rawData()[i];
    }
    plot-> setData( res.total(), data);
    
    // recalculate byproducts
    recalculateByproducts();
}

void ProfileController::onPFManagerQz(ProfileService::Results res)
{
    // save the results
    m_QzResults = res;

    // Occasionally we might receive profile for previous cursor location. If
    // this happens we just ignore the results. New profile will arrive shortly.
    if( res.input.stamp != m_stamp) {
        return;
    }

    /*
    Gauss1DFitterController * plot = m_QzProfileRenderer;
    std::vector<double> data( res.available());
    for( quint64 i = 0 ; i < res.available () ; i ++ ) {
        data[i] = res.rawData()[i];
    }
    plot-> setData( res.total(), data);
    */

    // update byproducts
    recalculateByproducts();
}

void ProfileController::onPFManagerUz(ProfileService::Results res)
{
    // save the results
    m_UzResults = res;

    // Occasionally we might receive profile for previous cursor location. If
    // this happens we just ignore the results. New profile will arrive shortly.
    if( res.input.stamp != m_stamp) {
        return;
    }

    /*
    Gauss1DFitterController * plot = m_UzProfileRenderer;
    std::vector<double> data( res.available());
    for( quint64 i = 0 ; i < res.available () ; i ++ ) {
        data[i] = res.rawData()[i];
    }
    plot-> setData( res.total(), data);
    */

    // update byproducts
    recalculateByproducts();
}

/**
 * @brief Called whenever one of the z-profile cursor changes. The other profiles
 *        then have the cursor set to this value
 * @param pos
 */
void ProfileController::syncCursorInZprofiles(qint64 pos)
{
    if( pos < 0) pos = m_z;

//    m_IzProfileRenderer->setCursor1( pos);
//    m_QzProfileRenderer->setCursor1( pos);
//    m_UzProfileRenderer->setCursor1( pos);
//    m_PiProfileRenderer->setCursor1( pos);
//    m_PaProfileRenderer->setCursor1( pos);

    for( Gauss1DFitterController * zp : m_1D_zProfiles) {
        zp-> setCursor1( pos);
    }

    m_QUprofileController->setCursor( pos);
}

void ProfileController::syncXzoomInZprofiles(quint64 min, quint64 max)
{
//    m_IzProfileRenderer-> setXZoom( min, max);
//    m_QzProfileRenderer-> setXZoom( min, max);
//    m_UzProfileRenderer-> setXZoom( min, max);
//    m_PiProfileRenderer-> setXZoom( min, max);
//    m_PaProfileRenderer-> setXZoom( min, max);

    for( auto zp : m_1D_zProfiles) {
        zp-> setXZoom( min, max);
    }
}

void ProfileController::syncVisibleInQzChanged(double min, double max)
{
    m_UzProfileRenderer->setYZoomAutoRange( min, max);
}

void ProfileController::syncVisibleInUzChanged(double min, double max)
{
    m_QzProfileRenderer->setYZoomAutoRange( min, max);
}

void ProfileController::syncYzoomInQUzChanged(double min, double max)
{
    m_QzProfileRenderer->setYZoom( min, max);
    m_UzProfileRenderer->setYZoom( min, max);
}

void ProfileController::syncAutoYinQUzChanged(bool val)
{
    m_QzProfileRenderer->setAutoY( val);
    m_UzProfileRenderer->setAutoY( val);
}

void ProfileController::syncAutoXinZchanged(bool val)
{
//    m_IzProfileRenderer->setAutoX( val);
//    m_QzProfileRenderer->setAutoX( val);
//    m_UzProfileRenderer->setAutoX( val);
//    m_PiProfileRenderer->setAutoX( val);
//    m_PaProfileRenderer->setAutoX( val);

    for( auto zp : m_1D_zProfiles) {
        zp-> setAutoX( val);
    }

}

void ProfileController::rightClickZCB(int z)
{
    emit requestFrame( z);
}

void ProfileController::invokeG1Dfit(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{
    // get an unused fitter
    Gauss1DFitterController * fitter = getUnusedFitter();
    if( fitter == nullptr) {
        pwset( "/warning/text",
               QString("You already have maximum number of fitters running (%1/%2).")
               .arg(m_g1dfitters.size()).arg(MAX_G1D_FITTERS));
        pwsetdc( "/warning/dc", 1);
        return;
    }

    QString sourceId = command["/id"].ValueOr<QString>( QString());

    dbg(1) << "Invoking g1d-fit for " << sourceId << "\n";

    Gauss1DFitterController * src = nullptr;
    if( sourceId == "ix") src = m_IxProfileRenderer;
    else if( sourceId == "iy") src = m_IyProfileRenderer;
    else if( sourceId == "iz") src = m_IzProfileRenderer;
    else if( sourceId == "qz") src = m_QzProfileRenderer;
    else if( sourceId == "uz") src = m_UzProfileRenderer;
    else if( sourceId == "pi") src = m_PiProfileRenderer;
    else if( sourceId == "pa") src = m_PaProfileRenderer;
    else if( sourceId == "rs-nan") src = m_RSnanProfileRenderer;
    else if( sourceId == "rs-min") src = m_RSminProfileRenderer;
    else if( sourceId == "rs-max") src = m_RSmaxProfileRenderer;
    else if( sourceId == "rs-avg") src = m_RSavgProfileRenderer;
    else if( sourceId == "rs-sum") src = m_RSsumProfileRenderer;
    else if( sourceId == "rs-std") src = m_RSstdProfileRenderer;
    else if( sourceId == "rs-bkg") src = m_RSbkgProfileRenderer;
    else if( sourceId == "rs-sumbkg") src = m_RSsumbkgProfileRenderer;
    else if( sourceId == "rs-maxbkg") src = m_RSmaxbkgProfileRenderer;
    if( src == nullptr) {
        dbg(1) << "src === nullptr!!!!\n";
        return;
    }

    fitter->setTitle( "Fit from " + src-> getTitle());

    // populate the fitter with the visible data from the profile
    if( src-> getData().size() > 0) {
        quint64 x1 = src->getXZoomMin();
        quint64 x2 = src->getXZoomMax();
        dbg(1) << " x1/x2 = " << x1 << " " << x2 << "\n";

        std::vector<double> data( x2 - x1 + 1);
        for( quint64 i = x1 ; i <= x2 ; i ++ ) {
            data[i - x1] = src->getData()[i];
        }
        dbg(1) << " data.size = " << data.size() << "\n";
        fitter-> setData( data.size(), data);
    }

    // the vertical labeler of the fitter will be the same as the vertical
    // labeler of the source plot but it'll be a copy
    // TODO: we are using a default copy constructor here, a little better
    // solution would be a clone() method of some sort?
    VerticalLabeler::SharedPtr oldVertLabeler = std::dynamic_pointer_cast<VerticalLabeler>(
                src-> verticalLabeler());
    if( oldVertLabeler) {
        VerticalLabeler::SharedPtr newVertLabeler = std::make_shared<VerticalLabeler>( * oldVertLabeler);
        fitter-> setVerticalLabeler( newVertLabeler);
    }

    // horizontal labeler will be the same as the source, but we need to offset
    // the value by the zoomMin factor. src-> horizontalLabeler()-> offset();
    // TODO: we are using a default copy constructor here, a little better
    // solution would be a clone() method of some sort?
    double newOffset = src-> horizontalLabeler()-> offset() + src-> getXZoomMin();
    HorizontalLabeler::SharedPtr oldHLabeler = std::dynamic_pointer_cast<HorizontalLabeler> (src-> horizontalLabeler());
    if( oldHLabeler) {
        HorizontalLabeler::SharedPtr newHorizontalLabeler = std::make_shared<HorizontalLabeler>( * oldHLabeler);
        newHorizontalLabeler-> setOffset( newOffset);
        fitter-> setHorizontalLabeler( newHorizontalLabeler);
    }

    // update the pureweb state with the active fitters
    for( size_t i = 0 ; i < MAX_G1D_FITTERS ; i ++ ) {
        bool flag = (i < m_g1dfitters.size()) ? m_g1dfittersUsedFlags[i] : false;
        pwset( QString("/g1dFittersList/x%1/active").arg( i), flag);
    }
    pwsetdc( "/g1dFittersList/dc", 1);

}

void ProfileController::releaseG1Dfit(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{
    QString id = command["/id"].ValueOr<QString>( QString());

    dbg(1) << "Release g1d-fit for " << id << "\n";

    bool foundIt = false;
    size_t ind = 0;
    for( size_t i = 0 ; i < m_g1dfitters.size() ; i ++ ) {
        if( m_g1dfitters[i]-> getId() == id) {
            ind = i;
            foundIt = true;
            break;
        }
    }
    if( ! foundIt) {
        dbg(1) << ConsoleColors::warning() << "Could not find g1df with id = " << id
               << ConsoleColors::resetln();
        return;
    }

    Gauss1DFitterController * fitter = m_g1dfitters[ ind];
    fitter->setTitle( "Not in use anymore");
    fitter->setData( 0, std::vector<double>());
    m_g1dfittersUsedFlags[ind] = false;

    // update the client list of active fitters
    pwset( QString("/g1dFittersList/x%1/active").arg( ind), false);
    pwsetdc( "/g1dFittersList/dc", 1);
}

Gauss1DFitterController *ProfileController::getUnusedFitter()
{
    // try to get an unused fitter first
    for( size_t i = 0 ; i < m_g1dfittersUsedFlags.size() ; i ++ ) {
        // skip used fitters
        if( m_g1dfittersUsedFlags[i]) continue;
        // mark this used
        m_g1dfittersUsedFlags[i] = true;
        return m_g1dfitters[i];
    }

    // at this point we have not found an unused fitter, so we'll have to create one
    // but first check the limits
    if( m_g1dfitters.size() >= MAX_G1D_FITTERS) {
        return nullptr;
    }

    QString id = QString("g1d%1").arg( m_g1dfitters.size());
    QString title = "n/a " + id;
    Gauss1DFitterController * fitter = new Gauss1DFitterController(
                this, id, title, true);
    m_g1dfitters.push_back( fitter);
    m_g1dfittersUsedFlags.push_back( true);
    return fitter;
}

void ProfileController::recalculateByproducts()
{
    quint64 totalLength = std::max({m_QzResults.total(), m_UzResults.total(),
                                    m_IzResults.total()});

    // recalculate Pi/Pa profiles
    std::vector<double> pia, paa, qz, uz;
    if( m_UzResults.input.stamp == m_stamp
            && m_QzResults.input.stamp == m_stamp
            && m_IzResults.input.stamp == m_stamp )
    {
        quint64 avail = std::min({m_QzResults.available(), m_UzResults.available(),
                                  m_IzResults.available()});
        pia.resize( avail);
        paa.resize( avail);
        qz.resize( avail);
        uz.resize( avail);
        for( quint64 i = 0 ; i < avail ; i ++ ) {
            double pol = m_fractionalPolarizationMode ? (1.0 / m_IzResults.rawData()[i]) : 1.0;
            double q = m_QzResults.rawData()[i] * pol;
            double u = m_UzResults.rawData()[i] * pol;
            double pi = std::sqrt( q*q + u*u);
            double pa = 0.5 * std::atan2( u, q);
            pia[i] = pi;
            paa[i] = pa;
            qz[i] = q;
            uz[i] = u;
        }
    }
    else {
        // do nothing, as the arrays are empty
    }
    m_QzProfileRenderer->setData( totalLength, qz);
    m_UzProfileRenderer->setData( totalLength, uz);
    m_PiProfileRenderer->setData( totalLength, pia);
    m_PaProfileRenderer->setData( totalLength, paa);
    m_QUprofileController->setData( m_QzResults.total(), qz, uz);
}

void ProfileController::globalVariableCB(const QString &name)
{
    if( m_ignoreVarCallbacks) return;

    if( name == "loadedImageType") {
        if( m_vars.loadedImageType-> get() == "iqu-fake") {
            m_IxProfileRenderer->setTitle( "Ix profile");
            m_IyProfileRenderer->setTitle( "Iy profile");
            m_IzProfileRenderer->setTitle( "Iz profile");
        } else {
            m_IxProfileRenderer->setTitle( "X profile");
            m_IyProfileRenderer->setTitle( "Y profile");
            m_IzProfileRenderer->setTitle( "Z profile");
        }
    }
    else if( name == "fractionalPolarization") {
        m_fractionalPolarizationMode = m_vars.fractionalPolarization-> get();
        if( m_fractionalPolarizationMode) {
            m_QzProfileRenderer->setTitle( "Qz profile [frac]");
            m_UzProfileRenderer->setTitle( "Uz profile [frac]");
            m_PiProfileRenderer->setTitle( "Polarized intensity [frac]");
            m_PaProfileRenderer->setTitle( "Position angle [frac]");
            m_QUprofileController->setTitle( "Qz vs Uz [frac]");
        }
        else {
            m_QzProfileRenderer->setTitle( "Qz profile");
            m_UzProfileRenderer->setTitle( "Uz profile");
            m_PiProfileRenderer->setTitle( "Polarized intensity");
            m_PaProfileRenderer->setTitle( "Position angle");
            m_QUprofileController->setTitle( "Qz vs Uz");
        }
        recalculateByproducts();
    }
    else {
        warn() << "Unknown global var: " << name;
    }
}


void ProfileController::setVisibleRectFits(double minx, double maxx, double miny, double maxy)
{
    m_IxProfileRenderer->setXZoomLimits( floor(minx), ceil(maxx));
    m_IyProfileRenderer->setXZoomLimits( floor(miny), ceil(maxy));
}

void ProfileController::setRegionStatsResults(RegionStatsService::ResultsRS & res)
{
    int nDone = std::min( res.nFramesComputed, res.frames.size());

    std::vector<double> arr( nDone);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].nanPixels; }
    m_RSnanProfileRenderer->setData( res.depth, arr);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].min; }
    m_RSminProfileRenderer->setData( res.depth, arr);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].max; }
    m_RSmaxProfileRenderer->setData( res.depth, arr);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].average; }
    m_RSavgProfileRenderer->setData( res.depth, arr);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].sum; }
    m_RSsumProfileRenderer->setData( res.depth, arr);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].rms; }
    m_RSstdProfileRenderer->setData( res.depth, arr);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].bkgLevel; }
    m_RSbkgProfileRenderer->setData( res.depth, arr);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].sumMinusBkg; }
    m_RSsumbkgProfileRenderer->setData( res.depth, arr);

    for( int i = 0 ; i < nDone ; i ++ ) { arr[i] = res.frames[i].maxMinusBkg; }
    m_RSmaxbkgProfileRenderer->setData( res.depth, arr);
}

/// figure out the best scale for the given unit for the range min..max
/// result is return by overwriting unit and scale
//static void getBestScaleForUnit( QString & unit, double & scale, double min, double max) {
//    scale = 1.0;
//    if( unit.isEmpty()) return;
//    if( unit == "Hz") {
//        unit = "GHz";
//        scale = 1e-9;
//    }
//    else if( unit.startsWith( "hh:mm:ss")) {
//        unit = "";
//        scale = 1.0;
//    }
//    else if( unit.startsWith( "ddd:mm:ss")) {
//        unit = "";
//        scale = 1.0;
//    }
//}



void ProfileController::updateWcs(WcsHero::Hero::SharedPtr wcsHero)
{
    m_wcsHero = wcsHero;
    if( ! m_wcsHero) {
        warn() << "Tried to access NULL hero ptr...";
        return;
    }

    QString bunit = m_wcsHero-> htmlBunit();

//    auto setVertLabelerOld = [&] ( Gauss1DFitterController * gfc, QString caption, QString unit) {
//        Plot2dLabelers::BasicLabeler::SharedPtr vertLabeler = std::make_shared< Plot2dLabelers::BasicLabeler>();
//        vertLabeler-> setCaption1( caption);
//        vertLabeler-> setUnit1( unit);
//        vertLabeler-> setDual( false);
//        gfc-> setVerticalLabeler( vertLabeler);
//    };

    auto setVertLabeler = [&] ( Gauss1DFitterController * gfc, QString caption, QString unit) {
        auto vLab = std::make_shared<VerticalLabeler>( caption, unit);
//        vLab-> setCaption1( caption);
//        vLab-> setUnit1( unit);
        vLab-> setDual( false);
        gfc-> setVerticalLabeler( vLab);
    };

    // update vertical labelers for all graphs
    setVertLabeler( m_IxProfileRenderer, "Value", bunit);
    setVertLabeler( m_IyProfileRenderer, "Value", bunit);
    setVertLabeler( m_IzProfileRenderer, "Value", bunit);
    setVertLabeler( m_QzProfileRenderer, "Value", bunit);
    setVertLabeler( m_UzProfileRenderer, "Value", bunit);
    setVertLabeler( m_PaProfileRenderer, "Pos. Angle", "rad");
    setVertLabeler( m_PiProfileRenderer, "Pol. Intensity", "");
    setVertLabeler( m_RSavgProfileRenderer, "Value", bunit);
    setVertLabeler( m_RSbkgProfileRenderer, "Value", bunit);
    setVertLabeler( m_RSmaxbkgProfileRenderer, "Value", bunit);
    setVertLabeler( m_RSmaxProfileRenderer, "Value", bunit);
    setVertLabeler( m_RSminProfileRenderer, "Value", bunit);
    setVertLabeler( m_RSnanProfileRenderer, "Value", "");
    setVertLabeler( m_RSstdProfileRenderer, "Value", bunit);
    setVertLabeler( m_RSsumbkgProfileRenderer, "Value", bunit);
    setVertLabeler( m_RSsumProfileRenderer, "Value", bunit);

    // update horizontal labelers for all z-based profiles too
//    if( m_wcsHero-> naxes() > 2) {
//        auto zLabeler = makeWcsLabelMaker( {1, 1, 1}, 2, m_wcsHero);
//        const std::vector<WcsHero::AxisInfo> & ai = m_wcsHero-> axesInfos();
//        QString hUnit = "";
//        QString hCaption = "";
//        if( ai.size() > 2) {
//            hUnit = ai[2].unit;
//            hCaption = ai[2].label;
//        }
//        for( Gauss1DFitterController * zp : m_1D_zProfiles) {
//            zp-> horizontalLabeler()-> setCaption1( hCaption);
//            zp-> horizontalLabeler()-> setUnit1( hUnit);
//            zp-> horizontalLabeler()-> setLmf( zLabeler);
//        }
//    }

    // update horizontal labelers
    std::vector<double> cursor;
    cursor.push_back( m_x + 1);
    cursor.push_back( m_y + 1);
    cursor.push_back( m_z + 1);
    auto setHorizLabeler = [&] ( Gauss1DFitterController * gfc, size_t which) {
        Plot2dLabelers::BasicLabeler::SharedPtr hLab = std::make_shared< HorizontalLabeler>( m_wcsHero, cursor, which);
        gfc-> setHorizontalLabeler( hLab);
    };
    setHorizLabeler( m_IxProfileRenderer, 0);
    setHorizLabeler( m_IyProfileRenderer, 1);
    for( Gauss1DFitterController * zp : m_1D_zProfiles) {
        setHorizLabeler( zp, 2);
    }

    // update captions
//    if( m_wcsHero-> naxes() > 0)
//        m_IxProfileRenderer-> horizontalLabeler()-> setCaption1( m_wcsHero-> axesInfos()[0].label);
//    if( m_wcsHero-> naxes() > 1)
//        m_IyProfileRenderer-> horizontalLabeler()-> setCaption1( m_wcsHero-> axesInfos()[1].label);
//    if( m_wcsHero-> naxes() > 2) {
//        for( Gauss1DFitterController * zp : m_1D_zProfiles) {
//            zp-> horizontalLabeler()-> setCaption1( m_wcsHero-> axesInfos()[2].label);
//        }
//    }
}

