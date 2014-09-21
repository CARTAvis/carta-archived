/**
 *
 **/


#pragma once

#include "CCRawView.h"
#include "common/IImage.h"
#include "images/Images/ImageInterface.h"
#include <QDebug>


/// implementation of the ImageInterface that the casacore image loader plugin
/// returns to the core
///
/// Note: Because I could not figure out whether CasaCore supports storing the
/// image in a non-templated class while maintaining fast data access, I templated
/// this class as well...

// disabling templating for now... until I debug linker errors
//template < typename PType >
class CCImage : public Image::ImageInterface
{
//    Q_OBJECT

public:

    virtual const Unit & getPixelUnit() const override
    {
        return m_unit;
    }

    virtual const std::vector<int> & dims() const override
    {
        return m_dims;
    }

    virtual bool hasMask() const override
    {
        return false;
    }
    virtual bool hasErrorsInfo() const override
    {
        return false;
    }
    virtual Image::PixelType pixelType() const override
    {
        return m_pixelType;
    }
    virtual Image::PixelType errorType() const override
    {
        qFatal( "not implemented");

    }
    virtual NdArray::RawViewInterface * getDataSlice(const SliceND & sliceInfo) override
    {
        return new CCRawView( this, sliceInfo);
    }
    virtual void getMaskSlice(const SliceND & sliceInfo, NdArray::Byte & result) override
    {
        Q_UNUSED( sliceInfo);
        Q_UNUSED( result);
        qFatal( "not implemented");

    }
    virtual void getErrorSlice(const SliceND & sliceInfo, NdArray::RawViewInterface & result) override
    {
        qFatal( "not implemented");

    }
    virtual Image::MetaDataInterface & metaData() override
    {
        qFatal( "not implemented");

    }

    static CCImage * create2( casa::LatticeBase * casaImage) {
        qDebug() << "create2" << casaImage;
        CCImage * img = new CCImage;
        img-> m_casaLattice = casaImage;
        // try to cast to various known casa image types....

        if( casa::ImageInterface<float> * fi = dynamic_cast<casa::ImageInterface<float> *>(casaImage)) {
            img->m_pixelType = Image::PixelType::Real32;
            img->m_ptr.floatII = fi;
            img-> m_unit = Unit( fi-> units().getName().c_str());
        }
        else if( casa::ImageInterface<double> * di = dynamic_cast<casa::ImageInterface<double> *>(casaImage)) {
            img->m_pixelType = Image::PixelType::Real64;
            img->m_ptr.doubleII = di;
            img-> m_unit = Unit( di-> units().getName().c_str());
        }
        else if( casa::ImageInterface<int64_t> * i64i = dynamic_cast<casa::ImageInterface<int64_t> *>(casaImage)) {
            img->m_pixelType = Image::PixelType::Int64;
            img->m_ptr.int64II = i64i;
            img-> m_unit = Unit( i64i-> units().getName().c_str());
        }
        else if( casa::ImageInterface<int32_t> * i32i = dynamic_cast<casa::ImageInterface<int32_t> *>(casaImage)) {
            img->m_pixelType = Image::PixelType::Int32;
            img->m_ptr.int32II = i32i;
            img-> m_unit = Unit( i32i-> units().getName().c_str());
        }
        else if( casa::ImageInterface<int16_t> * i16i = dynamic_cast<casa::ImageInterface<int16_t> *>(casaImage)) {
            img->m_pixelType = Image::PixelType::Int16;
            img->m_ptr.int16II = i16i;
            img-> m_unit = Unit( i16i-> units().getName().c_str());
        }
        else {
            delete img;
            return nullptr;
        }

        img-> m_dims = casaImage-> shape().asStdVector();
        return img;
    }


    template < typename PType >
    static CCImage * create( casa::ImageInterface<PType> * casaImage) {
        qDebug() << "create running" << casaImage;
        CCImage * img = new CCImage;
        img-> m_casaLattice = casaImage;
        img-> m_pixelType = Image::CType2PixelType<PType>::type;
        img-> m_dims = casaImage-> shape().asStdVector();
//        for( auto x : casaImage->shape()) {
//            img->m_dims.push_back( static_cast<int64_t>(x));
//        }
        switch (Image::CType2PixelType<PType>::type) {
        case Image::PixelType::Real32:
            img->m_ptr.floatII = casaImage;
            break;
        default:
            break;
        }

        qDebug() << "casaImage-> units().getName().c_str() = " << casaImage-> units().getName().c_str();
        img-> m_unit = Unit( casaImage-> units().getName().c_str());
//        img->foo();

        return img;
    }

protected:

    CCImage() : Image::ImageInterface() {}
    ~CCImage() {}

    Image::PixelType m_pixelType;
    std::vector<int> m_dims;
    casa::LatticeBase * m_casaLattice;
    Unit m_unit;

    union {
        casa::ImageInterface<float> * floatII;
        casa::ImageInterface<double> * doubleII;
        casa::ImageInterface<int16_t> * int16II;
        casa::ImageInterface<int32_t> * int32II;
        casa::ImageInterface<int64_t> * int64II;
    } m_ptr;

    friend class CCRawView;
};

//template< typename PType>
//Image::PixelType CCImage<PType>::pixelType() const
//{
//    return m_pixelType;
//}

//template < typename PType >
//CCImage<PType> * CCImage<PType>::create(casa::ImageInterface<PType> * cimage)
//{
//    CCImage * img = new CCImage;
//    img-> m_pixelType = Image::CType2PixelType<PType>::type;
//    return img;
//}
