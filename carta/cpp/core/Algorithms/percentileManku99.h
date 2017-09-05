/** Implementation of the approximate quantile algorithm described in
 * 'Random Sampling Techniques for Space Efficient Online Computation of 
 * Order Statistics of Large Datasets' (Manku et al., 1999) */

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include "percentileAlgorithms.h"
#include <vector>
 
namespace Carta
{
namespace Core
{
namespace Algorithms
{

class Buffer {
    
    /** The capacity of the buffer */
    const int capacity;
    
    /** The state of the buffer */
    enum class State {empty, partial, full};
    
    State state;
    
    /** The actual current size of the buffer */
    const int size() {
        return elements.size();
    }

    /** The weight of the buffer */
    int weight;
    
    /** The level of the buffer */
    int level;
    
    /** The elements */
    
    template <typename Scalar> std::vector<Scalar> elements;
        
    Buffer(capacity) : capacity(capacity), state(State::empty), weight(1), level(1) {
    }
    
    ~Buffer();

    template <typename Scalar> void new(std::vector<Scalar> elements, int level=0, int rate=1) {
        // should we actually be executing this per-element?
        // should we be iterating over the image in chunks?
        // should we be doing the random selection outside the function?
        // what if the chunk boundaries cross frame boundaries?
        // we need a function which accumulates values, regardless of where they come from
        // we could do a one-pass random selection per block, but this will be a problem with the last block
        // but we only have to store the last r, not the last k
        
        //if r == 1:
            //self.elements = [e for e in elements if e is not None]
        //else:
            //self.elements = []
            
            //for block in grouper(elements, r):
                //block = [e for e in block if e is not None]
                //self.elements.append(random.choice(block))

        //self.weight = r
        //if len(self.elements) == self.size:
            //self.state = self.FULL
        //else:
            //self.state = self.PARTIAL
        
        //self.level = level
        
        this->elements.clear();
        
        if (rate == 1) {
            for (auto& e : elements) {
                this->elements.push_back(e);
            }
        } else {
        }
    }

    static void collapse(std::vector<Buffer> input_buffers) {
    }
    
    static std::vector<double> output(std::vector<Buffer> input_buffers, std::vector<double> quantiles) {
    }
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
    
    // TODO: put the common implementation here
    auto process = [](const Scalar & val) {
        // ???
    };
    
    
    // put implementation here
    
    if (converter && converter->frameDependent) {
        // we need to apply the frame-dependent conversion to each intensity value before copying it
        view_lambda = [&converter, &hertzVal, &process](const Scalar & val) {
            if ( std::isfinite( val ) ) {
                process(converter->_frameDependentConvert(val, hertzVal));
            }
        };
        
        // to avoid calculating the frame index repeatedly we use slices to iterate over the image one frame at a time

        for (size_t f = 0; f < hertzValues.size(); f++) {
            double hertzVal = hertzValues[f];
            
            Carta::Lib::NdArray::Double viewSlice = Carta::Core::Algorithms::viewSliceForFrame(view, spectralIndex, f);

            // iterate over the frame
            viewSlice.forEach(view_lambda);
        }
        
    } else {
        // we don't have to do any conversions in the loop
        view_lambda = [&process] ( const Scalar & val ) {
            if ( std::isfinite( val ) ) {
                process(val);
            }
        };
        
        // and we can loop over the flat image
        view.forEach(view_lambda);
    }

    std::map < double, Scalar > result;

    CARTA_ASSERT( result.size() == percentiles.size());

    return result;
}

}
}
}
