/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/AxisInfo.h"
#include "CCRawView.h"
#include "CCMetaDataInterface.h"
#include "casacore/images/Images/ImageInterface.h"
#include <QDebug>
#include <memory>

/// helper base class so that we can easily determine if this is a an image
/// interface created by this plugin if we ever want to down-cast it...
class CCImageBase
    : public Image::ImageInterface
{
    CLASS_BOILERPLATE( CCImageBase );

public:
    /// \todo not sure if this is necessary for RTTI to work, if it's not it
    /// can be removed...
    virtual bool
    isCasaImage() const final
    {
        return true;
    }

    /**
     * Returns a pointer to the underlying casa::LatticeBase* or null if there is no underlying
     * casacore image.
     * @return casa::LatticeBase *
     */
    virtual casa::LatticeBase * getCasaImage() = 0;

};

/// implementation of the ImageInterface that the casacore image loader plugin
/// returns to the core
///
/// Note: Because I could not figure out whether CasaCore supports storing the
/// image in a non-templated class while maintaining fast data access, I templated
/// this class as well...
template < typename PType >
class CCImage
    : public CCImageBase
      , public std::enable_shared_from_this < CCImage < PType > >
{
    CLASS_BOILERPLATE( CCImage );

public:
    virtual const Unit &
    getPixelUnit() const override
    {
        return m_unit;
    }

    virtual const std::vector < int > &
    dims() const override
    {
        return m_dims;
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

    /// it is safe to static_cast the instance of CCImageBase to CCImage<T>
    /// based on this type
    virtual Image::PixelType
    pixelType() const override
    {
        return m_pixelType;
    }

    virtual Image::PixelType
    errorType() const override
    {
        qFatal( "not implemented" );
    }

    virtual NdArray::RawViewInterface *
    getDataSlice( const SliceND & sliceInfo ) override
    {
        return new CCRawView < PType > ( this, sliceInfo );
    }

    /// \todo implement this
    virtual NdArray::Byte *
    getMaskSlice( const SliceND & sliceInfo) override
    {
        Q_UNUSED( sliceInfo );
        qFatal( "not implemented" );
    }

    /// \todo implement this
    virtual NdArray::RawViewInterface *
    getErrorSlice( const SliceND & sliceInfo) override
    {
        Q_UNUSED( sliceInfo );
        qFatal( "not implemented" );
    }

    virtual Image::MetaDataInterface::SharedPtr
    metaData() override
    {
        return m_meta;
    }



    static CCImage::SharedPtr
    create( casa::ImageInterface < PType > * casaImage )
    {
        // create an image interface instance and populate it with various
        // values from casa::ImageInterface
        CCImage::SharedPtr img = std::make_shared < CCImage < PType > > ();
        img-> m_pixelType = Image::CType2PixelType < PType >::type;
        img-> m_dims      = casaImage-> shape().asStdVector();
        img-> m_casaII    = casaImage;
        img-> m_unit      = Unit( casaImage-> units().getName().c_str() );

        // get title and escape html characters in case there are any
        QString htmlTitle = casaImage->imageInfo().objectName().c_str();
        htmlTitle = htmlTitle.toHtmlEscaped();

        // make our own copy of the coordinate system using 'clone'
        std::shared_ptr<casa::CoordinateSystem> casaCS(
                    static_cast<casa::CoordinateSystem *> (casaImage->coordinates().clone()));

        // construct a meta data instance
        img-> m_meta = std::make_shared < CCMetaDataInterface > ( htmlTitle, casaCS );

        return img;
    } // create

    virtual casa::LatticeBase * getCasaImage(){
        return m_casaII;
    }

    /// this should be protected... but I don't have time to fix the compiler errors
    /// basically we
//    template < class X >
//    friend class CCImage<PType>;
//    friend class CCImage<double>;
//    friend class CCImage<int16_t>;
//    friend class CCImage<int32_t>;
//    friend class CCImage<int64_t>;
//    friend class CCImage<u_int8_t>;
//protected:

    CCImage() { }

    virtual
    ~CCImage() { }

protected:
    /// type of the image data
    Image::PixelType m_pixelType;

    /// cached dimensions of the image
    std::vector < int > m_dims;

    /// pointer to the actual casa::ImageInterface
    casa::ImageInterface < PType > * m_casaII;

    /// cached unit
    Unit m_unit;

    /// meta data pointer
    CCMetaDataInterface::SharedPtr m_meta;

    /// we want CCRawView to access our internals...
    /// \todo maybe we just need a public accessor, no? I don't like friends :)
    friend class CCRawView < PType >;


};
