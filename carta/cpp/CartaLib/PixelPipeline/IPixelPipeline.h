#pragma once

#include "CartaLib/CartaLib.h"
#include <QRgb>
#include <stdexcept>
#include <cmath>
#include <array>

namespace Carta
{
namespace Lib
{
/// Code related to converting raw pixels to RGB
/// this is non-templated, specialized for double
namespace PixelPipeline
{
/// conversion of raw pixels (double) to QRgb (8 bit color/rgb channel)
///
/// NaN takes a different path, the following are for non-nans
///
/// - stage 0: double -> double (clamping)
///   - this is where values are clamped to fall between clip values
///   - not configurable
/// - stage 1: double -> double
///   - this can have multiple double -> double steps
///   - min/max is carried through
///   - configurable (reverse,scale eg. power,log)
/// - stage 2: normalization double -> [0..1]
///    - not configurable
/// - stage 3: [0..1] -> DRgb
///   - this is a single step
///   - configurable
/// - stage 4: Drgb -> Drgb
///   - can have mulitple sub-steps
///   - configurable
/// - stage 5: Drgb -> QRgb
///   - simple conversion to 8 bit values using auto-rounding
///   - not configurable
///
/// Setup for a changed min/max
///
/// for s : stage1
///   min,max = s.setMinMax( min, max)
/// stage2.setMinMax( min,max)
///
/// after setting up min/max, here is the algorithm for converting val(double) to QRgb
///
/// val = input
/// val = stage0(val) // clamp
/// for s : stage1
///   val = s(val)
/// val = stage2(val)
/// DRgb drgb = stage3(val)
/// for s : stage4
///   drgb = s(drgb)
/// QRgb qrgb = stage5(drgb)
/// return qrgb

/// RGB as triplet of doubles in range 0..1
typedef std::array < double, 3 > NormRgb;
typedef double norm_double;

/// generic double->double converter
class Id2d
{
    CLASS_BOILERPLATE( Id2d );

public:

    /// in-place conversion
    virtual void
    convert( double & val ) = 0;

    virtual
    ~Id2d() { }
};

/// interface for implementing stage 1 (double-> double)
/// we add here the concept of min/max
class IStage1 : public Id2d
{
    CLASS_BOILERPLATE( IStage1 );

public:

    virtual void
    setMinMax( double & min, double & max ) = 0;
};

/// interface for implementing stage 3 (normalized double -> normalized RGB)
/// this is the most generic colormap
class IColormap
{
    CLASS_BOILERPLATE( IColormap );

public:

    /// in-place conversion
    virtual void
    convert( norm_double val, NormRgb & result ) = 0;

    virtual
    ~IColormap() { }
};

/// IColormap with a name api
///
/// This is the interface that plugins have to implement to add a new colormap for
/// scalcar-type pixels.
class IColormapNamed : public PixelPipeline::IColormap
{
    CLASS_BOILERPLATE( IColormapNamed );

public:

    using norm_double = PixelPipeline::norm_double;
    using NormRgb = PixelPipeline::NormRgb;

    virtual QString
    name() = 0;
};


/// primitive gray scale colormap
class GrayCMap : public IColormap
{
    CLASS_BOILERPLATE( GrayCMap );

public:

    /// conversion where calaler supplies storage for result
    virtual void
    convert( norm_double val, NormRgb & result ) override
    {
        result.fill( val );
    }
};

/// interface for implementing stage 4 (norm rgb -> norm rgb)
class INormRgb2NormRgb
{
    CLASS_BOILERPLATE( INormRgb2NormRgb );

public:

    /// in-place conversion
    virtual void
    convert( NormRgb & drgb ) = 0;

    virtual
    ~INormRgb2NormRgb() { }
};

/// minimal interface representing an implemented pipeline
class IPixelPipeline
{
    CLASS_BOILERPLATE( IPixelPipeline );

public:

    /// do the conversion double -> normalized (0..1) RGB, i.e. up to stage 4
    virtual void
    convert( double val, NormRgb & result ) = 0;

    /// do the conversion double -> 8 bit RGB, i.e. up to stage 5
    virtual void
    convertq( double val, QRgb & result ) = 0;

    /// returns the input clip range
    /// \note this is not strictly necessary for minimalist interface, but we do use
    /// this just about everywhere where we need IPixelPipeline for caching, so I stuck
    /// it in here. We could break it apart into IPixelPipeline and IClippedPixelPipeline
    /// in the future...

    virtual
    ~IPixelPipeline() { }
};

class IClippedPixelPipeline : public IPixelPipeline
{
    CLASS_BOILERPLATE( IClippedPixelPipeline);

public:

    virtual void getClips( double & min, double & max) = 0;
};

/// composite function for converting pixels to rgb
/// can have multiple stage 1 (double->double) functions, and one stage 2 (double->rgb),
/// followed by multiple rgb2rgb
class Composite : public IPixelPipeline
{
    CLASS_BOILERPLATE( Composite );

public:

    // e.g. log scaling, gamma correction
    void
    addStage1( IStage1::SharedPtr func )
    {
        m_stage1.push_back( func );
    }

    // e.g. colormap
    void
    setStage3( IColormap::SharedPtr func )
    {
        m_stage3 = func;
    }

    // e.g. invert
    void
    addStage4( INormRgb2NormRgb::SharedPtr func )
    {
        m_stage4.push_back( func );
    }

    void
    setMinMax( double min, double max )
    {
        m_min = min;
        m_max = max;
        for ( auto & s : m_stage1 ) {
            s-> setMinMax( m_min, m_max );
        }
    }

    virtual void
    convert( double p_val, NormRgb & result ) override
    {
        CARTA_ASSERT( ! std::isnan( p_val ) );
        CARTA_ASSERT( m_stage3 );

        // stage 0: clamp
        double val = Carta::Lib::clamp( p_val, m_min, m_max );

        // stage 1: scalar to scalar
        for ( auto & s : m_stage1 ) {
            s-> convert( val );
        }

        // stage 2: normalize
        val = ( val - m_min ) / ( m_max - m_min );

        // stage 3: scalar -> drgb
        m_stage3-> convert( val, result );

        // stage 4: drgb -> drgb
        for ( auto & s : m_stage4 ) {
            s-> convert( result );
        }
    } // convert

    virtual void
    convertq( double p_val, QRgb & result ) override
    {
        NormRgb drgb;
        convert( p_val, drgb );

        /// stage 5:
        result = qRgb( drgb[0] * 255 , drgb[1] * 255 ,  drgb[2] * 255 );
    } // convert

protected:

//    typedef Id2d Stage1;
    typedef IColormap Stage3;
    typedef INormRgb2NormRgb Stage4;

    std::vector < IStage1::SharedPtr > m_stage1;
    Stage3::SharedPtr m_stage3 = std::make_shared < GrayCMap > ();
    std::vector < Stage4::SharedPtr > m_stage4;

    double m_min = 0, m_max = 1; // needed for stage 3 (normalization)
};

/// adapter for implementing double -> double from std::function
class FunctionD2DAdapter : public Id2d
{
public:

    FunctionD2DAdapter( std::function < double (double) > func )
        : m_func( func )
    { }

//    virtual void
//    setMinMax( double & min, double & max ) override
//    {
//        Q_UNUSED( min );
//        Q_UNUSED( max );
//    }

    virtual void
    convert( double & val ) override
    {
        val = m_func( val );
    }

protected:

    std::function < double (double) > m_func;
};

/// convenience function to convert normalized RGB to 8-bit RGB
inline void
normRgb2QRgb( const NormRgb & drgb, QRgb & result )
{
    result = qRgb( drgb[0] * 255, drgb[1] * 255 , drgb[2] * 255 );;
}

/// algorithm for caching a double->rgb function
/// it's templated over 'interpolated' flag
template < bool interpolated>
class CachedPipeline /*: public Id2NormQRgb*/
{
    CLASS_BOILERPLATE( CachedPipeline );

public:

    typedef IPixelPipeline Func;

    /// creates a default cache
    CachedPipeline() { }

    /// \brief create a cached version of the supplied function
    /// \param funcToCache function to cache
    /// \param nSegments how many segments to create for caching
    /// \param min minimum value
    /// \param max maximum value
    /// \return the cached version, caller assumes ownership
    /// @warning funcToCache should already be prepped with min/max if applicable
    void
    cache( Func & funcToCache, int64_t nSegments, double min, double max )
    {
        CARTA_ASSERT( nSegments > 1 );
        CARTA_ASSERT( min < max );
        m_min = min;
        m_max = max;
        m_cache.resize( nSegments );
        double delta = ( max - min ) / ( nSegments - 1 );
        for ( int64_t i = 0 ; i < nSegments ; i++ ) {
            double x = min + i * delta;
            funcToCache.convert( x, m_cache[i] );
        }

        // pre-cache some stuff
        m_n1 = m_cache.size() - 1;
        m_d = ( m_max - m_min ) / m_n1;
        m_dInvN1 = 1 / m_d;
    }

    void
    convert( double x, NormRgb & result );

    void
    convertq( double x, QRgb & result ) /*override*/
    {
        NormRgb drgb;
        convert( x, drgb );
        normRgb2QRgb( drgb, result );
    }

private:

    std::vector < NormRgb > m_cache;
//    NormRgb m_nanColor { { 1.0, 0.0, 0.0 } };
    double m_min = 0, m_max = 1;
    double m_d, m_dInvN1, m_n1;

};


template <>
inline void CachedPipeline<false>::convert(double x, NormRgb & result) /*override*/
{
    int64_t ind = round( ( x - m_min ) * m_dInvN1 );
    if ( Q_UNLIKELY( ind < 0 ) ) {
        result = m_cache[0];
        return;
    }
    if ( Q_UNLIKELY( ind >= m_n1 ) ) {
        result = m_cache.back();
        return;
    }
    result = m_cache[ind];
}

template <>
inline void CachedPipeline<true>::convert(double x, NormRgb & result) /*override*/
{
    double dind = ( x - m_min ) * m_dInvN1;

    if ( Q_UNLIKELY( dind < 0 ) ) {
        result = m_cache[0];
        return;
    }

    double intpart;
    double frac = std::modf (dind , & intpart);
    int64_t ind = intpart;

//    qDebug() << "intpart = " << intpart << "frac=" << frac << "ind=" << ind;

//    if ( Q_UNLIKELY( ind < 0 ) ) {
//        result = m_cache[0];
//        return;
//    }
    if ( Q_UNLIKELY( ind >= m_n1 ) ) {
        result = m_cache.back();
        return;
    }

    result[0] = m_cache[ind][0] * (1-frac) + m_cache[ind+1][0] * frac;
    result[1] = m_cache[ind][1] * (1-frac) + m_cache[ind+1][1] * frac;
    result[2] = m_cache[ind][2] * (1-frac) + m_cache[ind+1][2] * frac;
}


} // namespace PixelPipeline
} // namespace Lib
} // namespace Carta
