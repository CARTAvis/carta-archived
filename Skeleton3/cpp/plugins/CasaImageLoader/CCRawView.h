/**
 *
 **/

#pragma once

#include "common/IImage.h"

class CCImage;

/// CasaImageLoader plugin's implementation of the raw view
class CCRawView
    : public NdArray::RawViewInterface
{
public:
    CCRawView(CCImage * ccimage, const SliceND & sliceInfo);

    virtual PixelType
    pixelType() override;

    virtual const VI &
    dims() override;

    virtual const char *
    get( const VI & pos ) override;

    virtual void
    forEach( std::function < void (const char *) > func, Traversal traversal ) override;

    virtual const VI &
    currentPos() override;

    union IIBuff {
        float f;
        double d;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        char chr;
    };

protected:
    CCImage * m_ccimage = nullptr;
    VI m_currPosSrc, m_currPosDest;
    SliceND::ApplyResult m_appliedSlice;
    VI m_viewDims;
    static union IIBuff m_buff;

};
