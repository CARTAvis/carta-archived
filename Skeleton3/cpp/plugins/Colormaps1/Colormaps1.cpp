#include "Colormaps1.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
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
    else if ( hookData.is < Carta::Lib::Hooks::ColormapsScalar > () ) {
        Carta::Lib::Hooks::ColormapsScalar & hook
                    = static_cast < Carta::Lib::Hooks::ColormapsScalar & > ( hookData );
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
               Carta::Lib::Hooks::ColormapsScalar::staticId
    };
}

namespace Impl
{
typedef QRgb Rgb;

class ColormapFunction : public Carta::Lib::ColormapScalarNamed
{
public:

    typedef Carta::Lib::PWLinear PWLinear;

    Rgb
    operator() ( double x );

    virtual QRgb
    convert( const double & val )
    {
        return operator() ( val );
    }

    ColormapFunction( QString name, PWLinear & red, PWLinear & green, PWLinear & blue );
    ColormapFunction();
    ColormapFunction( QString name );

    // predefined colormaps
    static ColormapFunction
    gray();

    static ColormapFunction
    heat();

    static ColormapFunction
    fire();

    static ColormapFunction
    spring();

    static ColormapFunction
    sea();

    static ColormapFunction
    sunbow();

    static ColormapFunction
    mutant();

    static ColormapFunction
    aberration();

    static ColormapFunction
    rgb();

    static ColormapFunction
    velocity();

    static ColormapFunction
    cubeHelix( double start, double rots, double hue, double gamma );

private:

    PWLinear red_, green_, blue_;
};

ColormapFunction::ColormapFunction()
    : ColormapFunction( "Some-colormap" )
{ }

ColormapFunction::ColormapFunction( QString name )
    : ColormapScalarNamed( name )
{ }

ColormapFunction::ColormapFunction( QString name, PWLinear & red, PWLinear & green, PWLinear & blue )
    : ColormapFunction( name )
{
    red_   = red;
    green_ = green;
    blue_  = blue;
}

Rgb
ColormapFunction::operator() ( double x )
{
    if ( ! std::isfinite( x ) ) {
        return 0;
    }
    return qRgb( 255 * red_( x ), 255 * green_( x ), 255 * blue_( x ) );
}
}

std::vector < Carta::Lib::IColormapScalar::SharedPtr >
Colormap1::getColormaps()
{
    using namespace Carta::Lib;

    std::vector < IColormapScalar::SharedPtr > res;

    res.emplace_back(
        std::make_shared < Impl::ColormapFunction > (
            "sea",
            PWLinear().add( 0.5, 0 ).add( 1, 1 ),
            PWLinear().add( 0, 0 ).add( 0.5, 1 ),
            PWLinear().add( 0.25, 0 ).add( 0.75, 1 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::ColormapFunction > (
            "sunbow",
            PWLinear().add( 0.5, 1 ).add( 0.75, 0 ),
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.75, 1 ).add( 1, 0 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 )
            ) );

    return res;
} // getColormaps
