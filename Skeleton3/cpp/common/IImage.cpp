/// empty file to avoid qt creator bug double loading some include files when using
/// outside tree shadow build

/// contains implementations of pure virtual functions to report fatal errors as
/// plugins that only declare but don't define methods compile just fine... :(

#include "IImage.h"




const Unit &Image::ImageInterface::getPixelUnit() const
{
    qFatal( "Calling unimplemented virtual function... ");
}


const std::vector<int> &Image::ImageInterface::dims() const
{
    qFatal( "Calling unimplemented virtual function... ");
}


const QString &Image::ImageInterface::imageType() const
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


void Image::ImageInterface::getDataSlice(const SliceND & sliceInfo, NdArray::RawViewInterface & result)
{
    qFatal( "Calling unimplemented virtual function... ");
}


void Image::ImageInterface::getMaskSlice(const SliceND & sliceInfo, NdArray::Byte & result)
{
    qFatal( "Calling unimplemented virtual function... ");
}


void Image::ImageInterface::getErrorSlice(const SliceND & sliceInfo, NdArray::RawViewInterface & result)
{
    qFatal( "Calling unimplemented virtual function... ");
}


Image::MetaDataInterface &Image::ImageInterface::metaData()
{
    qFatal( "Calling unimplemented virtual function... ");
}
