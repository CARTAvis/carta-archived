/**
 *
 **/


#pragma once

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
        qFatal( "not implemented");
    }

    virtual const std::vector<int> & dims() const override
    {
        auto res = m_casaLattice-> shape().asStdVector();

        qDebug() << "m_casaLattice=" << m_casaLattice;
        return res;

        return m_casaLattice-> shape().asStdVector();
//        return m_dims;
    }

    virtual const QString & imageType() const override
    {
        qFatal( "not implemented");

    }

    virtual bool hasMask() const override
    {
        qFatal( "not implemented");

    }
    virtual bool hasErrorsInfo() const override
    {
        qFatal( "not implemented");

    }
    virtual Image::PixelType pixelType() const override
    {
        return m_pixelType;
    }
    virtual Image::PixelType errorType() const override
    {
        qFatal( "not implemented");

    }
    virtual void getDataSlice(const SliceND & sliceInfo, NdArray::RawViewInterface & result) override
    {
        qFatal( "not implemented");

    }
    virtual void getMaskSlice(const SliceND & sliceInfo, NdArray::Byte & result) override
    {
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

    template < typename PType >
    static CCImage * create( casa::ImageInterface<PType> * casaImage) {
        CCImage * img = new CCImage;
        img-> m_casaLattice = casaImage;
        img-> m_pixelType = Image::CType2PixelType<PType>::type;
//        img-> m_dims = casaImage-> shape().asStdVector();
//        m_casaLattice->shape();
        return img;
    }

protected:

    CCImage() : Image::ImageInterface() {}
    ~CCImage() {}

    Image::PixelType m_pixelType;
    std::vector<int> m_dims;
    casa::LatticeBase * m_casaLattice;
//    Image::ImageInterface * m_
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
