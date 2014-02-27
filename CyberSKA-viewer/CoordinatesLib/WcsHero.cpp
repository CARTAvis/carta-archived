/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#include "WcsHero.h"
#include "FitsViewerLib/FitsParser.h"
#include "FitsViewerLib/common.h"
#include <QString>

extern "C" {
#include "ast.h"
}

// ast wrappers
namespace AstWrappers {
QString getC( void * fset, const QString & key ) {
    std::string keys = key.toStdString();
    return astGetC( fset, keys.c_str());
}

int getI( void * fset, const QString & key ) {
    std::string keys = key.toStdString();
    return astGetI( fset, keys.c_str());
}

void set( void * obj, const QString & val) {
    std::string vals = val.toStdString();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    astSet( obj, vals.c_str());
#pragma GCC diagnostic pop
}
}

//#pragma GCC diagnostic ignored "-Wformat-security"

namespace WcsHero {

// trap ASTlib errors by installing our own astPutErr_
static Hero * currHero = nullptr;
static void logAstErrorToHero( int st, const char * msg) {
    if( currHero == nullptr) return;
    currHero-> addError( QString( "%1:%2").arg(st).arg(msg));
}

extern "C" {
void astPutErr_( int status_value, const char *message ) {
    if( currHero != 0) {
        logAstErrorToHero( status_value, message);
    }
    std::cerr << "AST error: " << status_value << ":" << message << "\n";
}
}

// RAI guard for intercepting ast errors
struct AstErrorGuard {
    AstErrorGuard( Hero * hero) {
        currHero = hero;
    }
    ~AstErrorGuard() {
        currHero = 0;
    }
};

// RAI guard for AST garbage collection
struct AstGCGuard {
    AstGCGuard() {
        astBegin;
    }
    ~AstGCGuard() {
        astEnd;
    }
};

Hero *  Hero::constructFromFitsFile(const QString &fname)
{
    FitsParser parser;
    bool parsedOk = parser.loadFile( FitsFileLocation::fromLocal( fname));
    if( ! parsedOk) {
        dbg(1) << "Parser failed to load " << fname;
        Hero * heroPtr = new Hero;
        heroPtr-> addError( "FitsParser failed to load the file");
        return heroPtr;
    }

    // alias hdr
    auto & hdr = parser.getHeaderInfo().headerLines;

    Hero * heroPtr = new Hero;
    Hero & hero = * heroPtr;
    AstErrorGuard guard( heroPtr);
    AstGCGuard gcGuard;

    // set naxes in case AST fails to read this file
    hero.m_ast.naxes = parser.getHeaderInfo().naxis;

    // set up bunit
    {
        hero.m_bunit = parser.getHeaderInfo().bunit;
    }
    // and the nicer version of bunit
    {
        QString u = hero.m_bunit.simplified();
        if( u.toLower() == "kelvin") {
            hero.m_bunitNiceHtml = "K";
        }
        else {
            hero.m_bunitNiceHtml = u;
        }
    }

    // Create a FitsChan and feed it the fits header
    AstFitsChan *fitschan;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
    fitschan = astFitsChan( NULL, NULL, "" );
#pragma GCC diagnostic pop

    std::cout << "astOK = " << astOK << "\n";

    // feed the header lines one by one and check for errors
    for( const QString & s : hdr) {
        std::string stdstr = s.toStdString();
        astPutFits( fitschan, stdstr.c_str(), 1);
        if( ! astOK) {
            astClearStatus;
            QString ss = s.trimmed();
            std::cout << "Skipping bad card: " << ss << "\n";
            hero.addError( "Skipping card: " + ss);
        }
    }
    // reposition to the beginning of the channel (ast thing, it's required, hmmmkey)
    astClear( fitschan, "Card" );

    std::cout << "astOK = " << astOK << "\n";

    std::cout << "Here\n";

    auto encoding = AstWrappers::getC( fitschan, "Encoding" );
    std::cout << "Encoding = " << encoding << "\n";

    // do we have warnings?
    AstKeyMap * warnings = static_cast<AstKeyMap *>( astWarnings( fitschan));

    if( warnings && astOK ) {
        std::cout << "Warnings:\n";

        int iwarn = 1;
        while( astOK ) {
            std::string key = QString("Warning_%1").arg( iwarn).toStdString();
            const char * message = nullptr;
            if( astMapGet0C( warnings, key.c_str(), & message ) ) {
                printf( "\n- %s\n", message );
                hero.addError( QString( "Warning: %1").arg( message));
            } else {
                break;
            }
        }
    }
    else {
        std::cout << "No warnings\n";
    }

    // create a frameset for this file
    AstFrameSet * wcsinfo = static_cast<AstFrameSet *> ( astRead( fitschan ));
    std::cout << "astOK = " << astOK << "\n";

    if ( ! astOK ) {
        std::cout << "astOK is not ok\n";
        hero.addError( "astRead failed");
        astClearStatus;
        return heroPtr;
    }
    else if ( wcsinfo == AST__NULL ) {
        hero.addError( "No WCS found in the fits file");
        std::cout << "No WCS found\n";
        return heroPtr;
    }
    else if ( AstWrappers::getC( wcsinfo, "Class" ) != "FrameSet") {
        std::cout << "Some other weird error occured\n";
        hero.addError( "AstLib returned non-frame-set");
        return heroPtr;
    }        

    // frame was read in OK, save it
    hero.m_ast.origWcsInfo = wcsinfo;
    astExempt( hero.m_ast.origWcsInfo);
    hero.m_ast.currWcsInfo = astClone( hero.m_ast.origWcsInfo);
    astExempt( hero.m_ast.currWcsInfo);

    astShow( wcsinfo);

    // extract the current sky system
    // TODO: this assumes axis1 is a skycs
    QString skysys = AstWrappers::getC( wcsinfo, "System(1)");
    hero.m_currentSkyCs = string2skycs( skysys);
    hero.m_originalSkyCs = hero.m_currentSkyCs;

    // extract the labels/etc for axes
    hero.m_ast.naxes = AstWrappers::getI( wcsinfo, "Naxes" );
    hero.parseAxesInfo();

    return heroPtr;
}

Hero::SKYCS Hero::getCurentSkyCS()
{
    return m_currentSkyCs;
}

std::vector<Hero::SKYCS> Hero::getConvertibleSkyCS()
{
    if( m_ast.origWcsInfo == AST__NULL) {
        return { SKYCS::PIXEL };
    }

    AstErrorGuard guard( this); // intercept errors
    AstGCGuard gcGuard;

    // go through all known systems and find out which can be used
    std::vector<SKYCS> res;
    for( auto & cs : getAllKnownCS()) {
        if( cs == m_originalSkyCs) {
            res.push_back( cs);
            continue;
        }
        void * testFrame = astCopy( m_ast.origWcsInfo);
        std::string str = QString("System=%1").arg( skycs2string(cs)).toStdString();
        dbg(1) << "Trying " << str;
        AstWrappers::set( testFrame, QString("System=%1").arg( skycs2string(cs)));
        if ( ! astOK ) {
            clearErrors();
            astClearStatus;
        }
        else {
            res.push_back( cs);
        }
    }

    astClearStatus;
    return res;
}

static std::vector<Hero::SKYCS> allKnownCS {
    Hero::SKYCS::FK5, Hero::SKYCS::FK4, Hero::SKYCS::FK4_NO_E,
            Hero::SKYCS::J2000,
            Hero::SKYCS::ICRS, Hero::SKYCS::GALACTIC, Hero::SKYCS::ECLIPTIC,
            Hero::SKYCS::GAPPT, Hero::SKYCS::AZEL, Hero::SKYCS::HELIOECLIPTIC,
            Hero::SKYCS::SUPERGALACTIC, Hero::SKYCS::PIXEL
};
const std::vector<Hero::SKYCS> & Hero::getAllKnownCS()
{
    return allKnownCS;
}

bool Hero::setSkyCS(Hero::SKYCS skyCS)
{
    if( m_currentSkyCs == skyCS) {
        // nothing to do
        return true;
    }

    if( m_ast.origWcsInfo == AST__NULL) {
        addError( "No wcsinfo from astlib");
        return false;
    }

    AstErrorGuard guard( this); // intercept errors
    AstGCGuard gcguard;

    // make a clone
    void * newWcsInfo = AST__NULL;
    if( m_originalSkyCs == skyCS) {
        // original skycs is the same as requested skycs, so
        // clone only a pointer to the original frame
        newWcsInfo = astClone( m_ast.origWcsInfo);
    }
    else {
        // otherwise make a full clone of the frame and set it's system
        newWcsInfo = astCopy( m_ast.origWcsInfo);
        if( ! astOK) {
            addError( "Could not clone the original astFrameSet");
            return false;
        }
        // set the new system for the clone
        AstWrappers::set( newWcsInfo, QString( "System=%1").arg( skycs2string(skyCS)));
        if( ! astOK) {
            addError( "Could not convert to this coordinate system " + skycs2string( skyCS));
            return false;
        }

        astClear( newWcsInfo, "Epoch,Equinox");
    }

    // free up the old ast pointer
    if( m_ast.currWcsInfo != AST__NULL) {
        astAnnul( m_ast.currWcsInfo);
        m_ast.currWcsInfo = AST__NULL;
    }

    m_ast.currWcsInfo = newWcsInfo;
    astExempt( m_ast.currWcsInfo);
    m_currentSkyCs = skyCS;

    parseAxesInfo();


    QString title = AstWrappers::getC( m_ast.currWcsInfo, "Title(1)");
    dbg(1) << "Title = " << title << "["
           << AstWrappers::getC( m_ast.currWcsInfo, "LonAxis") << ","
           << AstWrappers::getC( m_ast.currWcsInfo, "LatAxis") << "]";

    // clear up ast errors if any
    astClearStatus;

    return true;
}

const std::vector<double> & Hero::fits2world(const std::vector<double> & p_pixel)
{
    if( m_ast.origWcsInfo == AST__NULL) {
        m_pixel2wcsCache = p_pixel;
        m_pixel2wcsCache.resize( naxes(), 1.0);
        return m_pixel2wcsCache;
    }

    AstErrorGuard errGuard( this);
    AstGCGuard gcGuard;

    // resize and pad with '1' to conform to naxis
    auto pixel = p_pixel;
    pixel.resize( naxes(), 1.0);

    m_pixel2wcsCache.resize( naxes());

    // do the conversion
    astTranN( m_ast.currWcsInfo, 1, naxes(), 1, & pixel.front(), 1, naxes(), 1,
              & m_pixel2wcsCache.front());

    astClearStatus;
    return m_pixel2wcsCache;
}

QStringList Hero::formatPixelCursor(const std::vector<double> & coords, bool normalize)
{
//    if( m_ast.currWcsInfo == AST__NULL) {
//        QStringList res;
//        res << "WCS not available";
//        return res;
//    }
//    {
//        std::stringstream s;
//        for( auto val : coords) s << QString::number(val).toStdString() << ",";
//        dbg(1) << "fmt_pixel_cursr: " << s.str();
//    }

    // first convert to world coordinates
    const auto & world = fits2world( coords);
    // now format the world coordinates
    return formatWorldCursor( world, normalize);
}

QStringList Hero::formatWorldCursor(const std::vector<double> & coords, bool normalize)
{
    if( m_ast.origWcsInfo == AST__NULL) {
        // format the cursor
        QStringList res;
        for( auto i = 1 ; i <= naxes() ; i ++ ) {
            res.append( QString::number( coords[i-1]));
        }
        return res;
    }

    if( int(coords.size()) != naxes()) {
        LTHROW( QString( "formatWorldCursor called with coords.size=%1 but naxes=%2").arg(coords.size()).arg(naxes()));
    }
    // make a copy of the coordinates
    auto wrld = coords;
    // apply normalization to it
    if( normalize) {
        astNorm( m_ast.currWcsInfo, & wrld.front());
    }
    // format the cursor
    QStringList res;
    for( auto i = 1 ; i <= naxes() ; i ++ ) {
        res.append( astFormat( m_ast.currWcsInfo, i, wrld[i-1]));
    }
    return res;
}

QString Hero::formatWorldValue(double val, int axis)
{
    if( m_ast.origWcsInfo == AST__NULL) {
        return QString::number( val);
    }

    if( axis >= naxes()) {
        LTHROW( QString( "formatWorldValue called with axis=%1 but naxes=%2").arg(axis).arg(naxes()));
    }
    // format the cursor
    QString res = astFormat( m_ast.currWcsInfo, axis + 1, val);
    return res;
}

double Hero::distance(const std::vector<double> &c1, const std::vector<double> &c2)
{
    dbg(1) << "XXXX c1.size = " << c1.size() << " c2.size = " << c2.size();
    dbg(1) << "XXXX c1 = " << c1;
    dbg(1) << "XXXX c2 = " << c2;

    // resize and pad with '1' to conform to naxis
    auto pix1 = c1; pix1.resize( naxes(), 1.0);
    auto pix2 = c2; pix2.resize( naxes(), 1.0);

    // if we don't have a frame, return cartesian distance
    if( m_ast.origWcsInfo == AST__NULL) {
        double sumSq = 0.0;
        for( int i = 0 ; i < naxes() ; i ++ ) {
            double diff = pix1[i] - pix2[i];
            sumSq += diff * diff;
        }
        return sqrt( sumSq);
    }

    dbg(1) << "XXXX fpix1 = " << pix1;
    dbg(1) << "XXXX fpix2 = " << pix2;

    pix1 = fits2world( pix1);
    pix2 = fits2world( pix2);

    dbg(1) << "XXXX wpix1 = " << pix1;
    dbg(1) << "XXXX wpix2 = " << pix2;

    AstErrorGuard errGuard( this);
    AstGCGuard gcGuard;

    // call ast to figure out the distance
    double res = astDistance( m_ast.currWcsInfo, & pix1.front(), & pix2.front());
    dbg(1) << "XXXX dist = " << res;

    astClearStatus;
    return res;
}

const std::vector<AxisInfo> & Hero::axesInfos()
{
    if( int( m_axisInfos.size()) != naxes()) {
        warn() << "axes info size and naxes mismatch";
        // return some default labels
        m_axisInfos.clear();
        for( int i = 0 ; i < naxes() ; i ++ ) {
            AxisInfo info;
            info.label = QString('X' + i);
            info.symbol = QString('X' + i) + "_";
            info.title = QString('X' + i) + " axis";

            // default html versions will be the same as the raw versions
            info.htmlLabel = info.label;
            info.htmlSymbol = info.symbol;
            info.htmlUnit = info.unit;
            info.htmlTitle = info.title;

            m_axisInfos.push_back( info);
        }
    }
    return m_axisInfos;
}

QString Hero::skycs2string(const Hero::SKYCS &skycs)
{
    switch( skycs) {
    case SKYCS::FK5 : return "FK5";
    case SKYCS::FK4 : return "FK4";
    case SKYCS::FK4_NO_E : return "FK4_NO_E";
    case SKYCS::J2000 : return "J2000";
    case SKYCS::ICRS : return "ICRS";
    case SKYCS::GALACTIC : return "GALACTIC";
    case SKYCS::ECLIPTIC : return "ECLIPTIC";
    case SKYCS::GAPPT : return "GAPPT";
    case SKYCS::AZEL : return "AZEL";
    case SKYCS::HELIOECLIPTIC : return "HELIOECLIPTIC";
    case SKYCS::SUPERGALACTIC : return "SUPERGALACTIC";
    case SKYCS::PIXEL : return "PIXEL";
    default :
        return "UNKNOWN";
    }
}

Hero::SKYCS Hero::string2skycs(const QString & str)
{
    if( str == "FK5" || str == "EQUATORIAL") {
        return SKYCS::FK5;
    }
    else if( str == "FK4") {
        return SKYCS::FK4;
    }
    else if( str == "FK4-NO-E" || str == "FK4_NO_E") {
        return SKYCS::FK4_NO_E;
    }
    else if( str == "J2000") {
        return SKYCS::J2000;
    }
    else if( str == "ICRS") {
        return SKYCS::ICRS;
    }
    else if( str == "GAPPT" || str == "GEOCENTRIC" || str == "APPARENT") {
        return SKYCS::GAPPT;
    }
    else if( str == "AZEL") {
        return SKYCS::AZEL;
    }
    else if( str == "ECLIPTIC") {
        return SKYCS::ECLIPTIC;
    }
    else if( str == "HELIOECLIPTIC") {
        return SKYCS::HELIOECLIPTIC;
    }
    else if( str == "GALACTIC") {
        return SKYCS::GALACTIC;
    }
    else if( str == "SUPERGALACTIC") {
        return SKYCS::SUPERGALACTIC;
    }
    else {
        return SKYCS::UNKNOWN;
    }
}

int Hero::naxes() const
{
    return m_ast.naxes;
}

const QStringList &Hero::errors()
{
    return m_errors;
}

void Hero::clearErrors()
{
    m_errors.clear();
}

void Hero::addError(const QString &str)
{
    m_errors.push_back( str);
}

Hero::~Hero()
{
    dbg(1) << "XXXX destroying hero";
    if( m_ast.origWcsInfo != AST__NULL) {
        astAnnul( m_ast.origWcsInfo);
    }
    if( m_ast.currWcsInfo != AST__NULL) {
        astAnnul( m_ast.currWcsInfo);
    }
}

Hero::Hero()
{
    dbg(1) << "XXXX constructing hero";
    m_currentSkyCs = SKYCS::PIXEL;
    m_originalSkyCs = SKYCS::PIXEL;
    m_ast.origWcsInfo = AST__NULL;
    m_ast.currWcsInfo = AST__NULL;
    m_ast.naxes = 0;
}

void Hero::parseAxesInfo()
{
    m_axisInfos.clear();
    m_labelsForAxes.clear();
    for( int i = 1 ; i <= naxes() ; i ++ ) {
        AxisInfo info;
        info.label = AstWrappers::getC( m_ast.currWcsInfo, QString("Label(%1)").arg(i));
        info.symbol = AstWrappers::getC( m_ast.currWcsInfo, QString("Symbol(%1)").arg(i));
        info.unit = AstWrappers::getC( m_ast.currWcsInfo, QString("Unit(%1)").arg(i));
        info.title = AstWrappers::getC( m_ast.currWcsInfo, QString("Title(%1)").arg(i));

        info.setIsLongitude( AstWrappers::getI( m_ast.currWcsInfo, QString("IsLonAxis(%1)").arg(i)));
        astClearStatus;
        info.setIsLatitude( AstWrappers::getI( m_ast.currWcsInfo, QString("IsLatAxis(%1)").arg(i)));
        astClearStatus;

        // default html versions will be the same as the raw versions
        info.htmlLabel = info.label;
        info.htmlSymbol = info.symbol;
        info.htmlUnit = info.unit;
        info.htmlTitle = info.title;

        m_axisInfos.push_back( info);
    }

    for( auto v : m_axisInfos) {
        dbg(1) << "...label:" << v.label
               << " symbol:" << v.symbol
               << " unit:" << v.unit
               << " title:" << v.title;
    }

    // htmlize special cases
    for( AxisInfo & v : m_axisInfos) {

        // symbols
        if( v.symbol == "RA") {
            v.htmlSymbol = "&alpha;";
        }
        else if( v.symbol == "Dec") {
            v.htmlSymbol = "&delta;";
        }
        else if( v.symbol == "Lambda") {
            v.htmlSymbol = "&lambda;";
        }
        else if( v.symbol == "Beta") {
            v.htmlSymbol = "&beta;";
        }
        else if( v.symbol == "FREQ") {
            v.htmlSymbol = "Freq";
        }
        else if( v.symbol == "STOKES") {
            v.htmlSymbol = "Stokes";
        }

        // units
        if( v.unit == "degrees") {
            v.htmlUnit = "&deg;";
        }
        else if( v.unit == "hh:mm:ss.s") {
            v.htmlUnit = "";
        }
        else if( v.unit == "ddd:mm:ss") {
            v.htmlUnit = "";
        }
    }
}









}

