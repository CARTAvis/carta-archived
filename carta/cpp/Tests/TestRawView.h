/**
 * Stub view for testing quantile algorithms
 * Contains a vector of data
 **/

#pragma once

#include "CartaLib/IImage.h"


/// Implementing the bare minimum; excluding slices for now

template < typename PType >
class TestRawView
    : public Carta::Lib::NdArray::RawViewInterface
{
public:
    TestRawView(std::vector<PType> data, VI dims) {
        this->data = std::move(data);
        m_viewDims = dims;
        m_pixelType = Carta::Lib::Image::CType2PixelType < PType >::type;
    }

    virtual PixelType
    pixelType() override {
        return m_pixelType;
    }

    virtual const VI &
    dims() override {
        return m_viewDims;
    }

     // we don't need this
    virtual const char *
    get( const VI & pos ) override {
        Q_UNUSED(pos);
        qFatal( "not implemented");
    }

    virtual void
    forEach( std::function < void (const char *) > func, Traversal traversal ) override {
        if ( traversal != Carta::Lib::NdArray::RawViewInterface::Traversal::Sequential ) {
            qFatal( "sorry, not implemented yet" );
        }
        
        for ( const auto & val : data ) {
            func( reinterpret_cast < const char * > ( & val ) );
        }
    }

    virtual const VI &
    currentPos() override {
        qFatal( "not implemented");
    }
    
    // we'll need to implement this to test the algorithms with unit conversion
    virtual RawViewInterface *
    getView(const SliceND & sliceInfo) override {
        Q_UNUSED(sliceInfo);
        qFatal( "not implemented");
    }

    virtual int64_t
    read( int64_t buffSize, char * buff,
          Traversal traversal = Traversal::Sequential ) override {
        Q_UNUSED( buffSize);
        Q_UNUSED( buff);
        Q_UNUSED( traversal);
        qFatal( "not implemented");
    }

    virtual void
    seek(int64_t ind) override
    {
        Q_UNUSED( ind);
        qFatal( "not implemented");
    }

    virtual int64_t
    read( int64_t chunk, int64_t buffSize, char * buff,
          Traversal traversal = Traversal::Sequential ) override
    {
        Q_UNUSED( chunk );
        Q_UNUSED( buffSize );
        Q_UNUSED( buff );
        Q_UNUSED( traversal );
        qFatal( "not implemented" );
    }

    virtual void
    forEach(
        int64_t buffSize,
        std::function < void (const char *, int64_t count) > func,
        char * buff = nullptr,
        Traversal traversal = Traversal::Sequential ) override
    {
        Q_UNUSED( buffSize );
        Q_UNUSED( func );
        Q_UNUSED( buff );
        Q_UNUSED( traversal );
        qFatal( "not implemented" );
    }

    std::vector<PType> data;
    VI m_viewDims;
    Carta::Lib::Image::PixelType m_pixelType;
};
