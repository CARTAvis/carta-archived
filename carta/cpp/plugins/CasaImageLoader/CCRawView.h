/**
 *
 **/

#pragma once

#include "CartaLib/IImage.h"
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/casa/Arrays/IPosition.h>

template < typename PType >
class CCImage;

/// CasaImageLoader plugin's implementation of the raw view
///
/// \warning We are not handling negative step
/// \warning We are not handling 'index' slices, i.e. axis removal
///
/// \todo Implement negative step
/// \todo Implement indexed slices (i.e. axis removal)
template < typename PType >
class CCRawView
    : public NdArray::RawViewInterface
{
public:

    /// construct a view on an image from provided slice information
    /// \param ccimage pointer to CCImage which we keep on using, but we don't assume ownership.
    /// It has to remain valid for the duration of existance of this instance.
    /// \param sliceInfo for which part of the ccimage to create view
    CCRawView( CCImage < PType > * ccimage, const SliceND & sliceInfo );

    virtual PixelType
    pixelType() override
    {
        return m_ccimage->pixelType();
    }

    virtual const VI &
    dims() override
    {
        return m_viewDims;
    }

    virtual const char *
    get( const VI & pos ) override;

    virtual void
    forEach( std::function < void (const char *) > func, Traversal traversal ) override;

    virtual const VI &
    currentPos() override;

    virtual RawViewInterface *
    getView(const SliceND & sliceInfo) override
    {
        // apply the slice to dimensions of this view
        SliceND::ApplyResult ar = sliceInfo.apply( dims());

        // create applied result that combines m_appliedSlice with ar
        SliceND::ApplyResult newAr = SliceND::ApplyResult::combine( m_appliedSlice, ar);

        // return a new view bases on the new slice
        return new CCRawView( m_ccimage, newAr);
    }

    virtual int64_t
    read( int64_t buffSize, char * buff,
          Traversal traversal = Traversal::Sequential ) override
    {
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

    /// another high performance accessor to data
    /// motivated by unix read() but stateless (i.e. one needs to supply the
    /// chunk number)
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

    /// yet another high performance accessor... similar to forEach above,
    /// but this time the supplied function gets called with whatever number
    /// elements that fit into the buffer
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

protected:

    /// construct a view directly from applied slice
    CCRawView( CCImage < PType > * ccimage, const SliceND::ApplyResult & applyResult );

    CCImage < PType > * m_ccimage = nullptr; // we don't own this!
    VI m_currPosImage, m_currPosView;
    SliceND::ApplyResult m_appliedSlice;
    VI m_viewDims;

    // buffer for reporting results when calling get()
    PType m_buff;

    // minicache to make get() a little bit faster
    VI m_destPos;
};

// public constructor
template < typename PType >
CCRawView < PType >::CCRawView( CCImage < PType > * ccimage, const SliceND & sliceInfo )
{
    // remember the pointer to the carta image
    m_ccimage = ccimage;

    // figure out what data to extract for each of the dimensions
    m_appliedSlice = sliceInfo.apply( m_ccimage-> dims() );

    // cache the dimensions of the result
    for ( auto & x : m_appliedSlice.dims() ) {
        m_viewDims.push_back( x.count );
    }

    // prepare destPos mini cache
    m_destPos.resize( m_viewDims.size());
}

// protected constructor
template < typename PType >
CCRawView < PType >::CCRawView( CCImage < PType > * ccimage, const SliceND::ApplyResult & applyResult )
{
    /// \todo refactor common code between the two constructors into some
    /// sort of 'init' method?

    // remember the pointer to the carta image
    m_ccimage = ccimage;

    // figure out what data to extract for each of the dimensions
    m_appliedSlice = applyResult;

    // cache the dimensions of the result
    for ( auto & x : m_appliedSlice.dims() ) {
        m_viewDims.push_back( x.count );
    }

    // prepare destPos mini cache
    m_destPos.resize( m_viewDims.size());
}

template < typename PType >
const char *
CCRawView < PType >::get( const NdArray::RawViewInterface::VI & pos )
{
    // preconditions
    if ( CARTA_RUNTIME_CHECKS && pos.size() > dims().size() ) {
        throw std::runtime_error( "invalid position" );
    }

    // we need to translate pos to the destination...
    VI::value_type p;
    for ( size_t i = 0 ; i < dims().size() ; i++ ) {
        if ( i < pos.size() ) {
            p = pos[i];
        }
        else {
            p = 0;
        }
//        m_destPos.push_back( m_appliedSlice.dims()[i].start
//                           + p * m_appliedSlice.dims()[i].step );
        m_destPos[i] = m_appliedSlice.dims()[i].start
                       + p * m_appliedSlice.dims()[i].step;
    }

    // for some reason casa::ImageInterface::operator() returns the result by value
    // so in order to return reference (to satisfy our API) we need to store this
    // in a buffer first...
    m_buff = m_ccimage-> m_casaII->
                 operator() ( m_destPos );

    return reinterpret_cast < const char * > ( & m_buff );
} // get

template < typename PType >
void
CCRawView < PType >::forEach(
    std::function < void (const char *) > func,
    NdArray::RawViewInterface::Traversal traversal )
{
    if ( traversal == NdArray::RawViewInterface::Traversal::Optimal ) {
        qFatal( "sorry, not implemented yet" );
    }
    auto casaII     = m_ccimage-> m_casaII;
    int imgDims     = casaII-> ndim();
    auto imageShape = casaII-> shape();

//    qDebug() << "CCRawView::forEach=" << m_appliedSlice.toStr()  ;

    /// \todo I guessed wrong as to what the cursor shape meant when using subSection()
    /// \todo the shape refers to the shape within the subsection... so there is no need
    /// \todo to specify cursor shape to be the whole image, it should instead be
    /// \todo the shape of the subsection... e.g. [3:7:2,5:6] only needs a cursor
    /// of size 2x1(x1x1x1....x1) regardless of the image size...
    auto cursorShape = imageShape;
    casa::LatticeStepper stepper( imageShape, cursorShape, casa::LatticeStepper::RESIZE );
    casa::IPosition blc( imgDims, 0 );
    auto trc = blc;
    auto inc = blc;
    for ( size_t i = 0 ; i < imgDims ; i++ ) {
        const auto & slice1d = m_appliedSlice.dims()[i];
        blc( i ) = slice1d.start;
        trc( i ) = slice1d.end();
        inc( i ) = slice1d.step;
    }
    stepper.subSection( blc, trc, inc );
    casa::RO_LatticeIterator < PType > iterator( * casaII, stepper );

    bool first = true;
    for ( iterator.reset() ; ! iterator.atEnd() ; iterator++ ) {
        if ( ! first ) {
            // sanity check for now, to make sure I understand casa cursors
            // if this works, the outer loop is not necessary...
            qFatal( "something went wrong" );
        }
        const auto & cursor = iterator.cursor();
        for ( const auto & val : cursor ) {
            func( reinterpret_cast < const char * > ( & val ) );
        }
        first = false;
    }
} // forEach

template < typename PType >
const NdArray::RawViewInterface::VI &
CCRawView < PType >::currentPos()
{
    return m_currPosView;
}
