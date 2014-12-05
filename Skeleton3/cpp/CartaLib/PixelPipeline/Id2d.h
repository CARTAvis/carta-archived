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
typedef std::array < double, 3 > DRgb;

/// interface for implementing stage 1 (double-> double)
class Id2d
{
    CLASS_BOILERPLATE( Id2d );

public:

    virtual void
    setMinMax( double & min, double & max ) = 0;

    /// in-place conversion
    virtual void
    convert( double & val ) = 0;

    virtual
    ~Id2d() { }
};

/// interface for implementing stage 3 (normalized double -> DRgb)
/// this is the most generic colormap
class Id2DRgb
{
    CLASS_BOILERPLATE( Id2DRgb );

public:

    /// in-place conversion
    virtual void
    convert( const double & val, DRgb & result ) = 0;

    virtual
    ~Id2DRgb() { }
};

/// primitive gray scale colormap
class GrayCMap : public Id2DRgb
{
    CLASS_BOILERPLATE( GrayCMap );

public:

    virtual void
    convert( const double & val, DRgb & result ) override
    {
        result.fill( val );
    }
};

/// interface for implementing stage 4 (Drgb -> Drgb)
class IDRgb2DRgb
{
    CLASS_BOILERPLATE( IDRgb2DRgb );

public:

    virtual void
    convert( DRgb & drgb ) = 0;

    virtual
    ~IDRgb2DRgb() { }
};

/// the minimal interface representing an implemented pipeline
/// - if we ever wanted to replace the whole pipeline
class Id2DQRgb
{
    CLASS_BOILERPLATE( Id2DQRgb );

public:

    virtual void
    setMinMax( double min, double max ) = 0;

    /// do the conversion up to stage 4
    virtual void
    convert( const double & val, DRgb & result ) = 0;

    /// do the conversion including stage 5
    virtual void
    convertq( const double & val, QRgb & result ) = 0;

    virtual
    ~Id2DQRgb() { }
};

/// composite function for converting pixels to rgb
/// can have multiple stage 1 (double->double) functions, and one stage 2 (double->rgb),
/// followed by multiple rgb2rgb
class CompositeD2QRgb : public Id2DQRgb
{
    CLASS_BOILERPLATE( CompositeD2QRgb );

public:

    // e.g. log scaling, gamma correction
    void
    addStage1( Id2d::SharedPtr func )
    {
        m_stage1.push_back( func );
    }

    // e.g. colormap
    void
    setStage3( Id2DRgb::SharedPtr func )
    {
        m_stage3 = func;
    }

    // e.g. invert
    void
    addStage4( IDRgb2DRgb::SharedPtr func )
    {
        m_stage4.push_back( func );
    }

    virtual void
    setMinMax( double min, double max ) override
    {
        m_min = min;
        m_max = max;
        for ( auto & s : m_stage1 ) {
            s-> setMinMax( m_min, m_max );
        }
    }

    virtual void
    convert( const double & p_val, DRgb & result ) override
    {
        CARTA_ASSERT( ! std::isnan( p_val ) );

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
    convertq( const double & p_val, QRgb & result ) override
    {
        CARTA_ASSERT( ! std::isnan( p_val ) );

        DRgb drgb;
        convert( p_val, drgb );

        /// stage 5:
        result = qRgb( round( drgb[0] * 255 ), round( drgb[1] * 255 ), round( drgb[2] * 255 ) );
    } // convert

protected:

    typedef Id2d Stage1;
    typedef Id2DRgb Stage3;
    typedef IDRgb2DRgb Stage4;

    std::vector < Stage1::SharedPtr > m_stage1;
    Stage3::SharedPtr m_stage3;
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

    virtual void
    setMinMax( double & min, double & max ) override
    {
        Q_UNUSED( min );
        Q_UNUSED( max );
    }

    virtual void
    convert( double & val ) override
    {
        val = m_func( val );
    }

protected:

    std::function < double (double) > m_func;
};

/// default implementation of pixel transfer
/// basically just applies gray colormap
class DefaultD2Rgb : public Id2DQRgb
{
public:

    DefaultD2Rgb()
    {
        m_comp = std::make_shared < CompositeD2QRgb > ();
        m_comp-> setStage3( std::make_shared < GrayCMap > () );
    }

    virtual void
    setMinMax( double min, double max )
    {
        m_comp-> setMinMax( min, max );
    }

    virtual void
    convert( const double & val, DRgb & result ) override
    {
        m_comp-> convert( val, result );
    }

    virtual void
    convertq( const double & val, QRgb & result ) override
    {
        m_comp-> convertq( val, result );
    }

protected:

    CompositeD2QRgb::SharedPtr m_comp;
};

void drgb2qrgb( const DRgb & drgb, QRgb & result) {
    result = qRgb( round( drgb[0] * 255 ), round( drgb[1] * 255 ), round( drgb[2] * 255 ) );
}


/// algorithm for caching a double->rgb function
class CachedD2Rgb : public Id2DQRgb
{
    CLASS_BOILERPLATE( CachedD2Rgb );

public:

    /// creates a default cache
    CachedD2Rgb() {}

    /// \brief create a cached version of the supplied function
    /// \param funcToCache function to cache
    /// \param nSegments how many segments to create for caching
    /// \param min minimum value
    /// \param max maximum value
    /// \return the cached version, caller assumes ownership
    /// @warning funcToCache's state may be changed during the process
    template < typename Func >
    void cache( Func & funcToCache, int64_t nSegments, double min, double max )
    {
        CARTA_ASSERT( nSegments > 1);
        CARTA_ASSERT( min < max);
        m_cache.resize( nSegments);
        double delta = (max - min) / (nSegments-1);
        funcToCache.setMinMax( min, max);
        for( int64_t i = 0 ; i < nSegments ; i ++ ) {
            double x = min + i * delta;
            funcToCache.convert( x, m_cache[i] );
        }
    }

    std::vector<DRgb> m_cache;
    DRgb m_nanColor {{ 1.0, 0.0, 0.0 }};
    double m_min = 0, m_max = 1;

    virtual void convert( const double & x, DRgb & result) override {
        double d = (m_max - m_min) / (m_cache.size() - 1);
        double dInvN1 = 1 / d;
        int64_t n1 = m_cache.size() - 1;
        int64_t ind = round( ( x - m_min ) * dInvN1 );
        if ( ind < 0 ) { result = m_cache[0]; return; }
        if ( ind > n1 ) { result = m_cache.back(); return; }
        result = m_cache[ind];
    }

    virtual void convertq( const double & x, QRgb & result) override {
        DRgb drgb;
        convert( x, drgb);
        drgb2qrgb( drgb, result);
    }

    virtual void setMinMax(double min, double max) override
    {
        Q_UNUSED( min);
        Q_UNUSED( max);
        throw std::runtime_error( "probably not want to call this");
    }
};


} // namespace PixelPipeline
} // namespace Lib
} // namespace Carta
