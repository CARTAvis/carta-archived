/**
 *
 **/

#pragma once

#include "CCRawView.h"
#include "CCMetaDataInterface.h"
#include "common/IImage.h"
#include "images/Images/ImageInterface.h"
#include <QDebug>
#include <memory>

/// helper base class so that we can easily determine if this is a an image
/// interface created by this plugin if we ever want to down-cast it...
class CCImageBase : public Image::ImageInterface
{
public:
    /// \todo not sure if this is necessary for RTTI to work, if it's not it
    /// can be removed...
    virtual bool
    isCasaImage() const final
    {
        return true;
    }
};

/// implementation of the ImageInterface that the casacore image loader plugin
/// returns to the core
///
/// Note: Because I could not figure out whether CasaCore supports storing the
/// image in a non-templated class while maintaining fast data access, I templated
/// this class as well...
template < typename PType >
class CCImage : public CCImageBase
{
//    Q_OBJECT

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
    virtual void
    getMaskSlice( const SliceND & sliceInfo, NdArray::Byte & result ) override
    {
        Q_UNUSED( sliceInfo );
        Q_UNUSED( result );
        qFatal( "not implemented" );
    }

    /// \todo implement this
    virtual void
    getErrorSlice( const SliceND & sliceInfo, NdArray::RawViewInterface & result ) override
    {
        Q_UNUSED( sliceInfo );
        Q_UNUSED( result );
        qFatal( "not implemented" );
    }

    virtual Image::MetaDataInterface::SharedPtr
    metaData() override
    {
        return m_meta;
    }

    static CCImage *
    create( casa::ImageInterface < PType > * casaImage )
    {
        qDebug() << "create running" << casaImage;
        CCImage * img = new CCImage < PType >;
        img-> m_pixelType = Image::CType2PixelType < PType >::type;
        img-> m_dims      = casaImage-> shape().asStdVector();
        img-> m_casaII    = casaImage;
        img-> m_unit      = Unit( casaImage-> units().getName().c_str() );
        qDebug() << "iii objectName=" << casaImage->imageInfo().objectName().c_str();

        QString htmlTitle = casaImage->imageInfo().objectName().c_str();
        img-> m_meta = std::make_shared<CCMetaDataInterface>(htmlTitle);

        return img;
    } // create

protected:

    virtual
    ~CCImage() { }

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
