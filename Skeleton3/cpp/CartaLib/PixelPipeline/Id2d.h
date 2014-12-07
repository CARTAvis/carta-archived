#pragma once

#include "CartaLib/CartaLib.h"
#include <QRgb>
#include <stdexcept>

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
///   - not configurable
/// - stage 1: double -> double
///   - this can have multiple double -> double steps
///   - min/max is carried through
///   - configurable
/// - stage 2: normalization double -> [0..1]
///    - not configurable
/// - stage 3: [0..1] -> DRgb
///   - this is a single step
///   - configurable
/// - stage 4: Drgb -> Drgb
///   - can have mulitple sub-steps
///   - configurable
/// - stage 5: Drgb -> QRgb
///   - simple conversion to 8 bit values using rounding
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
/// but as a stage in the staged pipeline, so it needs concept of min/max
class IStage1 : public Id2d
{
    CLASS_BOILERPLATE( IStage1 );

public:

    virtual void
    setMinMax( double & min, double & max ) = 0;
};

/// interface for implementing stage 3 (normalized double -> DRgb)
/// this is the most generic colormap
class Inormd2NormRgb
{
    CLASS_BOILERPLATE( Inormd2NormRgb );

public:

    /// in-place conversion
    virtual void
    convert( norm_double val, NormRgb & result ) = 0;

    virtual
    ~Inormd2NormRgb() { }
};

typedef Inormd2NormRgb IColormap;

/// primitive gray scale colormap
class GrayCMap : public IColormap
{
    CLASS_BOILERPLATE( GrayCMap );

public:

    virtual void
    convert( norm_double val, NormRgb & result ) override
    {
        result.fill( val );
    }
};

/// interface for implementing stage 4 (Drgb -> Drgb)
class INormRgb2NormRgb
{
    CLASS_BOILERPLATE( INormRgb2NormRgb );

public:

    virtual void
    convert( NormRgb & drgb ) = 0;

    virtual
    ~INormRgb2NormRgb() { }
};

/// the minimal interface representing an implemented pipeline
/// - if we ever wanted to replace the whole pipeline
class Id2NormQRgb
{
    CLASS_BOILERPLATE( Id2NormQRgb );

public:

//    virtual void
//    setMinMax( double min, double max ) = 0;

    /// do the conversion up to stage 4
    virtual void
    convert( double val, NormRgb & result ) = 0;

    /// do the conversion including stage 5
    virtual void
    convertq( double val, QRgb & result ) = 0;

    virtual
    ~Id2NormQRgb() { }
};

/// composite function for converting pixels to rgb
/// can have multiple stage 1 (double->double) functions, and one stage 2 (double->rgb),
/// followed by multiple rgb2rgb
class CompositeD2QRgb : public Id2NormQRgb
{
    CLASS_BOILERPLATE( CompositeD2QRgb );

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

    virtual void
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
        double val = clamp( p_val, m_min, m_max );

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
        result = qRgb( round( drgb[0] * 255 ), round( drgb[1] * 255 ), round( drgb[2] * 255 ) );
    } // convert

protected:

//    typedef Id2d Stage1;
    typedef Inormd2NormRgb Stage3;
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

inline void
drgb2qrgb( const NormRgb & drgb, QRgb & result )
{
    result = qRgb( round( drgb[0] * 255 ), round( drgb[1] * 255 ), round( drgb[2] * 255 ) );
}

/// algorithm for caching a double->rgb function
template < class Func >
class CachedPipeline : public Id2NormQRgb
{
    CLASS_BOILERPLATE( CachedPipeline );

public:

    /// creates a default cache
    CachedPipeline() { }

    /// \brief create a cached version of the supplied function
    /// \param funcToCache function to cache
    /// \param nSegments how many segments to create for caching
    /// \param min minimum value
    /// \param max maximum value
    /// \return the cached version, caller assumes ownership
    /// @warning funcToCache's state may be changed during the process
    void
    cache( Func & funcToCache, int64_t nSegments, double min, double max )
    {
        CARTA_ASSERT( nSegments > 1 );
        CARTA_ASSERT( min < max );
        m_cache.resize( nSegments );
        double delta = ( max - min ) / ( nSegments - 1 );
        funcToCache.setMinMax( min, max );
        for ( int64_t i = 0 ; i < nSegments ; i++ ) {
            double x = min + i * delta;
            funcToCache.convert( x, m_cache[i] );
        }
    }

    std::vector < NormRgb > m_cache;
    NormRgb m_nanColor { { 1.0, 0.0, 0.0 } };
    double m_min = 0, m_max = 1;

    virtual void
    convert( double x, NormRgb & result ) override
    {
        double d = ( m_max - m_min ) / ( m_cache.size() - 1 );
        double dInvN1 = 1 / d;
        int64_t n1 = m_cache.size() - 1;
        int64_t ind = round( ( x - m_min ) * dInvN1 );
        if ( Q_UNLIKELY( ind < 0 ) ) {
            result = m_cache[0];
            return;
        }
        if ( Q_UNLIKELY( ind > n1 ) ) {
            result = m_cache.back();
            return;
        }
        result = m_cache[ind];
    }

    virtual void
    convertq( double x, QRgb & result ) override
    {
        NormRgb drgb;
        convert( x, drgb );
        drgb2qrgb( drgb, result );
    }

    virtual void
    setMinMax( double min, double max ) override
    {
        Q_UNUSED( min );
        Q_UNUSED( max );
        throw std::runtime_error( "probably not want to call this" );
    }
};

/// reversable implementing stage1
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

/// customizable pipeline:
/// - invert, reverse, colormap, log/gamma/cycles, manual clip
class CustomizablePipeline : public Id2NormQRgb
{
    CLASS_BOILERPLATE( CustomizablePipeline );

public:

    CustomizablePipeline()
    {
        m_pipe.reset( new CompositeD2QRgb() );
        m_reversible = std::make_shared < ReversableStage1 > ();
        m_pipe-> addStage1( m_reversible );
        m_invertible = std::make_shared < InvertibleStage4 > ();
        m_pipe-> addStage4( m_invertible );
    }

    void
    setInvert( bool flag )
    {
        m_invertible-> setInverted( flag );
    }

    void
    setReverse( bool flag )
    {
        m_reversible-> setReversed( flag );
    }

    void
    setColormap( Inormd2NormRgb::SharedPtr colormap ) {
        m_pipe-> setStage3( colormap);
    }

//    void
//    setScalingFunc( IStage1::SharedPtr scaleFunc ) { }

    virtual void
    setMinMax( double min, double max )
    {
        m_pipe-> setMinMax( min, max);
    }

    virtual void
    convert( double val, Carta::Lib::PixelPipeline::NormRgb & result )
    {
        m_pipe-> convert( val, result);
    }

    virtual void
    convertq( double val, QRgb & result )
    {
        m_pipe-> convertq( val, result);
    }

private:

    CompositeD2QRgb::UniquePtr m_pipe;
    ReversableStage1::SharedPtr m_reversible;
    InvertibleStage4::SharedPtr m_invertible;
};
} // namespace PixelPipeline
} // namespace Lib
} // namespace Carta
