#include "Colormaps1.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "CartaLib/PWLinear.h"
#include <QDebug>
#include <cstdint>

Colormap1::Colormap1( QObject * parent ) :
    QObject( parent )
{ }

bool
Colormap1::handleHook( BaseHook & hookData )
{
    if ( hookData.is < Initialize > () ) {
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::ColormapsScalarHook > () ) {
        Carta::Lib::Hooks::ColormapsScalarHook & hook
                    = static_cast < Carta::Lib::Hooks::ColormapsScalarHook & > ( hookData );
        hook.result = getColormaps();
        return true;
    }

    qWarning() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector < HookId >
Colormap1::getInitialHookList()
{
    return {
               Initialize::staticId,
               Carta::Lib::Hooks::ColormapsScalarHook::staticId
    };
}

namespace Impl
{
class MyColormapFunc : public Carta::Lib::PixelPipeline::IColormapNamed
{
public:

    typedef Carta::Lib::PWLinear PWLinear;

//    QRgb
//    operator() ( double x );

//    virtual QRgb
//    convert( const double & val ) override
//    {
//        return operator() ( val );
//    }

    virtual void
    convert( norm_double val, NormRgb & nrgb ) override
    {
        if ( ! std::isfinite( val ) ) {
            nrgb.fill(0.0);
        }
        nrgb[0] = m_red(val);
        nrgb[1] = m_green(val);
        nrgb[2] = m_blue(val);
    }

    virtual QString name() override
    {
        return m_name;
    }


    MyColormapFunc( QString name, PWLinear & red, PWLinear & green, PWLinear & blue );
    MyColormapFunc( QString name );

private:

    PWLinear m_red, m_green, m_blue;
    QString m_name;

    // IColormapScalar interface
};

MyColormapFunc::MyColormapFunc( QString name )
    : m_name(name)
{ }

MyColormapFunc::MyColormapFunc( QString name, PWLinear & red, PWLinear & green, PWLinear & blue )
    : MyColormapFunc( name )
{
    m_red   = red;
    m_green = green;
    m_blue  = blue;
}

//QRgb
//MyColormapFunc::operator() ( double x )
//{
//    if ( ! std::isfinite( x ) ) {
//        return 0;
//    }
//    return qRgb( 255 * m_red( x ), 255 * m_green( x ), 255 * m_blue( x ) );
//}

/// ================================================================================
/// cube helix function as described here:
/// http://www.mrao.cam.ac.uk/~dag/CUBEHELIX/
///
/// and in
///
/// Green, D. A., 2011, Bulletin of the Astronomical Society of India, Vol.39, p.289
/// ================================================================================
static Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr
cubeHelix( QString name, double start, double rots, double hue, double gamma )
{
    Carta::Lib::PWLinear red, green, blue;

    int nlev = 1000;
    for ( int i = 0 ; i < nlev ; i++ ) {
        double fract = double (i) / nlev;
        double angle = 2 * M_PI * ( start / 3 + 1 + rots + fract );
        fract = pow( fract, gamma );

        double amp = hue * fract * ( 1 - fract ) / 2.0;

        double r = fract + amp * ( - 0.14861 * cos( angle ) + 1.78277 * sin( angle ) );
        double g = fract + amp * ( - 0.29227 * cos( angle ) - 0.90649 * sin( angle ) );
        double b = fract + amp * ( + 1.97294 * cos( angle ) );

        if ( r < 0 ) {
            r = 0;
        }
        if ( r > 1 ) {
            r = 1;
        }
        if ( g < 0 ) {
            g = 0;
        }
        if ( g > 1 ) {
            g = 1;
        }
        if ( b < 0 ) {
            b = 0;
        }
        if ( b > 1 ) {
            b = 1;
        }
        red.add( fract, r );
        green.add( fract, g );
        blue.add( fract, b );
    }
    return std::make_shared < MyColormapFunc > ( name, red, green, blue );
} // cubeHelix
}

std::vector < Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr >
Colormap1::getColormaps()
{
    using namespace Carta::Lib;

    std::vector < PixelPipeline::IColormapNamed::SharedPtr > res;

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Sea",
            PWLinear().add( 0.5, 0 ).add( 1, 1 ),
            PWLinear().add( 0, 0 ).add( 0.5, 1 ),
            PWLinear().add( 0.25, 0 ).add( 0.75, 1 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Sunbow",
            PWLinear().add( 0.5, 1 ).add( 0.75, 0 ),
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.75, 1 ).add( 1, 0 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Spring",
            PWLinear().add( 0.25, 0 ).add( 0.75, 1 ),
            PWLinear().add( 0, 0 ).add( 0.5, 1 ),
            PWLinear().add( 0.5, 0 ).add( 1, 1 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Fire",
            PWLinear().add( 0, 0 ).add( 1.0 / 2, 1 ),
            PWLinear().add( 1.0 / 4, 0 ).add( 3.0 / 4, 1 ),
            PWLinear().add( 1.0 / 2, 0 ).add( 1, 1 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Heat",
            PWLinear().add( 0, 0 ).add( 1.0 / 3, 1 ),
            PWLinear().add( 0, 0 ).add( 1, 1 ),
            PWLinear().add( 2.0 / 3, 0 ).add( 1, 1 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Mutant",
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.75, 1 ).add( 1, 0 ),
            PWLinear().add( 0.5, 1 ).add( 0.75, 0 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Aberration",
            PWLinear().add( 0.5, 0 ).add( 0.75, 1 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 ),
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.75, 1 ).add( 1, 0 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Rgb",
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.5, 0 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 ).add( 0.75, 0 ),
            PWLinear().add( 0.5, 0 ).add( 0.75, 1 ).add( 1, 0 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Velocity",
            PWLinear().add( 0.5, 0 ).add( 1, 1 ),
            PWLinear().add( 0, 0 ).add( 0.5, 1 ).add( 1, 0 ),
            PWLinear().add( 0, 1 ).add( 0.5, 0 )
            ) );

    res.emplace_back( Impl::cubeHelix( "CubeHelix1", 0.5, - 1.5, 1.5, 1.0 ) );

    res.emplace_back( Impl::cubeHelix( "CubeHelix2", 0.5, - 1.5, 1.0, 0.8 ) );

    return res;
} // getColormaps
