#include "Colormaps1.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/PWLinear.h"
#include <QDebug>
#include <cstdint>

typedef Carta::Lib::Hooks::Initialize Initialize;

Colormap1::Colormap1( QObject * parent ) :
    QObject( parent )
{ }

bool
Colormap1::handleHook( BaseHook & hookData )
{
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
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
cubeHelix( double start, double rots, double hue, double gamma )
{
    Carta::Lib::PWLinear red, green, blue;

    int nlev = 1000;
    for ( int i = 0 ; i < nlev ; i++ ) {
        double fract = double (i) / nlev;
        double angle = 2.0 * M_PI * ( start / 3.0 + 1.0 + rots * fract );
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
    QString start_str = QString::number(start);
    QString rots_str = QString::number(rots);
    QString hue_str = QString::number(hue);
    QString gamma_str = QString::number(gamma);
    QString comma = QChar(0x0375);
    QString left_parenthesis = QChar(0x0028);
    QString right_parenthesis = QChar(0x0029);
    QString name = "CubeHelix" + left_parenthesis
            + start_str + comma
            + rots_str + comma
            + hue_str + comma
            + gamma_str + right_parenthesis;
    return std::make_shared < MyColormapFunc > ( name, red, green, blue );
} // cubeHelix

static Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr
continuousFunc( QString name )
{
    Carta::Lib::PWLinear red, green, blue;

    int nlev = 100;
    for ( int i = 0 ; i < nlev ; i++ ) {

        double fract = double (i) / nlev;
        double r, g, b;

        if ( name == "gnuplot-test" ) {
            r = sqrt(fract); // python syntax: np.sqrt(x)
            g = pow( fract, 3 ); // python syntax: x ** 3
            b = sin(fract * 2 * M_PI) ; // python syntax: np.sin(x * 2 * np.pi)
        } else {
            // for name == "gnuplot2-test"
            r = fract / 0.32 - 0.78125;
            g = 2 * fract - 0.84;
            if ( fract < 0.25 ) {
                b = 4 * fract;
            } else if ( fract >= 0.25 && fract < 0.92) {
                b = -2 * fract + 1.84;
            } else if ( fract >= 0.92 ) {
                b = fract / 0.08 - 11.5;
            } else {
                b = 0.0;
            }
        }

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
} // continuousFunc

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

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Spectral",
            PWLinear().add( 0, 0.61960784313725492 ).add( 0.1, 0.83529411764705885 )
                      .add( 0.2, 0.95686274509803926 ).add( 0.3, 0.99215686274509807 )
                      .add( 0.4, 0.99607843137254903 ).add( 0.5, 1.0 )
                      .add( 0.6, 0.90196078431372551 ).add( 0.7, 0.6705882352941176 )
                      .add( 0.8, 0.4 ).add( 0.9, 0.19607843137254902 )
                      .add( 1, 0.36862745098039218 ),
            PWLinear().add( 0, 0.003921568627450980 ).add( 0.1, 0.24313725490196078 )
                      .add( 0.2, 0.42745098039215684 ).add( 0.3, 0.68235294117647061 )
                      .add( 0.4, 0.8784313725490196 ).add( 0.5, 1.0 )
                      .add( 0.6, 0.96078431372549022 ).add( 0.7, 0.8666666666666667 )
                      .add( 0.8, 0.76078431372549016 ).add( 0.9, 0.53333333333333333 )
                      .add( 1, 0.30980392156862746 ),
            PWLinear().add( 0, 0.25882352941176473 ).add( 0.1, 0.30980392156862746 )
                      .add( 0.2, 0.2627450980392157 ).add( 0.3, 0.38039215686274508 )
                      .add( 0.4, 0.54509803921568623 ).add( 0.5, 0.74901960784313726 )
                      .add( 0.6, 0.59607843137254901 ).add( 0.7, 0.64313725490196083 )
                      .add( 0.8, 0.6470588235294118 ).add( 0.9, 0.74117647058823533 )
                      .add( 1, 0.63529411764705879 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "coolwarm",
            PWLinear().add(0.0, 0.2298057).add(0.03125, 0.26623388)
                      .add(0.0625, 0.30386891).add(0.09375, 0.342804478)
                      .add(0.125, 0.38301334).add(0.15625, 0.424369608)
                      .add(0.1875, 0.46666708).add(0.21875, 0.509635204)
                      .add(0.25, 0.552953156).add(0.28125, 0.596262162)
                      .add(0.3125, 0.639176211).add(0.34375, 0.681291281)
                      .add(0.375, 0.722193294).add(0.40625, 0.761464949)
                      .add(0.4375, 0.798691636).add(0.46875, 0.833466556)
                      .add(0.5, 0.865395197).add(0.53125, 0.897787179)
                      .add(0.5625, 0.924127593).add(0.59375, 0.944468518)
                      .add(0.625, 0.958852946).add(0.65625, 0.96732803)
                      .add(0.6875, 0.969954137).add(0.71875, 0.966811177)
                      .add(0.75, 0.958003065).add(0.78125, 0.943660866)
                      .add(0.8125, 0.923944917).add(0.84375, 0.89904617)
                      .add(0.875, 0.869186849).add(0.90625, 0.834620542)
                      .add(0.9375, 0.795631745).add(0.96875, 0.752534934)
                      .add(1.0, 0.705673158),
            PWLinear().add(0.0, 0.298717966).add(0.03125, 0.353094838)
                      .add(0.0625, 0.406535296).add(0.09375, 0.458757618)
                      .add(0.125, 0.50941904).add(0.15625, 0.558148092)
                      .add(0.1875, 0.604562568).add(0.21875, 0.648280772)
                      .add(0.25, 0.688929332).add(0.28125, 0.726149107)
                      .add(0.3125, 0.759599947).add(0.34375, 0.788964712)
                      .add(0.375, 0.813952739).add(0.40625, 0.834302879)
                      .add(0.4375, 0.849786142).add(0.46875, 0.860207984)
                      .add(0.5, 0.86541021).add(0.53125, 0.848937047)
                      .add(0.5625, 0.827384882).add(0.59375, 0.800927443)
                      .add(0.625, 0.769767752).add(0.65625, 0.734132809)
                      .add(0.6875, 0.694266682).add(0.71875, 0.650421156)
                      .add(0.75, 0.602842431).add(0.78125, 0.551750968)
                      .add(0.8125, 0.49730856).add(0.84375, 0.439559467)
                      .add(0.875, 0.378313092).add(0.90625, 0.312874446)
                      .add(0.9375, 0.24128379).add(0.96875, 0.157246067)
                      .add(1.0, 0.01555616),
            PWLinear().add(0.0, 0.753683153).add(0.03125, 0.801466763)
                      .add(0.0625, 0.84495867).add(0.09375, 0.883725899)
                      .add(0.125, 0.917387822).add(0.15625, 0.945619588)
                      .add(0.1875, 0.968154911).add(0.21875, 0.98478814)
                      .add(0.25, 0.995375608).add(0.28125, 0.999836203)
                      .add(0.3125, 0.998151185).add(0.34375, 0.990363227)
                      .add(0.375, 0.976574709).add(0.40625, 0.956945269)
                      .add(0.4375, 0.931688648).add(0.46875, 0.901068838)
                      .add(0.5, 0.865395561).add(0.53125, 0.820880546)
                      .add(0.5625, 0.774508472).add(0.59375, 0.726736146)
                      .add(0.625, 0.678007945).add(0.65625, 0.628751763)
                      .add(0.6875, 0.579375448).add(0.71875, 0.530263762)
                      .add(0.75, 0.481775914).add(0.78125, 0.434243684)
                      .add(0.8125, 0.387970225).add(0.84375, 0.343229596)
                      .add(0.875, 0.300267182).add(0.90625, 0.259301199)
                      .add(0.9375, 0.220525627).add(0.96875, 0.184115123)
                      .add(1.0, 0.150232812)
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Set1",
            PWLinear().add( 0, 0.89411764705882357 ).add( 1.0/8, 0.21568627450980393 )
                      .add( 2.0/8, 0.30196078431372547 ).add( 3.0/8, 0.59607843137254901 )
                      .add( 4.0/8, 1.0 ).add( 5.0/8, 1.0 )
                      .add( 6.0/8, 0.65098039215686276 ).add( 7.0/8, 0.96862745098039216 )
                      .add( 1, 0.6 ),
            PWLinear().add( 0, 0.10196078431372549 ).add( 1.0/8, 0.49411764705882355 )
                      .add( 2.0/8, 0.68627450980392157 ).add( 3.0/8, 0.30588235294117649 )
                      .add( 4.0/8, 0.49803921568627452 ).add( 5.0/8, 1.0 )
                      .add( 6.0/8, 0.33725490196078434 ).add( 7.0/8, 0.50588235294117645 )
                      .add( 1, 0.6 ),
            PWLinear().add( 0, 0.10980392156862745 ).add( 1.0/8, 0.72156862745098038 )
                      .add( 2.0/8, 0.29019607843137257 ).add( 3.0/8, 0.63921568627450975 )
                      .add( 4.0/8, 0.0 ).add( 5.0/8, 0.2 )
                      .add( 6.0/8, 0.15686274509803921 ).add( 7.0/8, 0.74901960784313726 )
                      .add( 1, 0.6 )
            ) );

    // The following function for "gnuplot2-test" is slow if we choose a large number of segments ( > 1000 )
    //res.emplace_back( Impl::continuousFunc( "gnuplot2-test" ) );

    // use this function for "gnuplot2-test" instead
    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "gnuplot2",
            PWLinear().add( 0.25, 0 ).add( 0.57, 1 ),
            PWLinear().add( 0.42, 0 ).add( 0.92, 1 ),
            PWLinear().add( 0.0, 0 ).add( 0.25, 1 )
                      .add( 0.42, 1 ).add( 0.92, 0 )
                      .add( 1.0 ,1 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "jet",
            PWLinear().add( 0., 0 ).add( 0.35, 0 )
                      .add( 0.66, 1 ).add( 0.89, 1 )
                      .add( 1, 0.5 ),
            PWLinear().add( 0., 0 ).add( 0.125, 0 )
                      .add( 0.375, 1 ).add( 0.64, 1 )
                      .add( 0.91, 0 ).add( 1, 0 ),
            PWLinear().add( 0., 0.5 ).add( 0.11, 1 )
                      .add( 0.34, 1 ).add( 0.65, 0 )
                      .add( 1, 0 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Reds",
            PWLinear().add( 0, 1.0 ).add( 1.0/8, 0.99607843137254903 )
                      .add( 2.0/8, 0.9882352941176471 ).add( 3.0/8, 0.9882352941176471 )
                      .add( 4.0/8, 0.98431372549019602 ).add( 5.0/8, 0.93725490196078431 )
                      .add( 6.0/8, 0.79607843137254897 ).add( 7.0/8, 0.6470588235294118 )
                      .add( 1, 0.40392156862745099 ),
            PWLinear().add( 0, 0.96078431372549022 ).add( 1.0/8, 0.8784313725490196 )
                      .add( 2.0/8, 0.73333333333333328 ).add( 3.0/8, 0.5725490196078431 )
                      .add( 4.0/8, 0.41568627450980394 ).add( 5.0/8, 0.23137254901960785 )
                      .add( 6.0/8, 0.094117647058823528 ).add( 7.0/8, 0.058823529411764705 )
                      .add( 1, 0.0 ),
            PWLinear().add( 0, 0.94117647058823528 ).add( 1.0/8, 0.82352941176470584 )
                      .add( 2.0/8, 0.63137254901960782 ).add( 3.0/8, 0.44705882352941179 )
                      .add( 4.0/8, 0.29019607843137257 ).add( 5.0/8, 0.17254901960784313 )
                      .add( 6.0/8, 0.11372549019607843 ).add( 7.0/8, 0.08235294117647058 )
                      .add( 1, 0.05098039215686274 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "gist_stern",
            PWLinear().add( 0.0 , 0.0 ).add( 0.0547, 1.0 )
                      .add( 0.25, 0.027 ).add( 0.25, 0.25 )
                      .add( 1.0, 1.0 ),
            PWLinear().add( 0.0, 0.0 ).add ( 1.0, 1.0 ),
            PWLinear().add( 0.0, 0.0 ).add( 0.5, 1.0 )
                      .add( 0.735, 0.0 ).add( 1.0, 1.0 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Blues",
            PWLinear().add( 0.0, 0.96862745098039216 ).add( 1.0/8, 0.87058823529411766 )
                      .add( 2.0/8, 0.77647058823529413 ).add( 3.0/8, 0.61960784313725492 )
                      .add( 4.0/8, 0.41960784313725491 ).add( 5.0/8, 0.25882352941176473 )
                      .add( 6.0/8, 0.12941176470588237 ).add( 7.0/8, 0.03137254901960784 )
                      .add( 1.0, 0.03137254901960784 ),
            PWLinear().add( 0.0, 0.98431372549019602 ).add( 1.0/8, 0.92156862745098034 )
                      .add( 2.0/8, 0.85882352941176465 ).add( 3.0/8, 0.792156862745098 )
                      .add( 4.0/8, 0.68235294117647061 ).add( 5.0/8, 0.5725490196078431 )
                      .add( 6.0/8, 0.44313725490196076 ).add( 7.0/8, 0.31764705882352939 )
                      .add( 1.0, 0.18823529411764706 ),
            PWLinear().add( 0.0, 1.0 ).add( 1.0/8, 0.96862745098039216 )
                      .add( 2.0/8, 0.93725490196078431 ).add( 3.0/8, 0.88235294117647056 )
                      .add( 4.0/8, 0.83921568627450982 ).add( 5.0/8, 0.77647058823529413 )
                      .add( 6.0/8, 0.70980392156862748 ).add( 7.0/8, 0.61176470588235299 )
                      .add( 1.0, 0.41960784313725491 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "terrain",
            PWLinear().add( 0.0, 0.2 ).add( 0.15, 0.0 )
                      .add( 0.25, 0.0 ).add( 0.5, 1.0 )
                      .add( 0.75, 0.5 ).add( 1.0, 1.0 ),
            PWLinear().add( 0.0, 0.2 ).add( 0.15, 0.6 )
                      .add( 0.25, 0.8 ).add( 0.5, 1.0 )
                      .add( 0.75, 0.36 ).add( 1.0, 1.0 ),
            PWLinear().add( 0.0, 0.6 ).add( 0.15, 1.0 )
                      .add( 0.25, 0.4 ).add( 0.5, 0.6 )
                      .add( 0.75, 0.33 ).add( 1.0, 1.0 )
            ) );

    res.emplace_back( Impl::continuousFunc( "gnuplot" ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "hot",
            PWLinear().add( 0.0, 0.0416 ).add( 0.365079, 1.0 )
                      .add( 1.0, 1.0 ),
            PWLinear().add( 0.0, 0.0 ).add( 0.365079, 0.0 )
                      .add( 0.746032, 1.0 ).add( 1.0, 1.0 ),
            PWLinear().add( 0.0, 0.0 ).add( 0.746032, 0.0 )
                      .add( 1.0, 1.0 )
            ) );


    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "Greens",
            PWLinear().add( 0.0, 0.96862745098039216 ).add( 1.0/8, 0.89803921568627454 )
                      .add( 2.0/8, 0.7803921568627451 ).add( 3.0/8, 0.63137254901960782 )
                      .add( 4.0/8, 0.45490196078431372 ).add( 5.0/8, 0.25490196078431371 )
                      .add( 6.0/8, 0.13725490196078433 ).add( 7.0/8, 0.0 )
                      .add( 1.0, 0.0 ),
            PWLinear().add( 0.0, 0.9882352941176471 ).add( 1.0/8, 0.96078431372549022 )
                      .add( 2.0/8, 0.9137254901960784 ).add( 3.0/8, 0.85098039215686272 )
                      .add( 4.0/8, 0.7686274509803922 ).add( 5.0/8, 0.6705882352941176 )
                      .add( 6.0/8, 0.54509803921568623 ).add( 7.0/8, 0.42745098039215684 )
                      .add( 1.0, 0.26666666666666666 ),
            PWLinear().add( 0.0, 0.96078431372549022 ).add( 1.0/8, 0.8784313725490196 )
                      .add( 2.0/8, 0.75294117647058822 ).add( 3.0/8, 0.60784313725490191 )
                      .add( 4.0/8, 0.46274509803921571 ).add( 5.0/8, 0.36470588235294116)
                      .add( 6.0/8, 0.27058823529411763 ).add( 7.0/8, 0.17254901960784313 )
                      .add( 1.0, 0.10588235294117647 )
            ) );

    res.emplace_back(
        std::make_shared < Impl::MyColormapFunc > (
            "nipy_spectral",
            PWLinear().add(0.0, 0.0).add(0.05, 0.4667)
                      .add(0.10, 0.5333).add(0.15, 0.0)
                      .add(0.20, 0.0).add(0.25, 0.0)
                      .add(0.30, 0.0).add(0.35, 0.0)
                      .add(0.40, 0.0).add(0.45, 0.0)
                      .add(0.50, 0.0).add(0.55, 0.0)
                      .add(0.60, 0.0).add(0.65, 0.7333)
                      .add(0.70, 0.9333).add(0.75, 1.0)
                      .add(0.80, 1.0).add(0.85, 1.0)
                      .add(0.90, 0.8667).add(0.95, 0.80)
                      .add(1.0, 0.80),
            PWLinear().add(0.0, 0.0).add(0.05, 0.0)
                      .add(0.10, 0.0).add(0.15, 0.0)
                      .add(0.20, 0.0).add(0.25, 0.4667)
                      .add(0.30, 0.6000).add(0.35, 0.6667)
                      .add(0.40, 0.6667).add(0.45, 0.6000)
                      .add(0.50, 0.7333).add(0.55, 0.8667)
                      .add(0.60, 1.0).add(0.65, 1.0)
                      .add(0.70, 0.9333).add(0.75, 0.8000)
                      .add(0.80, 0.6000).add(0.85, 0.0)
                      .add(0.90, 0.0).add(0.95, 0.0)
                      .add(1.0, 0.80),
            PWLinear().add(0.0, 0.0).add(0.05, 0.5333)
                      .add(0.10, 0.6000).add(0.15, 0.6667)
                      .add(0.20, 0.8667).add(0.25, 0.8667)
                      .add(0.30, 0.8667).add(0.35, 0.6667)
                      .add(0.40, 0.5333).add(0.45, 0.0)
                      .add(0.5, 0.0).add(0.55, 0.0)
                      .add(0.60, 0.0).add(0.65, 0.0)
                      .add(0.70, 0.0).add(0.75, 0.0)
                      .add(0.80, 0.0).add(0.85, 0.0)
                      .add(0.90, 0.0).add(0.95, 0.0)
                      .add(1.0, 0.80)
            ) );

    //
    // for examples of Dave Green's `cubehelix' colour scheme
    // one can refer to the webpage http://www.mrao.cam.ac.uk/~dag/CUBEHELIX/cubewedges.html
    //
    res.emplace_back( Impl::cubeHelix( 0.5, -1.5, 1.5, 1.0 ) ); // origonal 1
    res.emplace_back( Impl::cubeHelix( 0.5, -1.5, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -1.5, 1.0, 0.8 ) ); // original 2
    res.emplace_back( Impl::cubeHelix( 1.5, -1.5, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 2.5, -1.5, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -1.5, 0.5, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -1.5, 1.3, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -2.5, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -2.0, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -1.0, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -0.5, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, 1.5, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, 0.0, 1.0, 1.0 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -1.5, 1.0, 0.5 ) );
    res.emplace_back( Impl::cubeHelix( 0.5, -1.5, 1.0, 1.5 ) );

    return res;
} // getColormaps
