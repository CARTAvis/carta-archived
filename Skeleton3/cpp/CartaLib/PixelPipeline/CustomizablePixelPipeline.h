/**
 *
 **/

#pragma once

#include "IPixelPipeline.h"

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
class InvertibleStage4 : public INormRgb2NormRgb
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


/// this is the pipeline that we use in CARTA
/// it should support all of the GUI actions of a colormap dialog, e.g.:
/// - invert, reverse, colormap, log/gamma/cycles, manual clip
///
/// \todo add missing functionality, e.g. gamma,log,power,sqrt, etc
///
class CustomizablePixelPipeline : public IClippedPixelPipeline
{
    CLASS_BOILERPLATE( CustomizablePixelPipeline );

public:

    CustomizablePixelPipeline()
    {
        m_pipe.reset( new Composite() );
        m_reversible = std::make_shared < ReversableStage1 > ();
        m_pipe-> addStage1( m_reversible );
        m_invertible = std::make_shared < InvertibleStage4 > ();
        m_pipe-> addStage4( m_invertible );
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
    }

    virtual void
    convertq( double val, QRgb & result ) override
    {
        m_pipe-> convertq( val, result );
    }

    virtual void
    getClips( double & min, double & max) override
    {
        min = m_clipMin;
        max = m_clipMax;
    }

    QString cacheId() {
        auto d2hex = [] (double x) -> QString {
            return QByteArray( (char *)(& x), sizeof(x)).toBase64();
        };

        return QString( "%1/%2/%3/%4/%5")
                .arg( m_cmapName )
                .arg( m_invertFlag)
                .arg( m_reverseFlag)
                .arg( d2hex(m_clipMin))
                .arg( d2hex(m_clipMax));
    }

private:

    Composite::UniquePtr m_pipe;
    ReversableStage1::SharedPtr m_reversible;
    InvertibleStage4::SharedPtr m_invertible;
    double m_clipMin = 0, m_clipMax = 1;

    QString m_cmapName;
    bool m_invertFlag = false, m_reverseFlag = false;
};
}
}
}
