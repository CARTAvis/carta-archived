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
#include "CartaLib/IRemoteVGView.h"
#include <QFileInfo>
#include <QPainter>
#include <set>

namespace Carta
{
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
    prefixedSetState( "knownSkyCS/s0", QString( "%1 J2000" ).arg(
                          Carta::Lib::knownSkyCS2int( Carta::Lib::KnownSkyCS::J2000 ) ) );
    prefixedSetState( "knownSkyCS/s1", QString( "%1 B1950" ).arg(
                          static_cast < int > ( Carta::Lib::KnownSkyCS::B1950 ) ) );
    prefixedSetState( "knownSkyCS/s2", QString( "%1 ICRS" ).arg(
                          static_cast < int > ( Carta::Lib::KnownSkyCS::ICRS ) ) );
    prefixedSetState( "knownSkyCS/s3", QString( "%1 Galactic" ).arg(
                          static_cast < int > ( Carta::Lib::KnownSkyCS::Galactic ) ) );
    prefixedSetState( "knownSkyCS/s4", QString( "%1 Ecliptic" ).arg(
                          static_cast < int > ( Carta::Lib::KnownSkyCS::Ecliptic ) ) );
    prefixedSetState( "knownSkyCS/count", "5" );

    // managed layer demo
    m_lvDemo.reset( new LayeredViewDemo( this));

#ifdef DONT_COMPILE
    // layered view 2 stuff
    auto ccl = [] ( int x, int y, int r, QColor color ) -> QImage {
        QImage img( 500, 500, QImage::Format_ARGB32_Premultiplied );
        img.fill( 0xff000000 );
        QPainter p( & img );
        p.setPen( "white" );
        p.setBrush( color );
        p.drawEllipse( QPoint( x, y ), r, r );
        p.end();
        return img;
    };
    static Carta::Lib::LayeredRemoteVGView::SharedPtr vgview2(
        new Carta::Lib::LayeredRemoteVGView( m_connector, "vgview2", this ));

    vgview2-> setRasterLayer( 0, ccl( 50, 50, 45, "white" ) );
    auto acombgreen = std::make_shared < Carta::Lib::PixelMaskCombiner > ();
    acombgreen-> setAlpha( 1.0 );
    acombgreen-> setMask( 0xff00ff00 );
    vgview2-> setRasterLayerCombiner( 0, acombgreen );

    vgview2-> setRasterLayer( 1, ccl( 70, 70, 45, QColor( "white" ) ) );
    auto acombred = std::make_shared < Carta::Lib::PixelMaskCombiner > ();
    acombred-> setAlpha( 0.9 );
    acombred-> setMask( 0xffff0000 );
    vgview2-> setRasterLayerCombiner( 1, acombred );

    vgview2-> setRasterLayer( 2, ccl( 50, 80, 45, QColor( "white" ) ) );
    auto acombblue = std::make_shared < Carta::Lib::PixelMaskCombiner > ();
    acombblue-> setAlpha( 0.9 );
    acombblue-> setMask( 0xff0000ff );
    vgview2-> setRasterLayerCombiner( 2, acombblue );

    vgview2-> scheduleRepaint();

    // commands:
    // <layer> <cmd> [args]
    // 0 load c1
    // 0 load v1
    // 0 load /scratch/Images/tree.jpg
    // 0 alpha 0.5
    // 0 mask 0xff0000 0.5
    auto cmdCB =
    [&] ( const QString & cmd, const QString & params, const QString & sid ) -> QString {
        qDebug() << "cmd" << cmd << params << sid;
        QStringList list = params.split( ' ', QString::SkipEmptyParts );
        if ( list.size() < 2 ) {
            qWarning() << "Bad command" << params;
            return "Bad command";
        }
        bool ok;
        int layer = list[0].toInt( & ok );
        if ( ! ok ) {
            qWarning() << "Bad layer in command" << params;
            return "Bad layer in command";
        }

        // get op
        QString opcmd = list[1];
        if ( opcmd == "load" ) {
            if ( list.size() < 3 ) {
                return "What do you want to load?";
            }
            QString load = list[2];
            if ( load.startsWith( '/' ) && QFileInfo( load ).exists() ) {
                QImage img;
                if ( ! img.load( load ) ) {
                    return "Could not load image";
                }
                vgview2-> setRasterLayer( layer, img );
            }
            else if ( load == "c1" ) {
                vgview2-> setRasterLayer( layer, ccl( 50, 50, 45, "white" ) );
            }
            else if ( load == "c2" ) {
                vgview2-> setRasterLayer( layer, ccl( 70, 70, 45, "white" ) );
            }
            else {
                return "Don't know how to load this";
            }
        }
        else if ( opcmd == "alpha" ) {
            if ( list.size() < 3 ) {
                return "No alpha value?";
            }
            double alpha = list[2].toDouble( & ok );
            if ( ! ok || ! ( alpha >= 0 && alpha <= 1 ) ) {
                return "Bad alpha";
            }
            auto comp = std::make_shared < Carta::Lib::AlphaCombiner > ();
            comp-> setAlpha( alpha );
            vgview2-> setRasterLayerCombiner( layer, comp );
        }
        else if ( opcmd == "mask" ) {
            if ( list.size() < 4 ) {
                return "No mask and/or alpha value?";
            }
            quint32 mask = list[2].toInt( & ok, 16 );
            if ( ! ok ) {
                return "Bad mask";
            }
            mask |= 0xff000000;
            double alpha = list[3].toDouble( & ok );
            if ( ! ok || ! ( alpha >= 0 && alpha <= 1 ) ) {
                return "Bad alpha";
            }
            auto comp = std::make_shared < Carta::Lib::PixelMaskCombiner > ();
            comp-> setAlpha( alpha );
            comp-> setMask( mask );
            vgview2-> setRasterLayerCombiner( layer, comp );
        }
        else if ( opcmd == "time" ) {}
        else {
            qWarning() << "Bad layer operation" << params;
            return "Bad layer operation";
        }
        vgview2-> scheduleRepaint();
        return "OK";
    };
    m_connector-> addCommandCallback( "lvgview-cmd", cmdCB );
#endif

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
