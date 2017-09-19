/** Implementation of the approximate quantile algorithm described in
 * 'Random Sampling Techniques for Space Efficient Online Computation of 
 * Order Statistics of Large Datasets' (Manku et al., 1999) */

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include "percentileAlgorithms.h"
#include <vector>
#include <queue>
#include <set>
#include <random>
#include <limits>
#include <functional>
#include <algorithm>
 
namespace Carta
{
namespace Core
{
namespace Algorithms
{

template<class T> using min_heap = std::priority_queue<T, std::vector<T>, std::greater<T> >;

template <typename Scalar>
class Buffer {
public:
    /** The capacity of the buffer */
    const size_t capacity;
    
    /** The state of the buffer */
    enum class State {empty, partial, full};
    
    State state;
    
    /** The actual current size of the buffer */
    size_t size() const {
        return elements.size();
    }

    /** The weight of the buffer */
    size_t weight;
    
    /** The level of the buffer */
    size_t level;
    
    /** The elements */
    min_heap<Scalar> elements;

    /** Will be toggled in successive calls of the collapse operation */
    static bool collapseChoice;
        
    Buffer(const size_t capacity) : capacity(capacity), state(State::empty), weight(0), level(0) {
    }
    
    /** NEW operation */
    void opNew(min_heap<Scalar>& elements, size_t weight, size_t level, State state) {        
        this->elements = std::move(elements);
        this->state = state;
        this->weight = weight;
        this->level = level;
    }

    /** Weighted buffer merge */
    static void merge(
        std::vector<Buffer<Scalar>*> inputBuffers, size_t start,
        std::function<bool()> stopIterationLambda,
        std::function<size_t(size_t)> nextIndexLambda,
        std::function<void(size_t, Scalar)> processValueLambda
    ) {

        // buffers which have elements left
        std::vector<Buffer<Scalar>*> remainingBuffers;
        for (auto& b : inputBuffers) {
            remainingBuffers.push_back(b);
        }

        // the index of the buffer with the overall lowest next value
        size_t minBufferIndex;
        // the overall lowest next value
        Scalar minNextVal(std::numeric_limits<Scalar>::infinity());
        // the corresponding buffer weight
        size_t weight;

        // the position within the virtual expanded list of elements
        size_t pos(0);
        // the next index to be sampled from the expanded list of elements
        size_t nextIndex(start);

        // while the destination element buffer isn't full
        while (!stopIterationLambda()) {
            // find the buffer with the lowest overall next value
            for (size_t i = 0; i < remainingBuffers.size(); i++) {
                if (remainingBuffers[i]->elements.top() < minNextVal) {
                    minBufferIndex = i;
                    // peek at the value here
                    minNextVal = remainingBuffers[i]->elements.top();
                    weight = remainingBuffers[i]->weight;
                }
            }

            // actually pop the value here
            remainingBuffers[minBufferIndex]->elements.pop();

            // if the buffer is now empty, remove it from the list of non-empty buffers
            // it doesn't matter if any elements are left in some of the buffers after the merge
            // because we're going to overwrite them in the NEW operation afterwards
            if (!remainingBuffers[minBufferIndex]->elements.size()) {
                remainingBuffers.erase(remainingBuffers.begin() + minBufferIndex);
            }

            // I'm sure there's a cleaner way to do this
            // if any samples fall within the next [weight] elements of the expanded list, use this value for those samples
            for (size_t p = pos; p < pos + weight; p++) {
                if (p == nextIndex) {
                    processValueLambda(nextIndex, minNextVal);
                    nextIndex = nextIndexLambda(nextIndex);
                }
            }
            // actually advance our position in the expanded list
            pos += weight;
        }
    }

    /** COLLAPSE operation */
    static void opCollapse(std::vector<Buffer<Scalar>*> & inputBuffers) {
        // Weight of collapsed buffer is the sum of the weights of all input buffers
        size_t YWeight = std::accumulate (begin(inputBuffers), end(inputBuffers), 0, [](size_t a, Buffer<Scalar>*& b){ return b->weight + a; });
        // Level of collapsed buffer is one more than the level of each input buffer
        size_t YLevel = inputBuffers[0]->level + 1;

        // Calculate sampling offset from total weight 
        size_t offset;
        if (YWeight % 2) { // odd
            offset = (YWeight + 1) / 2;
        } else if (collapseChoice) { // even (alternative 1)
            offset = YWeight / 2;
        } else { // even (alternative 2)
            offset = (YWeight + 2) / 2;
        }

        collapseChoice = !collapseChoice; // do we need to do something special because it's static?

        // the maximum size of the merged sampled list of elements 
        size_t bufferCapacity = inputBuffers[0]->capacity;
        // the destination for the sampled, merged elements
        min_heap<Scalar> YElements;

        auto stopIterationLambda = [&bufferCapacity, &YElements] () {
            return (YElements.size() < bufferCapacity);
        };

        auto nextIndexLambda = [&YWeight] (size_t lastIndex) {
            return lastIndex + YWeight;
        };

        auto processValueLambda = [&YElements] (size_t index, Scalar value) {
            Q_UNUSED(index);
            YElements.push(value);
        };

        // perform the weighted merge
        merge(inputBuffers, offset - 1, stopIterationLambda, nextIndexLambda, processValueLambda);

        for (auto& b : inputBuffers) {
            // weight and level are cosmetic here; is there a performance benefit to not setting them?
            b->weight = 0;
            b->state = State::empty;
            b->level = 0;
        }

        Buffer<Scalar>*& Y = inputBuffers[0];
        Y->elements = std::move(YElements);
        Y->weight = YWeight;
        Y->state = State::full;
        Y->level = YLevel;
    }
    

    /** OUTPUT operation */
    static std::map<double, Scalar> opOutput(std::vector<Buffer<Scalar>*> inputBuffers, const std::vector<double> quantiles) {
        // kW is the sum of the weight x actual size of all input buffers
        size_t kW = std::accumulate (begin(inputBuffers), end(inputBuffers), 0, [](size_t a, Buffer<Scalar>*& b){ return (b->size() * b->weight) + a; });

        min_heap<size_t> indices;
        std::map<size_t, double> quantileForIndex;
        
        for (auto & phi : quantiles) {
            size_t index = ceil(phi * kW) - 1;
            indices.push(index);
            quantileForIndex[index] = phi;
        }

        auto stopIterationLambda = [&indices] () {
            return indices.empty();
        };

        auto nextIndexLambda = [&indices] (size_t lastIndex) {
            Q_UNUSED(lastIndex);
            size_t next = indices.top();
            indices.pop();
            return next;
        };

        std::map<double, Scalar> values;

        auto processValueLambda = [&quantileForIndex, &values] (size_t index, Scalar value) {
            values[quantileForIndex[index]] = value;
        };
        
        // perform the weighted merge
        merge(inputBuffers, 0, stopIterationLambda, nextIndexLambda, processValueLambda);

        return values;
    }
};

template <typename Scalar> bool Buffer<Scalar>::collapseChoice;

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
    size_t numBuffers=10,
    size_t bufferCapacity=1000,
    size_t sampleAfter=10
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

    size_t samplingRate(1);
    size_t newBufferLevel(0);
    size_t height(0);
    
    std::vector<Buffer<Scalar> > buffers;
    for (size_t i = 0; i < numBuffers; i++) {
        buffers.push_back(Buffer<Scalar>(bufferCapacity));
    }

    /** Temporary storage for blocks of data before and after sampling */
    std::vector<Scalar> elementBlock;
    std::vector<double> hzForBlock; // we need to keep these so that we can convert after sampling
    min_heap<Scalar> bufferElements;

    /** Keep track of empty buffers */
    std::deque<Buffer<Scalar>*> empty; // this is safe because the buffer vector will never change size; buffers are modified in-place
    for (auto& b : buffers) { // is this correct?
        empty.push_back(&b);
    }
    
    /** Keep track of full buffers and their levels */
    min_heap<size_t> fullLevelHeap;
    std::set<size_t> fullLevelSet;
    std::map<size_t, std::vector<Buffer<Scalar>*> > full;
    
    size_t lowestFullLevel;
    std::vector<Buffer<Scalar>*> lowest;
    
    size_t secondLowestFullLevel;
    std::vector<Buffer<Scalar>*> secondLowest;

    /** Randomness */
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> dist(0, samplingRate - 1);
    size_t ri;

    // TODO this whole thing should probably go in the Buffer class
    auto process = [&](const Scalar & val, const double & hzVal) {
        if (std::isfinite(val)) {
            if (empty.size()) {                    
                elementBlock.push_back(val);
                hzForBlock.push_back(hzVal);

                if (elementBlock.size() == samplingRate) {
                    // select random element from block, do unit conversion and push to elements
                    ri = dist(mt);
                    bufferElements.push(conversion_lambda(ri));
                    elementBlock.clear();
                    hzForBlock.clear();
                    
                    // create new buffer whenever elements reach buffer capacity
                    // we'll do it once more at the end to account for a possible partial buffer
                    if (bufferElements.size() == bufferCapacity) { // NEW operation
                        Buffer<Scalar> *& newBuffer = empty.front();
                        newBuffer->opNew(bufferElements, samplingRate, newBufferLevel, Buffer<Scalar>::State::full);
                        if (fullLevelSet.find(newBufferLevel) != fullLevelSet.end()) {
                            // add the level to the set
                            fullLevelSet.insert(newBufferLevel);
                            // add the level to the heap
                            fullLevelHeap.push(newBufferLevel);
                        }
                        // add the buffer to the map of full buffers
                        full[newBufferLevel].push_back(newBuffer);
                        // remove buffer from list of empty buffers
                        empty.pop_front();
                    }
                }
            } else { // COLLAPSE operation
                // Find full buffers with the lowest level
                lowestFullLevel = fullLevelHeap.top();
                fullLevelHeap.pop();
                fullLevelSet.erase(lowestFullLevel);
                lowest = full[lowestFullLevel];
                full.erase(lowestFullLevel);

                // If there is only one,
                if (lowest.size() == 1) {
                    // add the next lowest buffer(s)
                    secondLowestFullLevel = fullLevelHeap.top();
                    fullLevelHeap.pop();
                    fullLevelSet.erase(secondLowestFullLevel);
                    secondLowest = full[secondLowestFullLevel];
                    full.erase(secondLowestFullLevel);
                    // and promote the lowest buffer
                    lowest[0]->level = secondLowestFullLevel;
                    secondLowest.push_back(lowest[0]);
                    lowest = std::move(secondLowest);
                }

                // perform the collapse
                Buffer<Scalar>::opCollapse(lowest);

                // all buffers after the first are now empty
                for (size_t i = 1; i < lowest.size(); i++) {
                    empty.push_back(lowest[i]);
                }
                
                // the first buffer is full and one level higher
                if (fullLevelSet.find(lowest[0]->level) != fullLevelSet.end()) {
                    // add the level to the set
                    fullLevelSet.insert(lowest[0]->level);
                    // add the level to the heap
                    fullLevelHeap.push(lowest[0]->level);
                }
                // add the buffer to the map of full buffers
                full[lowest[0]->level].push_back(lowest[0]);
                
                // update tree height
                height = std::max(height, lowest[0]->level);

                // update new buffer level, sampling rate and random distribution
                if (height >= sampleAfter) {
                    newBufferLevel = height - sampleAfter + 1;
                    samplingRate = pow(2, newBufferLevel);
                    dist = std::uniform_int_distribution<>(0, samplingRate - 1);
                }
            }
        }
    };
    
    // Enter all the values into buffers
    if (converter && converter->frameDependent) {
        // conversion is needed
        conversion_lambda = [&elementBlock, &hzForBlock, &converter] ( const size_t & ri) {
            return converter->_frameDependentConvert(elementBlock[ri], hzForBlock[ri]);
        };
        
        // to avoid calculating the frame index repeatedly we use slices to iterate over the image one frame at a time

        for (size_t f = 0; f < hertzValues.size(); f++) {
            hertzVal = hertzValues[f];
            
            Carta::Lib::NdArray::Double viewSlice = Carta::Core::Algorithms::viewSliceForFrame(view, spectralIndex, f);

            // iterate over the frame
            viewSlice.forEach([&process, &hertzVal] ( const Scalar & val ) {
                process(val, hertzVal);
            });
        }
        
    } else {
        // conversions are no-ops
        conversion_lambda = [&elementBlock, &hzForBlock] ( const size_t & ri) {
            return elementBlock[ri];
        };

        auto view_lambda = [&process] ( const Scalar & val ) {
            process(val, -1);
        };
        
        // we can loop over the flat image
        view.forEach(view_lambda);
    }

    /** Special cases for handling the end of the data */

    Buffer<Scalar>* partial;
    
    // process a possible partial block
    if (elementBlock.size()) {
        // select random element from block, do unit conversion and append to elements
        ri = dist(mt);
        // we may not select any element from this incomplete block
        if (ri < elementBlock.size()){
            bufferElements.push(conversion_lambda(ri));
        }
        elementBlock.clear();
        hzForBlock.clear();
    }
    // process a possible partial buffer
    if (bufferElements.size()) {        
        empty.front()->opNew(bufferElements, samplingRate, newBufferLevel, Buffer<Scalar>::State::partial);
        partial = empty.front();
        empty.pop_front();
    }

    // OUTPUT operation

    // find the non-empty buffers
    
    std::vector<Buffer<Scalar>*> nonEmptyBuffers;

    while (!fullLevelHeap.empty()) {
        lowestFullLevel = fullLevelHeap.top();
        fullLevelHeap.pop();
        fullLevelSet.erase(lowestFullLevel);
        nonEmptyBuffers.insert(nonEmptyBuffers.end(), full[lowestFullLevel].begin(), full[lowestFullLevel].end());
        full.erase(lowestFullLevel);
    }

    // partial buffer goes at the end
    if (partial) {
        nonEmptyBuffers.push_back(partial);
    }

    // call output to find the quantiles

    std::map <double, Scalar> result = Buffer<Scalar>::opOutput(nonEmptyBuffers, percentiles);

    CARTA_ASSERT( result.size() == percentiles.size());

    return result;
}

}
}
}
