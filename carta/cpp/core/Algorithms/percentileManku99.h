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
        
    Buffer(capacity) : capacity(capacity), state(State::empty), weight(0), level(0) {
    }
    
    ~Buffer();

    template <typename Scalar> void opNew(std::vector<Scalar>& elements, int weight, int level, State state) {        
        this->elements = std::move(elements);
        this->state = state;
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

    /** for conversion*/
    double hertzVal;
    std::function<Scalar(Scalar)> conversion_lambda;

    int samplingRate(1);
    int newBufferLevel(0);
    
    std::vector<Buffer> buffers;
    for (size_t i = 0; i < numBuffers; i++) {
        buffers.push_back(Buffer(bufferCapacity));
    }

    /** Temporary storage for blocks of data before and after sampling */
    std::vector<Scalar> elementBlock;
    std::vector<double> hzForBlock; // we need to keep these so that we can convert after sampling
    std::vector<Scalar> bufferElements;

    /** Keep track of empty buffers */
    std::deque<Buffer*> empty; // this is safe because the buffer vector will never change size; buffers are modified in-place
    for (size_t i = 0; i < buffers.size(); i++) {
        empty.push_back(&buffers[i]);
    }
    
    /** Keep track of full buffers and their levels */
    std::heap<int> fullLevelHeap; // TODO: need a smallest element heap
    std::set<int> fullLevelSet;
    std::map<int, std::vector<Buffer*> > full;

    /** Randomness */
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> dist(0, samplingRate - 1);
    int ri;

    auto process = [&](const Scalar & val, const double & hzVal) {
        if ( std::isfinite( val ) ) {
            if (empty.size()) {                    
                elementBlock.push_back(val);
                hzForBlock.push_back(hzVal);

                if (elementBlock.size() == samplingRate) {
                    // select random element from block, do unit conversion and append to elements
                    ri = dist(mt);
                    bufferElements.push_back(conversion_lambda(ri));
                    elementBlock.clear();
                    framesForBlock.clear();
                    
                    // create new buffer whenever elements reach buffer capacity
                    // we'll do it once more at the end to account for a possible partial buffer
                    if (bufferElements.size() == bufferCapacity) { // NEW operation
                        empty.front()->opNew(bufferElements, samplingRate, newBufferLevel, Buffer::State::full);
                        // TODO update full buffers / levels
                        empty.pop_front();
                    }
                }
            } else { // COLLAPSE operation
                // TODO find buffers to collapse
                // do the collapse
                // update empty / full
                // update height / level / rate
                // update distribution
            }
        }
    };
    
    
    // Enter all the values into buffers
    if (converter && converter->frameDependent) {
        // conversion is needed
        conversion_lambda = [&elementBlock, &hzForBlock, &converter] ( const int & ri) {
            return converter->_frameDependentConvert(elementBlock[ri], hzForBlock[ri]);
        };
        
        // to avoid calculating the frame index repeatedly we use slices to iterate over the image one frame at a time

        for (size_t f = 0; f < hertzValues.size(); f++) {
            hertzVal = hertzValues[f];
            
            Carta::Lib::NdArray::Double viewSlice = Carta::Core::Algorithms::viewSliceForFrame(view, spectralIndex, f);

            // iterate over the frame
            viewSlice.forEach([&process] ( const Scalar & val ) {
                process(val, hertzVal);
            });
        }
        
    } else {
        // conversions are no-ops
        conversion_lambda = [&elementBlock, &hzForBlock] ( const int & ri) {
            return elementBlock[ri];
        };
        
        // we can loop over the flat image
        view.forEach([&process] ( const Scalar & val ) {
            process(val, -1);
        });
    }

    /** Special cases for handling the end of the data */
    
    // process a possible partial block
    if (elementBlock.size()) {
        // select random element from block, do unit conversion and append to elements
        ri = dist(mt);
        // we may not select any element from this incomplete block
        if (ri < elementBlock.size()){
            bufferElements.push_back(conversion_lambda(ri));
        }
        elementBlock.clear();
        framesForBlock.clear();
    }
    // process a possible partial buffer
    if (bufferElements.size()) {        
        empty.front()->opNew(bufferElements, samplingRate, newBufferLevel, Buffer::State::partial);
        empty.pop_front();
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
