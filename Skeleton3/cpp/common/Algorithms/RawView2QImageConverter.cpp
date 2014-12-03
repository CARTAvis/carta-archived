/// \todo This is getting way too object oriented. We should probably refactor this into

/// pure template implementation to regain some speed via vectorization, at least for
/// the cached pixel transfer function case.

#include "CartaLib/CartaLib.h"
#include "GrayColormap.h"
#include "RawView2QImageConverter.h"

#include <functional>
#include <array>

namespace ScalarPixelToRgb
{
/// conversion of raw pixel (double) to QRgb (8 bit color/rgb channel)
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

/// interface for implementing stage 1 (double-> double)
class Id2d
{
    CLASS_BOILERPLATE( Id2d );

public:

    virtual void
    setMinMax( double & min, double & max ) = 0;

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

    virtual void convert(const double & val, DRgb & result) override
    {
        result.fill( val);
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
    convert( const double & val, QRgb & result ) = 0;

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
    convert( const double & p_val, QRgb & result ) override
    {
        CARTA_ASSERT( ! std::isnan( p_val ) );

        DRgb drgb;
        convert( p_val, drgb );

        /// stage 5:
        result = qRgb( round( drgb[0] * 255 ), round( drgb[1] * 255 ), round( drgb[2] * 255 ) );
    } // convert

protected:

    typedef Id2d       Stage1;
    typedef Id2DRgb    Stage3;
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

/// algorithm for caching a double->rgb function
class CachedD2Rgb : public Id2DQRgb
{
    CLASS_BOILERPLATE( CachedD2Rgb );

public:

    static Id2DQRgb *
    create( Id2DQRgb & funcToCache, int nSegments );
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
    { }

    virtual void
    convert( const double & val, DRgb & result ) override
    { }

    virtual void
    convert( const double & val, QRgb & result ) override
    { }

protected:

    CompositeD2QRgb::SharedPtr m_comp;
};
}

/// compute clip values from the values in a view
template < typename Scalar >
static
typename std::tuple < Scalar, Scalar >
computeClips(
    NdArray::TypedView < Scalar > & view,
    double perc
    )
{
    // read in all values from the view into an array
    // we need our own copy because we'll do quickselect on it...
    std::vector < Scalar > allValues;
    view.forEach(
        [& allValues] ( const Scalar & val ) {
            if ( std::isfinite( val ) ) {
                allValues.push_back( val );
            }
        }
        );

    // indicate bad clip if no finite numbers were found
    if ( allValues.size() == 0 ) {
        return std::make_tuple(
                   std::numeric_limits < Scalar >::quiet_NaN(),
                   std::numeric_limits < Scalar >::quiet_NaN() );
    }
    Scalar clip1, clip2;
    Scalar hist = ( 1.0 - perc ) / 2.0;
    int x1 = allValues.size() * hist;
    std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end() );
    clip1 = allValues[x1];
    x1 = allValues.size() - 1 - x1;
    std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end() );
    clip2 = allValues[x1];

    if ( 0 ) {
        // debug
        int64_t count = 0;
        for ( auto & x : allValues ) {
            if ( x >= clip1 && x <= clip2 ) {
                count++;
            }
        }
        qDebug() << "autoClip(" << perc << ")=" << count * 100.0 / allValues.size();
    }

    return std::make_tuple( clip1, clip2 );
} // computeClips

struct RawView2QImageConverter::CachedImage {
    QImage img;
    double clip1, clip2, perc;
    bool   specificToFrame;

    CachedImage( QImage pImg, double c1, double c2, bool spec, double p )
    {
        img = pImg;
        clip1 = c1;
        clip2 = c2;
        specificToFrame = spec;
        perc = p;
    }
};

RawView2QImageConverter::RawView2QImageConverter()
{
    // set cache size (in MB)
//    m_cache.reset( new QCache < int, CachedImage > () );
    m_cache.setMaxCost( 6 * 1024 );
    m_cmap = std::make_shared < Carta::Core::GrayColormap > ();
}

RawView2QImageConverter::~RawView2QImageConverter()
{ }

RawView2QImageConverter::Me &
RawView2QImageConverter::setView( NdArray::RawViewInterface * rawView )
{
    if ( ! rawView ) {
        throw std::runtime_error( "iimage is null" );
    }
    if ( rawView->dims().size() < 2 ) {
        throw std::runtime_error( "need at least 2 dimensions" );
    }
    m_rawView = rawView;
    return * this;
}

RawView2QImageConverter::Me &
RawView2QImageConverter::setFrame( int frame )
{
    if ( frame > 0 ) {
        int nFrames = 0;
        if ( m_rawView->dims().size() > 2 ) {
            nFrames = m_rawView->dims()[2];
        }
        if ( frame >= nFrames ) {
            throw std::runtime_error( "not enough frames in this image" );
        }
    }
    return * this;
}

RawView2QImageConverter::Me &
RawView2QImageConverter::setAutoClip( double val )
{
    m_autoClip = val;
    return * this;
}

RawView2QImageConverter::Me &
RawView2QImageConverter::setColormap( Carta::Lib::IColormapScalar::SharedPtr cmap )
{
    m_cmap = cmap;
    m_cache.clear();
    return * this;
}

const QImage &
RawView2QImageConverter::go( int frame, bool recomputeClip )
{
    // let's see if we have the result in cache
    CachedImage * img = m_cache.object( frame );
    if ( img ) {
        bool match = false;

        // we have something in cache, let's see if it matches what we want
        if ( recomputeClip ) {
            if ( img->specificToFrame && img->perc == m_autoClip ) {
                match = true;
            }
        }
        else if ( img->clip1 == m_clip1 && img->clip2 == m_clip2 ) {
            match = true;
        }

        // if we found a match, return it and we're done
        if ( match ) {
            qDebug() << "Cache hit #" << frame;
            return img->img;
        }
    }
    qDebug() << "Cache miss #" << frame;

    NdArray::TypedView < Scalar > view( m_rawView, false );

    if ( ! std::isfinite( m_clip1 ) || recomputeClip ) {
        std::tie( m_clip1, m_clip2 ) = computeClips < Scalar > ( view, m_autoClip );
    }
    int width = m_rawView->dims()[0];
    int height = m_rawView->dims()[1];

    // make sure there is at least 1 finite value, if not, return a black image
    if ( ! std::isfinite( m_clip1 ) ) {
        // there were no values.... return a black image
        m_qImage = QImage( width, height, QImage::Format_ARGB32 );
        m_qImage.fill( 0 );
        return m_qImage;
    }

    // construct image using clips (clipd, clipinv)
    m_qImage = QImage( width, height, QImage::Format_ARGB32 );
    auto bytesPerLine = m_qImage.bytesPerLine();
    if ( bytesPerLine != width * 4 ) {
        throw std::runtime_error( "qimage does not have consecutive memory..." );
    }
    QRgb * outPtr = reinterpret_cast < QRgb * > ( m_qImage.bits() + width * ( height - 1 ) * 4 );

    // precalculate linear transformation coeff.
    Scalar clipdinv = 1.0 / ( m_clip2 - m_clip1 );

    // apply the clips
    CARTA_ASSERT( m_cmap != nullptr );
    int64_t counter = 0;
    auto lambda = [& outPtr, & clipdinv, & counter, & width, this] ( const Scalar & ival )
    {
        if ( std::isfinite( ival ) ) {
            Scalar val = ( ival - m_clip1 ) * clipdinv;
            val = clamp < Scalar > ( val, 0.0, 1.0 );
            * outPtr = m_cmap->convert( val );
        }
        else {
            * outPtr = qRgb( 255, 0, 0 );
        }
        outPtr++;
        counter++;

        // build the image bottom-up
        if ( counter % width == 0 ) {
            outPtr -= width * 2;
        }
    };
    view.forEach( lambda );

    // stick this into the cache so we don't need to recompute it
    m_cache.insert( frame,
                    new CachedImage( m_qImage, m_clip1, m_clip2, recomputeClip, m_autoClip ),
                    width * height * 4 / 1024 / 1024 );

    qDebug() << "cache total=" << m_cache.totalCost() << "MB";

    return m_qImage;
} // go
