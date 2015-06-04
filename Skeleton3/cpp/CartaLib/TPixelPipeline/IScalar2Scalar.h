/// \file
///
/// This is just some branstorming ideas I had. Nothing from this file is currently used
/// anywhere.

#pragma once

#ifdef DONT_COMPILE

#include "CartaLib/CartaLib.h"

namespace Carta
{
namespace Lib
{
/// same as PixelPipeline but templated
/// this is more customizable, but I'm not sure how well it would work across
/// shared library boundaries
namespace TemplatedPixelPipeline
{
/// generic scalar RGB
template < typename Scalar >
using ScalarRgb = std::array < Scalar, 3 >;

/// RGB as triplet of doubles in range 0..1
typedef ScalarRgb < double > DRgb;

/// generic scalar -> scalar (Stage 1) component
template < typename Scalar >
class IScalar2Scalar
{
    CLASS_BOILERPLATE( IScalar2Scalar );

public:

    virtual void
    setMinMax( Scalar & min, Scalar & max ) = 0;

    virtual void
    convert( Scalar & val ) = 0;

    virtual
    ~IScalar2Scalar() { }
};

/// interface for implementing stage 3 (normalized scalar -> ScalarRgb)
/// this is the most generic colormap
template < typename InputScalar, typename OutputScalar >
class IScalar2ScalarRgb
{
    CLASS_BOILERPLATE( IScalar2ScalarRgb );

public:

    virtual void
    convert( const InputScalar & val, ScalarRgb < OutputScalar > & result ) = 0;

    virtual
    ~IScalar2ScalarRgb() { }
};

/// primitive gray scale colormap
template < typename InputScalar, typename OutputScalar >
class GrayCMap : public IScalar2ScalarRgb < InputScalar, OutputScalar >
{
    CLASS_BOILERPLATE( GrayCMap );

public:

    virtual void
    convert( const InputScalar & val, ScalarRgb < OutputScalar > & result ) override
    {
        result.fill( static_cast < OutputScalar > ( val ) );
    }
};

/// interface for implementing stage 4 (ScalarRgb -> ScalarRgb)
template < typename Scalar >
class IScalarRgb2ScalarRgb
{
    CLASS_BOILERPLATE( IScalarRgb2ScalarRgb );

public:

    virtual void
    convert( ScalarRgb < Scalar > & srgb ) = 0;

    virtual
    ~IScalarRgb2ScalarRgb() { }
};

/// the minimal interface representing an implemented pipeline
/// - if we ever wanted to replace the whole pipeline
template < typename InputScalar, typename OutputScalar >
class IScalar2ScalarRgbAndQRgb
{
    CLASS_BOILERPLATE( IScalar2ScalarRgbAndQRgb );

public:

    virtual void
    setMinMax( InputScalar min, InputScalar max ) = 0;

    /// do the conversion up to stage 4
    virtual void
    convert( InputScalar val, ScalarRgb < OutputScalar > & result ) = 0;

    /// do the conversion including stage 5
    virtual void
    convert( InputScalar val, QRgb & result ) = 0;

    virtual
    ~IScalar2ScalarRgbAndQRgb() { }
};

/// composite function for converting pixels to rgb
/// can have multiple stage 1 (double->double) functions, and one stage 2 (double->rgb),
/// followed by multiple rgb2rgb
template < typename InputScalar, typename OutputScalar >
class Composite : public IScalar2ScalarRgbAndQRgb < InputScalar, OutputScalar >
{
    CLASS_BOILERPLATE( Composite );

public:

    typedef IScalar2Scalar < InputScalar > Stage1;
    typedef IScalar2ScalarRgb < InputScalar, OutputScalar > Stage3;
    typedef IScalarRgb2ScalarRgb < OutputScalar > Stage4;

    // e.g. log scaling, gamma correction
    void
    addStage1( typename Stage1::SharedPtr func )
    {
        m_stage1.push_back( func );
    }

    // e.g. colormap
    void
    setStage3( typename Stage3::SharedPtr func )
    {
        m_stage3 = func;
    }

    // e.g. invert
    void
    addStage4( typename Stage4::SharedPtr func )
    {
        m_stage4.push_back( func );
    }

    virtual void
    setMinMax( InputScalar min, InputScalar max ) override
    {
        m_min = min;
        m_max = max;
        for ( auto & s : m_stage1 ) {
            s-> setMinMax( m_min, m_max );
        }
    }

    virtual void
    convert( InputScalar p_val, ScalarRgb < OutputScalar > & result ) override
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
    convert( InputScalar p_val, QRgb & result ) override
    {
        CARTA_ASSERT( ! std::isnan( p_val ) );

        ScalarRgb < OutputScalar > drgb;
        convert( p_val, drgb );

        /// stage 5:
        result = qRgb( round( drgb[0] * 255 ), round( drgb[1] * 255 ), round( drgb[2] * 255 ) );
    } // convert

protected:

    std::vector < typename Stage1::SharedPtr > m_stage1;
    typename Stage3::SharedPtr m_stage3;
    std::vector < typename Stage4::SharedPtr > m_stage4;

    InputScalar m_min = 0, m_max = 1; // needed for stage 3 (normalization)
};

/*void
test2()
{
    Composite < double, float > composite;
}*/

/// adapter for implementing double -> double from std::function
template < typename Scalar >
class FunctionD2DAdapter : public IScalar2Scalar < Scalar >
{
public:

    FunctionD2DAdapter( std::function < Scalar( Scalar ) > func )
        : m_func( func )
    { }

    virtual void
    setMinMax( Scalar & min, Scalar & max ) override
    {
        Q_UNUSED( min );
        Q_UNUSED( max );
    }

    virtual void
    convert( Scalar & val ) override
    {
        val = m_func( val );
    }

protected:

    std::function < Scalar( Scalar ) > m_func;
};

/*void
test3()
{
    FunctionD2DAdapter < double > adapter([] (double x) { return 1 - x;
                                          }
                                          );
    double x = 7;
    adapter.convert( x );
}*/

/// algorithm for caching a double->rgb function
template < typename InputScalar, typename OutputScalar >
class CachedD2Rgb : public IScalar2ScalarRgbAndQRgb < InputScalar, OutputScalar >
{
    CLASS_BOILERPLATE( CachedD2Rgb );

public:

//    static IScalar2ScalarRgbAndQRgb <  *
//    create( IScalar2ScalarRgbAndQRgb & funcToCache, int nSegments );
};

/// default implementation of pixel transfer
/// basically just applies gray colormap
template < typename InputScalar, typename OutputScalar >
class DefaultPipeline : public IScalar2ScalarRgbAndQRgb < InputScalar, OutputScalar >
{
public:

    DefaultPipeline()
    {
        m_comp = std::make_shared < Comp > ();
        m_comp-> setStage3( std::make_shared < GrayCMap < InputScalar, OutputScalar > > () );
    }

    virtual void
    setMinMax( double min, double max )
    {
        m_comp-> setMinMax( min, max );
    }

    virtual void
    convert( InputScalar val, ScalarRgb < OutputScalar > & result ) override
    {
        m_comp-> convert( val, result );
    }

    virtual void
    convert( InputScalar val, QRgb & result ) override
    {
        m_comp-> convert( val, result );
    }

protected:

    typedef Composite < InputScalar, OutputScalar > Comp;
    typename Comp::SharedPtr m_comp;
};

/*void
test4()
{
    DefaultPipeline < double, double > pipe;
    pipe.setMinMax( 1.0, 2.0 );
    ScalarRgb < double > drgb;
    pipe.convert( 1.4, drgb );

    IScalar2Scalar < int >::SharedPtr ptr;
    Composite < int, int >::SharedPtr x;
}*/
}
}
}

#endif
