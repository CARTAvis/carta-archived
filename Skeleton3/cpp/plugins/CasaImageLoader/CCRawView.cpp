/**
 *
 **/

#include "CCRawView.h"
#include "CCImage.h"

CCRawView::CCRawView( CCImage * ccimage, const SliceND & sliceInfo )
{
    m_ccimage      = ccimage;
    m_appliedSlice = sliceInfo.apply( m_ccimage-> dims() );
    qDebug() << sliceInfo.toStr() << "applied to" << m_ccimage->dims() << "="
             << m_appliedSlice.toStr();

    for ( auto & x : m_appliedSlice.dims() ) {
        m_viewDims.push_back( x.count );
    }
}

NdArray::RawViewInterface::PixelType
CCRawView::pixelType()
{
    return m_ccimage->pixelType();
}

const NdArray::RawViewInterface::VI &
CCRawView::dims()
{
    return m_viewDims;
}

const char *
CCRawView::get( const NdArray::RawViewInterface::VI & pos )
{
    if ( pos.size() > dims() ) { throw std::runtime_error( "invalid position" );
    }

    // we need to translate pos to the destination...
    VI destPos;
    VI::value_type p;
    for ( size_t i = 0 ; i < dims().size() ; i++ ) {
        if ( i < pos.size() ) {
            p = pos[i];
        }
        else {
            p = 0;
        }
        destPos.push_back( m_appliedSlice.dims()[i].start
                           + p * m_appliedSlice.dims()[i].step );
    }

    qDebug() << "destPos" << destPos;

    switch ( pixelType() )
    {
    case Image::PixelType::Real32 :
        m_buff.f = m_ccimage-> m_ptr.floatII->
                       operator() ( destPos );

        break;
    case Image::PixelType::Real64 :
        m_buff.f = m_ccimage-> m_ptr.doubleII->
                       operator() ( destPos );

        break;
    case Image::PixelType::Int16 :
        m_buff.f = m_ccimage-> m_ptr.int16II->
                       operator() ( destPos );

        break;
    default :
        qFatal( "Not implemented yet :(" );
    } // switch

    return & m_buff.chr;
} // get

void
CCRawView::forEach( std::function < void (const char *) > func,
                    NdArray::RawViewInterface::Traversal traversal )
{ }

const NdArray::RawViewInterface::VI &
CCRawView::currentPos()
{ }

CCRawView::IIBuff CCRawView::m_buff;
