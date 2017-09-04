/** Implementation of the approximate quantile algorithm described in
 * 'Random Sampling Techniques for Space Efficient Online Computation of 
 * Order Statistics of Large Datasets' (Manku et al., 1999) */

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include <vector>
 
namespace Carta
{
namespace Core
{
namespace Algorithms
{
    
class Buffer {
    /** The state of the buffer */
    enum class State {empty, partial, full};
    
    /** The capacity of the buffer */
    const int capacity;
    
    /** The actual current size of the buffer */
    const int size() {
        return elements.size();
    }

    /** The weight of the buffer */
    int weight;
    
    /** The level of the buffer */
    int level;
    
    /** The elements */
    
    template < typename Scalar > std::vector<Scalar> elements;
    
    // constructors with and without parameters

// here or in the function? un-oo these?
// new
// collapse
// output
}

template < typename Scalar >
static
typename std::map < double, Scalar >
percentile2pixels_approximate_manku99(
    Carta::Lib::NdArray::TypedView < Scalar > & view,
    std::vector < double > percentiles,
    int spectralIndex=-1,
    Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr,
    std::vector<double> hertzValues={}
    )
{
    // basic preconditions
    if ( CARTA_RUNTIME_CHECKS ) {
        for ( auto q : percentiles ) {
            CARTA_ASSERT( 0.0 <= q && q <= 1.0 );
            Q_UNUSED(q);
        }
    }

    // if we have a frame-dependent converter and no spectral axis,
    // we can't do anything because we don't know the channel units
    if (converter && converter->frameDependent && spectralIndex < 0) {
        qFatal("Cannot find intensities in these units: the conversion is frame-dependent and there is no spectral axis.");
    }
    
    
    // put implementation here
    
    
    

    std::map < double, Scalar > result;

    CARTA_ASSERT( result.size() == percentiles.size());

    return result;
}

}
}
}
