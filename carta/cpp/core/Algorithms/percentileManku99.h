/** Implementation of the approximate quantile algorithm described in
 * 'Random Sampling Techniques for Space Efficient Online Computation of 
 * Order Statistics of Large Datasets' (Manku et al., 1999) */

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include "percentileAlgorithms.h"
#include <vector>
#include <random>
 
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

    template <typename Scalar> void opNew(std::vector<Scalar> elements, int weight, int level) {        
        this->elements.clear();
        
        for (auto& e : elements) {
            this->elements.push_back(e);
        }

        state = this->size() == capacity ? State::full : State::partial;
        this->weight = weight;
        this->level = level;

    }

    static void opCollapse(std::vector<Buffer> input_buffers) {
    }
    
    static std::vector<double> opOutput(std::vector<Buffer> input_buffers, std::vector<double> quantiles) {
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
    std::vector<double> hertzValues={},
    // Manku 99 algorithm parameters
    int numBuffers=10,
    int bufferCapacity=1000,
    int sampleAfter=10
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

    int samplingRate(1);
    int newBufferLevel(0);
    
    int finite(0);
    
    std::vector<Scalar> bufferElements;

    std::vector<Buffer> buffers(numBuffers); // TODO does this create new buffer objects?
    
    std::deque<int> emptyBufferIndices(numBuffers);
    std::iota (std::begin(emptyBufferIndices), std::end(emptyBufferIndices), 0);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    auto makeNewBuffer = [&buffers, &bufferElements, &samplingRate, &newBufferLevel] () {        
        buffers[emptyBufferIndices.front()].opNew(bufferElements, samplingRate, newBufferLevel);
        emptyBufferIndices.pop_front();
        bufferElements.clear();
    };
    
    // TODO: process the values here
    auto process = [&samplingRate, &finite, &buffers, &bufferElements, &dist, &mt](const Scalar & val) {
        if ( std::isfinite( val ) ) {
            // TODO: do this only as long as there are empty buffers
            if (emptyBufferIndices.size()) {
                // this is horribly slow; should make a temp vector instead
                // sample current value with a probability of pos % rate / rate
                if (samplingRate == 1 || dist(mt) <= (finite % samplingRate + 1) / samplingRate) {
                    bufferElements.push_back(val);
                
                    // create new buffer whenever elements reach capacity
                    // we'll do it once at the end to account for a possible partial buffer
                    if (bufferElements.size() == bufferCapacity) {
                        makeNewBuffer(); // NEW operation
                    }
                }
            } else { // COLLAPSE operation
                
            }

            finite++;
        }
    };
    
    
    // Enter all the values into buffers
    
    double hertzVal;
    std::function<void(Scalar)> view_lambda;
    
    if (converter && converter->frameDependent) {
        // we need to apply the frame-dependent conversion to each intensity value before copying it
        view_lambda = [&converter, &hertzVal, &process](const Scalar & val) {
            process(converter->_frameDependentConvert(val, hertzVal));
        };
        
        // to avoid calculating the frame index repeatedly we use slices to iterate over the image one frame at a time

        for (size_t f = 0; f < hertzValues.size(); f++) {
            hertzVal = hertzValues[f];
            
            Carta::Lib::NdArray::Double viewSlice = Carta::Core::Algorithms::viewSliceForFrame(view, spectralIndex, f);

            // iterate over the frame
            viewSlice.forEach(view_lambda);
        }
        
    } else {
        // we don't have to do any conversions in the loop
        view_lambda = [&process] ( const Scalar & val ) {
            process(val);
        };
        
        // and we can loop over the flat image
        view.forEach(view_lambda);
    }

    // process a possible partial buffer
    if (bufferElements.size()) {
        makeNewBuffer();
    }

    // OUTPUT operation

    // Find the quantiles

    std::map < double, Scalar > result;

    // TODO

    CARTA_ASSERT( result.size() == percentiles.size());

    return result;
}

}
}
}
