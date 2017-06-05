/**
 *
 **/

#pragma once

#include "IPixelPipeline.h"
#include "CartaLib/Nullable.h"

namespace Carta
{
namespace Lib
{
namespace PixelPipeline
{
/// reversable implementing stage1
/// aka horizontal flip
class ReversableStage1 : public IStage1
{
    CLASS_BOILERPLATE( ReversableStage1 );

public:

    void
    setReversed( bool flag )
    {
        m_reversed = flag;
    }

    virtual void
    convert( double & val ) override
    {
        if ( ! m_reversed ) { return; }

        // val = m_max - val + m_min;
        val = m_minMaxSum - val;
    }

    virtual void
    setMinMax( double & min, double & max ) override
    {
        m_minMaxSum = min + max;
    }

private:

    double m_minMaxSum = 1.0;
    bool m_reversed = false;
};

/// invertible stage3
/// aka vertical flip
class InvertibleStage4
    : public INormRgb2NormRgb
{
    CLASS_BOILERPLATE( InvertibleStage4 );

public:

    void
    setInverted( bool flag )
    {
        m_inverted = flag;
    }

    virtual void
    convert( NormRgb & drgb ) override
    {
        if ( ! m_inverted ) {
            return;
        }
        drgb[0] = 1.0 - drgb[0];
        drgb[1] = 1.0 - drgb[1];
        drgb[2] = 1.0 - drgb[2];
    }

private:

    bool m_inverted;
};

enum class ScaleType
{
    Linear, /// x' = x
    Polynomial3, /// x' = x^3
    Polynomial4, /// x' = x^4
    Sqr, /// x' = x ^ 2
    Sqrt, /// x' = x ^ 1/2
    Log /// x' = log(ax + 1) / log(a+1)
};

/// stage I that implements scaling (log,poly, etc)
class ScaleStage
    : public IStage1
{
    CLASS_BOILERPLATE( ScaleStage );

public:

    ScaleStage()
    {
        setType( ScaleType::Linear );
    }

    QString
    cacheId()
    {
        QString gamma = "G" + double2base64( m_gamma);
        switch ( m_scaleType )
        {
        case ScaleType::Linear :
            return gamma + "Lin";

        case ScaleType::Log :
            return gamma + "Log" + double2base64( m_a );

        case ScaleType::Sqr :
            return gamma + "Sqr";

        case ScaleType::Sqrt :
            return gamma + "Sqrt";

        case ScaleType::Polynomial3 :
              return gamma + "Poly3";

        case ScaleType::Polynomial4 :
              return gamma + "Poly4";
        }
        CARTA_ASSERT_X( false, "Invalid scale type" );
        return "";
    } // cacheId

    void
    setGamma( double gamma) {
        m_gamma = gamma;
        if( m_gamma < 0) { m_gamma = 0; }
    }

    virtual void
    convert( double & val ) override
    {
        // normalize first
        double n = (val - m_min) / (m_max - m_min);
        // apply function
        m_func(n);
        // apply gamma
        n = std::pow( n, m_gamma);
        // denormalize
        val = n * (m_max-m_min) + m_min;
    }

    void
    setParam( double a )
    {
        m_a = a;
    }

    double
    param()
    {
        return m_a;
    }

    void
    setType( ScaleType stype )
    {
        m_scaleType = stype;
        switch ( m_scaleType )
        {
        case ScaleType::Linear :
            setParam( 1.0 );
            m_func = [] ( double & ) { };
            return;

        case ScaleType::Sqr :
            setParam( 1.0 );
            m_func = [] ( double & val ) {
                val = val * val;
            };
            return;

        case ScaleType::Sqrt :
            setParam( 1.0 );
            m_func = [] ( double & val ) {
                val = std::sqrt( val );
            };
            return;

        case ScaleType::Log :
            setParam( 1000.0 );
            m_func = [this] ( double & val ) {
                val = std::log( m_a * val + 1 ) / std::log( m_a + 1 );
            };
            return;

        case ScaleType::Polynomial3 :
            setParam( 3.0 );
            m_func = [this] ( double & val ) {
                val = std::pow( val, m_a );
            };
            return;

        case ScaleType::Polynomial4 :
            setParam( 4.0 );
            m_func = [this] ( double & val ) {
                val = std::pow( val, m_a );
            };
            return;
        } // switch
        CARTA_ASSERT_X( false, "Invalid scale type" );
    } // setType

    virtual void
    setMinMax( double & min, double & max ) override
    {
        m_min = min;
        m_max = max;
    }

private:

    double m_a = 1000.0;
    ScaleType m_scaleType = ScaleType::Linear;
    std::function < void (double &) > m_func;
    double m_min = 0.0;
    double m_max = 1.0;
    double m_gamma = 1.0;
};

/// this is the pipeline that we use in CARTA
/// it should support all of the GUI actions of a colormap dialog, e.g.:
/// - invert, reverse, colormap, log/gamma/cycles, manual clip
///
/// \todo this is getting extremely inefficient, I think it's time to
/// hand-optimize it without the stages
///
class CustomizablePixelPipeline : public IClippedPixelPipeline
{
    CLASS_BOILERPLATE( CustomizablePixelPipeline );

public:

    CustomizablePixelPipeline()
    {
        m_pipe.reset( new Composite() );
        m_scaleStage = std::make_shared < ScaleStage > ();
        m_pipe-> addStage1( m_scaleStage);
        m_reversible = std::make_shared < ReversableStage1 > ();
        m_pipe-> addStage1( m_reversible );
        m_invertible = std::make_shared < InvertibleStage4 > ();
        m_pipe-> addStage4( m_invertible );
    }

    /// set gamma correction facor (1.0 is default, i.e. no gamma)
    void
    setGamma( double gamma)
    {
        m_scaleStage-> setGamma( gamma);
    }

    /// set max values for rgb (values will be interpolated up to this value)
    /// for example, setting red=0 means there will be no red in the image
    void
    setRgbMax( NormRgb rgb) {
        m_maxRgb = rgb;
    }

    /// some scales require a parameter
    void
    setScaleParam( double a )
    {
        m_scaleStage-> setParam( a );
    }


    /// get the current scale parameter
    /// this could be set as a result of setScale()
    double
    scaleParam()
    {
        return m_scaleStage-> param();
    }

    /// which scale to use
    void
    setScale( ScaleType scale )
    {
        m_scaleStage-> setType( scale );
    }

    void
    setInvert( bool flag )
    {
        m_invertFlag = flag;
        m_invertible-> setInverted( flag );
    }

    void
    setReverse( bool flag )
    {
        m_reverseFlag = flag;
        m_reversible-> setReversed( flag );
    }

    void
    setColormap( IColormapNamed::SharedPtr colormap )
    {
        m_cmapName = colormap-> name();
        m_pipe-> setStage3( colormap );
    }

    /*virtual*/ void
    setMinMax( double min, double max )
    {
        m_clipMin = min;
        m_clipMax = max;
        m_pipe-> setMinMax( m_clipMin, m_clipMax );
    }

    virtual void
    convert( double val, Carta::Lib::PixelPipeline::NormRgb & result ) override
    {
        m_pipe-> convert( val, result );
        result[0] *= m_maxRgb[0];
        result[1] *= m_maxRgb[1];
        result[2] *= m_maxRgb[2];
    }

    virtual void
    convertq( double val, QRgb & result ) override
    {
        m_pipe-> convertq( val, result );
        auto red = std::round( qRed( result) * m_maxRgb[0]);
        auto green = std::round(qGreen( result) * m_maxRgb[1]);
        auto blue = std::round(qBlue( result) * m_maxRgb[2]);
        result = qRgb( red, green, blue);
    }

    virtual void
    getClips( double & min, double & max ) override
    {
        min = m_clipMin;
        max = m_clipMax;
    }

    QString
    cacheId()
    {
        return QString( "%1/%2/%3/%4/%5/%6/%7/" )
                   .arg( m_cmapName )
                   .arg( m_invertFlag )
                   .arg( m_reverseFlag )
                   .arg( m_scaleStage-> cacheId() )
                   .arg( double2base64( m_clipMin ) )
                   .arg( double2base64( m_clipMax ) )
                   .arg( QString::number(m_maxRgb[0])+QString::number(m_maxRgb[1])+QString::number(m_maxRgb[2]));
    }

private:

    Composite::UniquePtr m_pipe = nullptr;
    ScaleStage::SharedPtr m_scaleStage = nullptr;
    ReversableStage1::SharedPtr m_reversible = nullptr;
    InvertibleStage4::SharedPtr m_invertible = nullptr;
    double m_clipMin = 0, m_clipMax = 1;
    NormRgb m_maxRgb {{ 1.0, 1.0, 1.0}};

    QString m_cmapName;
    bool m_invertFlag = false, m_reverseFlag = false;
};
}
}
}
