#include "QImagePlugin.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/Initialize.h"
#include <QDebug>
#include <memory>
#include <algorithm>
#include <vector>

typedef Carta::Lib::HtmlString HtmlString;
typedef Carta::Lib::AxisInfo AxisInfo;

/// shortcut for LoadAstroImage
typedef Carta::Lib::Hooks::LoadAstroImage LoadAstroImage;
typedef Carta::Lib::Hooks::Initialize Initialize;

// forward declartion so we can use references/pointers to QImageII
class QImageII;

/// our implementation of Carta::Lib::NdArray::RawViewInterface
class QImageRawView
    : public Carta::Lib::NdArray::RawViewInterface
{
public:

    QImageRawView() = delete;

    // construct a view from raw gray data for the specified slice
    // the original data has dimesions 'dims'
    QImageRawView( std::shared_ptr < std::vector < unsigned char > > data,
                   const VI & dims,
                   const SliceND & sliceInfo )
    {
        // remember the data (shallow copy)
        m_data = data;
        m_rawData = & data-> at( 0 );

        // remember original dimensions of the data
        m_origDims = dims;

        // figure out what data to extract for each of the dimensions
        m_appliedSlice = sliceInfo.apply( dims );

        // cache the dimensions of the resulting view
        for ( auto & x : m_appliedSlice.dims() ) {
            m_viewDims.push_back( x.count );
        }
    }

    // similar to the first constructor, but the view is for an applied slice
    QImageRawView( std::shared_ptr < std::vector < unsigned char > > data,
                   const VI & dims,
                   const SliceND::ApplyResult & applyResult )
    {
        // remember the data (shallow copy)
        m_data = data;
        m_rawData = & data-> at( 0 );

        // remember original dimensions of the data
        m_origDims = dims;

        // figure out what data to extract for each of the dimensions
        m_appliedSlice = applyResult;

        // cache the dimensions of the resulting view
        for ( auto & x : m_appliedSlice.dims() ) {
            m_viewDims.push_back( x.count );
        }
    }

    virtual PixelType
    pixelType() override
    {
        return PixelType::Real32;
    }

    virtual const VI &
    dims() override
    {
        return m_viewDims;
    }

    virtual const char *
    get( const VI & pos ) override
    {
        int x = pos[0];
        int y = pos[1];

        m_floatBuff = float (m_rawData[x + y * m_origDims[0]]) / float (255.0);
        return reinterpret_cast < const char * > ( & m_floatBuff );
    }

    virtual void
    forEach( std::function < void (const char *) > func, Traversal traversal ) override
    {
        if ( traversal != Carta::Lib::NdArray::RawViewInterface::Traversal::Sequential ) {
            qFatal( "sorry, not implemented yet" );
        }

        const std::vector < Slice1D::ApplyResult > & dims = m_appliedSlice.dims();

        int y = dims[1].start;
        for ( int yc = 0 ; yc < dims[1].count ; ++yc ) {
            unsigned char * row = & m_rawData[m_origDims[0] * y];
            int x = dims[0].start;
            for ( int xc = 0 ; xc < dims[0].count ; ++xc ) {
                float buff = float (row[x]) / float (255.0);
                func( reinterpret_cast < const char * > ( & buff ) );

                x += dims[0].step;
            }
            y += dims[1].step;
        }
    } // forEach

    virtual const VI &
    currentPos() override
    {
        qFatal( "Not implemented yet" );
        return m_currPosView;
    }

    virtual Carta::Lib::NdArray::RawViewInterface *
    getView( const SliceND & sliceInfo ) override
    {
        // apply the slice to dimensions of this view
        SliceND::ApplyResult ar = sliceInfo.apply( dims() );

        // create applied result that combines m_appliedSlice with ar
        SliceND::ApplyResult newAr = SliceND::ApplyResult::combine( m_appliedSlice, ar );

        // return a new view bases on the new slice
        return new QImageRawView( m_data, m_origDims, newAr );
    }

    virtual int64_t
    read( int64_t buffSize, char * buff, Traversal traversal ) override
    {
        Q_UNUSED( buffSize );
        Q_UNUSED( buff );
        Q_UNUSED( traversal );
        qFatal( "not implemented" );
    }

    virtual void
    seek( int64_t ind ) override
    {
        Q_UNUSED( ind );
        qFatal( "not implemented" );
    }

    virtual int64_t
    read( int64_t chunk, int64_t buffSize, char * buff, Traversal traversal ) override
    {
        Q_UNUSED( chunk );
        Q_UNUSED( buffSize );
        Q_UNUSED( buff );
        Q_UNUSED( traversal );
        qFatal( "not implemented" );
    }

    virtual void
    forEach( int64_t buffSize,
             std::function < void (const char *, int64_t) > func,
             char * buff,
             Traversal traversal ) override
    {
        Q_UNUSED( buffSize );
        Q_UNUSED( func );
        Q_UNUSED( buff );
        Q_UNUSED( traversal );
        qFatal( "not implemented" );
    }

private:

    // dimensions of the view
    VI m_viewDims;

    // dimesions of the original data
    VI m_origDims;

    // we remember shared pointer to prevent data from disappearing
    std::shared_ptr < std::vector < unsigned char > > m_data = nullptr;

    // we remember raw data pointer for faster access
    unsigned char * m_rawData = nullptr;
    float m_floatBuff;
    VI m_currPosView;

    // the current resolved slice for the data we have
    SliceND::ApplyResult m_appliedSlice;
};

/// we need to implement our own coordinate formatter
class QImageCF : public
                 CoordinateFormatterInterface
{
public:

    QImageCF()
    {
        // setup axis infos
        m_axisInfos.push_back(
            AxisInfo()
                .setKnownType( AxisInfo::KnownType::OTHER )
                .setLongLabel( HtmlString::fromPlain( "X coordinate" ) )
                .setShortLabel( HtmlString::fromPlain( "X" ) )
                .setUnit( "n/a" ) );
        m_axisInfos.push_back(
            AxisInfo()
                .setKnownType( AxisInfo::KnownType::OTHER )
                .setLongLabel( HtmlString::fromPlain( "Y coordinate" ) )
                .setShortLabel( HtmlString::fromPlain( "Y" ) )
                .setUnit( "n/a" ) );

        // setup precisions
        m_precisions.push_back( 3 );
        m_precisions.push_back( 3 );
    }

    virtual CoordinateFormatterInterface *
    clone() const override
    {
        //  we'll use copy constructor for this
        auto res = new QImageCF( * this );
        return res;
    }

    virtual int
    nAxes() const override
    {
        return 2;
    }

    virtual QStringList
    formatFromPixelCoordinate( const VD & pix ) override
    {
        CARTA_ASSERT( pix.size() >= 2 );
        QStringList res;
        res.append( QString::number( pix[0] ) );
        res.append( QString::number( pix[1] ) );
        return res;
    }

    virtual QString
    calculateFormatDistance( const VD & p1, const VD & p2 ) override
    {
        Q_UNUSED( p1 );
        Q_UNUSED( p2 );

        qFatal( "not implemented" );
    }

    virtual void
    setTextOutputFormat( TextFormat fmt ) override
    {
        Q_UNUSED( fmt );
    }

    virtual const Carta::Lib::AxisInfo &
    axisInfo( int ind ) const override
    {
        CARTA_ASSERT( ind >= 0 && ind < nAxes() );
        return m_axisInfos[ind];
    }

    virtual Me &
    disableAxis( int ind ) override
    {
        Q_UNUSED( ind );
        qFatal( "not implemented" );
        return * this;
    }

    virtual Me &
    enableAxis( int ind ) override
    {
        Q_UNUSED( ind );
        qFatal( "not implemented" );
        return * this;
    }

    virtual KnownSkyCS
    skyCS() override
    {
        return KnownSkyCS::Unknown;
    }

    virtual Me &
    setSkyCS( const KnownSkyCS & scs ) override
    {
        Q_UNUSED( scs );
        return * this;
    }

    virtual SkyFormatting
    skyFormatting() override
    {
        return SkyFormatting::Default;
    }

    virtual Me &
    setSkyFormatting( SkyFormatting format ) override
    {
        Q_UNUSED( format );
        return * this;
    }

    virtual int
    axisPrecision( int axis ) override
    {
        CARTA_ASSERT( axis >= 0 && axis < nAxes() );
        return m_precisions[axis];
    }

    virtual Me &
    setAxisPrecision( int precision, int axis ) override
    {
        CARTA_ASSERT( axis >= 0 && axis < nAxes() );
        m_precisions[axis] = precision;
        return * this;
    }

    virtual bool
    toWorld( const VD & pixel, VD & world ) const override
    {
        world = pixel;
        return true;
    }

    virtual bool
    toPixel( const VD & world, VD & pixel ) const override
    {
        pixel = world;
        return true;
    }

private:

    /// cached info per axis
    std::vector < AxisInfo > m_axisInfos;

    /// precisions
    std::vector < int > m_precisions;
};

/// we need to implement our own MetaDataInterface
class QImageMDI : public Carta::Lib::Image::MetaDataInterface
{
public:

    QImageMDI( std::shared_ptr < QImageII > ii )
    {
        m_ii = ii;
        m_coordinateFormatter = std::make_shared < QImageCF > ();
    }

    virtual Carta::Lib::Image::MetaDataInterface *
    clone() override
    {
        qFatal( "not implemented" );
        return nullptr;
    }

    virtual CoordinateFormatterInterface::SharedPtr
    coordinateFormatter() override
    {
        return m_coordinateFormatter;
    }

    std::pair<double,QString> getRestFrequency() const override {
    	std::pair<double,QString> restFreq(0,"");
    	return restFreq;
    }

    virtual PlotLabelGeneratorInterface::SharedPtr
    plotLabelGenerator() override
    {
        qFatal( "not implemented" );
        return nullptr;
    }

    virtual QString
    title( TextFormat format ) override
    {
        if ( format == TextFormat::Plain ) {
            return m_title.plain();
        }
        else {
            return m_title.html();
        }
    }

    virtual QStringList
    otherInfo( TextFormat format ) override
    {
        Q_UNUSED( format );
        return QStringList()
               << "This is a qimage";
    }

    virtual Carta::Lib::Regions::ICoordSystemConverter::SharedPtr getCSConv() override;

private:

    Carta::Lib::HtmlString m_title;
    CoordinateFormatterInterface::SharedPtr m_coordinateFormatter = nullptr;
    std::shared_ptr < QImageII > m_ii = nullptr;
};

/// we need to implement our own ImageInterface
class QImageII
    : public Carta::Lib::Image::ImageInterface
      , public std::enable_shared_from_this < QImageII >
{
public:

    static std::shared_ptr < QImageII >
    load( QString fname )
    {
        // c++ does not allow this with QImageII having a private constructor:
        //        std::shared_ptr < QImageII > ptr = std::make_shared < QImageII > ();
        // so instead we need to do this /facepalm
        struct CPPSUCKS : public QImageII { };
        std::shared_ptr < QImageII > image = std::make_shared < CPPSUCKS > ();

        if ( image-> init( fname ) ) {
            return image;
        }
        else {
            return nullptr;
        }
    } // load

    virtual const Carta::Lib::Unit &
    getPixelUnit() const override
    {
        return m_unit;
    }

    virtual const VI &
    dims() const override
    {
        return m_dims;
    }

    virtual std::shared_ptr<Carta::Lib::Image::ImageInterface>
            getPermuted(const std::vector<int> & /*indices*/){
            qFatal( "Not implemented");
        }

    virtual bool
    hasMask() const override
    {
        return false;
    }

    virtual bool
    hasErrorsInfo() const override
    {
        return false;
    }

    virtual Carta::Lib::Image::PixelType
    pixelType() const override
    {
        return Carta::Lib::Image::PixelType::Real32;
    }

    virtual Carta::Lib::Image::PixelType
    errorType() const override
    {
        return Carta::Lib::Image::PixelType::Real32;
    }

    virtual Carta::Lib::NdArray::RawViewInterface *
    getDataSlice( const SliceND & sliceInfo ) override
    {
//        return new QImageRawView( shared_from_this(), sliceInfo );
        return new QImageRawView( m_data, m_dims, sliceInfo );
    }

    virtual Carta::Lib::NdArray::Byte *
    getMaskSlice( const SliceND & sliceInfo ) override
    {
        Q_UNUSED( sliceInfo );
        return nullptr;
    }

    virtual Carta::Lib::NdArray::RawViewInterface *
    getErrorSlice( const SliceND & sliceInfo ) override
    {
        Q_UNUSED( sliceInfo );
        return nullptr;
    }

    virtual Carta::Lib::Image::MetaDataInterface::SharedPtr
    metaData() override
    {
        return m_mdi;
    }

    bool
    valid() const { return m_valid; }

private:

    QImageII()
    {
        m_valid = false;
        m_dims.resize( 2, 0 );

        // prepare unit
        m_unit = Carta::Lib::Unit( "n/a" );
    }

    // do not call this directly
    bool
    init( QString fname )
    {
        QImage qimg;
        m_valid = qimg.load( fname );
        if ( ! m_valid ) {
            return false;
        }
        qimg = qimg.mirrored( false, true );
        if ( qimg.format() != QImage::Format_ARGB32_Premultiplied ) {
            qimg = qimg.convertToFormat( QImage::Format_ARGB32_Premultiplied );
        }

        // prepare dimensions
        m_dims[0] = qimg.width();
        m_dims[1] = qimg.height();

        // prepare metadata
        m_mdi = std::make_shared < QImageMDI > ( shared_from_this() );

        // extract the actual pixels and store it in our own data buffer (only one byte
        // per pixel, gray equivalent to be precise)
        m_data = std::make_shared < std::vector < unsigned char > > ( m_dims[0] * m_dims[1] );
        unsigned int * src = (unsigned int *) qimg.bits();
        int count = qimg.width() * qimg.height();
        unsigned char * dst = & m_data-> at( 0 );
        for ( int i = 0 ; i < count ; ++i ) {
            * dst = qGray( * src );
            src++;
            dst++;
        }
        return true;
    } // init

private:

    // data

    Carta::Lib::Unit m_unit;
    VI m_dims;
    Carta::Lib::Image::MetaDataInterface::SharedPtr m_mdi = nullptr;
    bool m_valid = false;

    // here we'll store the actual gray scale data
    std::shared_ptr < std::vector < unsigned char > > m_data = nullptr;
};

QImagePlugin::QImagePlugin( QObject * parent ) :
    QObject( parent )
{ }

bool
QImagePlugin::handleHook( BaseHook & hookData )
{
    qDebug() << "QImagePlugin is handling hook #" << hookData.hookId();
    if ( hookData.is < Initialize > () ) {
        // we may need to initialize some qt formats here in the future...
        return true;
    }
    else if ( hookData.is < LoadAstroImage > () ) {
        LoadAstroImage & hook = static_cast < LoadAstroImage & > ( hookData );
        auto fname = hook.paramsPtr->fileName;

        hook.result = QImageII::load( fname );

        // return true if result is not null
        return hook.result != nullptr;
    }

    qWarning() << "QImagePlugin: Sorrry, dont' know how to handle this hook";
    return false;
} // handleHook

std::vector < HookId >
QImagePlugin::getInitialHookList()
{
    return {
               Initialize::staticId,
               LoadAstroImage::staticId
    };
}

Carta::Lib::Regions::ICoordSystemConverter::SharedPtr QImageMDI::getCSConv() {
    int ndim =  m_ii-> dims().size();
    auto ptr = Carta::Lib::Regions::makePixelIdentityConverter( ndim);
    Carta::Lib::Regions::ICoordSystemConverter::SharedPtr sptr( std::move(ptr));
    return sptr;

//    return std::make_shared< Carta::Lib::Regions::DefaultCoordSystemConverter>(ndim);
//    return std::make_shared< Carta::Lib::Regions::PixelIdentityCSConverter>(ndim);
}
