/**
 *
 **/

#include "HackViewer.h"
#include "Globals.h"
#include "CmdLine.h"
#include "IConnector.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "core/Hacks/MainModel.h"
#include "core/GrayColormap.h"
#include "CartaLib/LinearMap.h"
#include <QPainter>
#include <set>

namespace Hacks
{
HackViewer::HackViewer( QString prefix ) :
    QObject( nullptr )
{
    m_statePrefix = prefix;
}

void
HackViewer::start()
{
    qDebug() << "HackViewer::start() starting";

    auto & globals = * Globals::instance();
    m_connector = globals.connector();

    // if we instanciated this class, that means hacks are enabled
    m_connector-> setState( "/hacks/enabled", "1" );

    auto pm = globals.pluginManager();

    // ================================================================================
    // colormap controller
    // ================================================================================
    // prepare a list of known colormaps
    // - built in colormap
    // - plugin colormaps
    {
        // built-in colormaps first (we only have one)
        m_allColormaps.push_back( std::make_shared < Carta::Core::GrayColormap > () );

        // now ask plugins for colormaps
        auto hh =
            pm-> prepare < Carta::Lib::Hooks::ColormapsScalarHook > ();

        auto lam = [this] ( const Carta::Lib::Hooks::ColormapsScalarHook::ResultType & cmaps ) {
            m_allColormaps.insert( m_allColormaps.end(), cmaps.begin(), cmaps.end() );
        };
        hh.forEach( lam );

        qDebug() << "We have" << m_allColormaps.size() << "colormaps:";
        for ( auto & cmap : m_allColormaps ) {
            qDebug() << "    " << cmap-> name();
        }
    }
    // tell clients about available colormaps
    for ( size_t i = 0 ; i < m_allColormaps.size() ; i++ ) {
        prefixedSetState( QString( "cm-names-%1" ).arg( i ), m_allColormaps[i]-> name() );
    }
    prefixedSetState( "cm-count", QString::number( m_allColormaps.size() ) );
    prefixedSetState( "cm-current", "0" );

    // get the name of the image from command line / url / whatever...
    QString fname;
    if ( ! Globals::instance()-> platform()-> initialFileList().isEmpty() ) {
        fname = Globals::instance()-> platform()-> initialFileList()[0];
    }

    // initialize hack globals
    Hacks::GlobalsH::instance().setPluginManager( pm );

    // initialize asynchronous renderer
    m_imageViewController.reset(
                new Hacks::ImageViewController( m_statePrefix + "/views/IVC7", "7" ) );

    // tell the view controller to load the image specified on the command line/url
    m_imageViewController-> loadImage( fname );

    typedef const QString CSR;

    // invert toggle
    prefixedAddStateCallback(
        "cm-invert",
        [this] ( CSR, CSR val ) {
            m_imageViewController-> setCmapInvert( val == "1" );
        }
        );

    // reverse toggle
    prefixedAddStateCallback(
        "cm-reverse",
        [this] ( CSR, CSR val ) {
            m_imageViewController-> setCmapReverse( val == "1" );
        }
        );

    auto colormapCB2 = [this] ( const QString & /*path*/, const QString & value ) {
        qDebug() << "Cmap2 changed to" << value;
        bool ok;
        int ind = value.toInt( & ok );
        if ( ! ok ) {
            return;
        }
        using namespace Carta::Core;
        if ( ind < 0 || size_t( ind ) >= m_allColormaps.size() ) {
        CARTA_ASSERT( "colormap index out of range!" );
        return;
        }
        m_imageViewController-> setColormap( m_allColormaps[ind] );
    };

    const QString pixelCachingOn = "pixelCacheOn";
    const QString pixelCacheSize = "pixelCacheSize";
    const QString pixelCacheInterpolationOn = "pixelCacheInterpolationOn";
    prefixedSetState( pixelCachingOn, "1" );
    prefixedSetState( pixelCacheInterpolationOn, "1" );
    prefixedSetState( pixelCacheSize, "1000" );

    prefixedAddStateCallback( pixelCachingOn, [this] ( CSR, CSR val ) {
                                  auto set = m_imageViewController-> getPPCsettings();
                                  set.enabled = val == "1";
                                  m_imageViewController-> setPPCsettings( set );
                              }
                              );
    prefixedAddStateCallback( pixelCacheInterpolationOn, [this] ( CSR, CSR val ) {
                                  auto set = m_imageViewController-> getPPCsettings();
                                  set.interpolated = val == "1";
                                  m_imageViewController-> setPPCsettings( set );
                              }
                              );
    prefixedAddStateCallback( pixelCacheSize, [ = ] ( CSR, CSR val ) {
                                  auto set = m_imageViewController-> getPPCsettings();
                                  bool ok;
                                  int size = val.toInt( & ok );
                                  if ( ! ok || size < 2 ) {
                                      size = 2;
                                  }
                                  set.size = size;
                                  m_imageViewController-> setPPCsettings( set );
                              }
                              );

    prefixedAddStateCallback( "cm-current", colormapCB2 );

    // tell clients about available coordinate systems
    prefixedSetState( "knownSkyCS/s0", QString("%1 J2000").arg(
                          Carta::Lib::knownSkyCS2int( Carta::Lib::KnownSkyCS::J2000)));
    prefixedSetState( "knownSkyCS/s1", QString("%1 B1950").arg(
                          static_cast<int>( Carta::Lib::KnownSkyCS::B1950)));
    prefixedSetState( "knownSkyCS/s2", QString("%1 ICRS").arg(
                          static_cast<int>( Carta::Lib::KnownSkyCS::ICRS)));
    prefixedSetState( "knownSkyCS/s3", QString("%1 Galactic").arg(
                          static_cast<int>( Carta::Lib::KnownSkyCS::Galactic)));
    prefixedSetState( "knownSkyCS/s4", QString("%1 Ecliptic").arg(
                          static_cast<int>( Carta::Lib::KnownSkyCS::Ecliptic)));
    prefixedSetState( "knownSkyCS/count", "5");

    qDebug() << "HackViewer has been initialized.";
} // start

// prefixed setState
void
HackViewer::prefixedSetState( const QString & path, const QString & value )
{
    m_connector-> setState( m_statePrefix + "/" + path, value );
}

// prefixed getState
QString
HackViewer::prefixedGetState( const QString & path )
{
    return m_connector-> getState( m_statePrefix + "/" + path );
}

IConnector::CallbackID
HackViewer::prefixedAddStateCallback( QString path, IConnector::StateChangedCallback cb )
{
    return m_connector-> addStateCallback( m_statePrefix + "/" + path, cb );
}
}

/// experiment, currently unused
///

/*
static
std::map < QString, QString >
parseParamMap2( const QString & paramsToParse, const std::set < QString > & keyList )
{
    std::map < QString, QString > result;
    for ( const auto & entry : paramsToParse.split( ',' ) ) {
        auto keyVal = entry.split( ':' );
        if ( keyVal.size() != 2 ) {
            qWarning() << "bad map format:" << paramsToParse;
            return { };
        }
        auto key = keyVal[0].trimmed();
        auto val = keyVal[1].trimmed();
        auto ind = result.find( key );
        if ( ind != result.end() ) {
            qWarning() << "duplicate key:" << paramsToParse;
            return { };
        }
        result.insert( ind, std::make_pair( key, val ) );
    }

    // make sure every key is in parameters
    for ( const auto & key : keyList ) {
        if ( ! result.count( key ) ) {
            qWarning() << "could not find key=" << key << "in" << paramsToParse;
            return { };
        }

        // make sure parameters don't have unknown keys
    }
    for ( const auto & kv : result ) {
        if ( ! keyList.count( kv.first ) ) {
            qWarning() << "unknown key" << kv.first << "in" << paramsToParse;
            return { };
        }
    }
    return result;
} // parseParamMap2
*/
