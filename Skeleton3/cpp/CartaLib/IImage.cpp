/// empty file to avoid qt creator bug double loading some include files when using
/// outside tree shadow build

/// contains implementations of pure virtual functions to report fatal errors as
/// plugins that only declare but don't define methods compile just fine... :(

#include "IImage.h"

const Unit &Image::ImageInterface::getPixelUnit() const
{
    qFatal( "Calling unimplemented virtual function... ");
}


const Image::ImageInterface::VI & Image::ImageInterface::dims() const
{
    qFatal( "Calling unimplemented virtual function... ");
}

bool Image::ImageInterface::hasMask() const
{
    qFatal( "Calling unimplemented virtual function... ");
}


Image::ImageInterface::PixelType Image::ImageInterface::pixelType() const
{
    qFatal( "Calling unimplemented virtual function... ");
}


Image::ImageInterface::PixelType Image::ImageInterface::errorType() const
{
    qFatal( "Calling unimplemented virtual function... ");
}


NdArray::RawViewInterface * Image::ImageInterface::getDataSlice(const SliceND & sliceInfo)
{
    Q_UNUSED( sliceInfo);
    qFatal( "Calling unimplemented virtual function... ");
}


NdArray::Byte * Image::ImageInterface::getMaskSlice(const SliceND & sliceInfo)
{
    Q_UNUSED( sliceInfo);
    qFatal( "Calling unimplemented virtual function... ");
}


NdArray::RawViewInterface * Image::ImageInterface::getErrorSlice(const SliceND & sliceInfo)
{
    Q_UNUSED( sliceInfo);
    qFatal( "Calling unimplemented virtual function... ");
}

Image::MetaDataInterface::SharedPtr Image::ImageInterface::metaData()
{
    qFatal( "Calling unimplemented virtual function... ");
}


Image::MetaDataInterface::~MetaDataInterface()
{

}
