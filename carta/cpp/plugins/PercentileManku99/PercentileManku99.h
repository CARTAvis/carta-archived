#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include "CartaLib/IPercentileCalculator.h"

#include <vector>
#include <queue>
#include <set>
#include <random>
#include <limits>
#include <functional>
#include <algorithm>

template<class T> using min_heap = std::priority_queue<T, std::vector<T>, std::greater<T> >;

template <typename Scalar>
class Buffer {
public:
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

    // TODO: we can probably just remove this
    Buffer() : weight(0), level(0) {
    }
};

template <typename Scalar>
class Manku99Algorithm {
public:
    Manku99Algorithm(
        const size_t numBuffers,
        const size_t bufferCapacity,
        const size_t sampleAfter,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter
    );
    
    /** Process a value */
    void process(const Scalar & val, const double & hzVal);

    /** OUTPUT operation */
    std::map<double, Scalar> opOutput(const std::vector<double> quantiles);
private:
    const size_t numBuffers;
    const size_t bufferCapacity;
    const size_t sampleAfter;
    
    size_t samplingRate;
    size_t newBufferLevel;
    size_t height;
    std::vector<Buffer<Scalar> > buffers;
    
    /** Temporary storage for blocks of data before and after sampling */
    std::vector<Scalar> elementBlock;
    std::vector<double> hzForBlock; // we need to keep these so that we can convert after sampling
    min_heap<Scalar> bufferElements;

    /** For unit conversion */
    Carta::Lib::IntensityUnitConverter::SharedPtr converter;
    std::function<void(Scalar, double)> blockPushLambda;
    std::function<Scalar(size_t)> convertedValueLambda;
    std::function<void()> blockClearLambda;

    /** Keep track of empty buffers */
    std::deque<Buffer<Scalar>*> empty;

    /** Keep track of full buffers and their levels */
    min_heap<size_t> fullLevelHeap;
    std::set<size_t> fullLevelSet;
    std::map<size_t, std::vector<Buffer<Scalar>*> > full;

    /** Randomness */
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<> dist;
    size_t ri;

    /** NEW operation */
    Buffer<Scalar>* opNew();

    /** Weighted buffer merge, used in COLLAPSE and OUTPUT operations */
    void merge(
        std::vector<Buffer<Scalar>*> inputBuffers, size_t start,
        std::function<bool()> stopIterationLambda,
        std::function<size_t(size_t)> nextIndexLambda,
        std::function<void(size_t, Scalar)> processValueLambda
    );

    /** Will be toggled in successive calls of the collapse operation */
    bool collapseChoice;
    
    /** COLLAPSE operation */
    void opCollapse(std::vector<Buffer<Scalar>*> & inputBuffers);

    /** Helper function to add a full buffer */
    void addFullBuffer(Buffer<Scalar> *& fullBuffer);
    
    /** Helper function to get all full buffers at the lowest level */
    std::vector<Buffer<Scalar>*> getLowest();
};


template <typename Scalar>
Manku99Algorithm<Scalar>::Manku99Algorithm(const size_t numBuffers, const size_t bufferCapacity, const size_t sampleAfter,
    Carta::Lib::IntensityUnitConverter::SharedPtr converter) : numBuffers(numBuffers),
    bufferCapacity(bufferCapacity), sampleAfter(sampleAfter),
    samplingRate(1), newBufferLevel(0), height(0)
{

    for (size_t i = 0; i < numBuffers; i++) {
        buffers.push_back(Buffer<Scalar>());
    }

    for (auto& b : buffers) {
        empty.push_back(&b);
    }

    mt = std::mt19937(rd());
    dist = std::uniform_int_distribution<>(0, samplingRate - 1);
    
    this->converter = converter;
    
    if (converter && converter->frameDependent) {
        blockPushLambda = [this] (const Scalar val, const double hzVal) {
            this->elementBlock.push_back(val);
            this->hzForBlock.push_back(hzVal);
        };
        
        convertedValueLambda = [this] (const size_t & ri) {
            return this->converter->_frameDependentConvert(this->elementBlock[ri], this->hzForBlock[ri]);
        };
        
        blockClearLambda = [this] () {
            this->elementBlock.clear();
            this->hzForBlock.clear();
        };
    } else {
        blockPushLambda = [this] (const Scalar val, const double hzVal) {
            Q_UNUSED(hzVal);
            this->elementBlock.push_back(val);
        };
        
        convertedValueLambda = [this] ( const size_t & ri) {
            return this->elementBlock[ri];
        };
        
        blockClearLambda = [this] () {
            this->elementBlock.clear();
        };
    }
}


template <typename Scalar>
Buffer<Scalar>* Manku99Algorithm<Scalar>::opNew() {
    Buffer<Scalar>* buffer = empty.front();
    
    buffer->elements = std::move(bufferElements);
    buffer->weight = samplingRate;
    buffer->level = newBufferLevel;
    
    empty.pop_front();
    
    return buffer;
}


template <typename Scalar>
void Manku99Algorithm<Scalar>::merge(
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
    Scalar minNextVal;
    // the corresponding buffer weight
    size_t weight;

    // the position within the virtual expanded list of elements
    size_t pos(0);
    // the next index to be sampled from the expanded list of elements
    size_t nextIndex(start);

    // while the destination element buffer isn't full
    while (!stopIterationLambda()) {
        // find the buffer with the lowest overall next value
        
        minNextVal = std::numeric_limits<Scalar>::max();
            
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
        if (remainingBuffers[minBufferIndex]->elements.empty()) {
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


template <typename Scalar>
void Manku99Algorithm<Scalar>::opCollapse(std::vector<Buffer<Scalar>*> & inputBuffers) {
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

    collapseChoice = !collapseChoice;

    // the destination for the sampled, merged elements
    min_heap<Scalar> YElements;

    auto stopIterationLambda = [this, &YElements] () {
        return (YElements.size() >= this->bufferCapacity);
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
        b->level = 0;
    }

    Buffer<Scalar>*& Y = inputBuffers[0];
    Y->elements = std::move(YElements);
    Y->weight = YWeight;
    Y->level = YLevel;
}


template <typename Scalar>
void Manku99Algorithm<Scalar>::addFullBuffer(Buffer<Scalar> *& fullBuffer) {
    size_t& level = fullBuffer->level;
    if (fullLevelSet.find(level) == fullLevelSet.end()) {
        // add the level to the set
        fullLevelSet.insert(level);
        // add the level to the heap
        fullLevelHeap.push(level);
    }
    // add the buffer to the map of full buffers
    full[newBufferLevel].push_back(fullBuffer);
}


template <typename Scalar>
std::vector<Buffer<Scalar>*> Manku99Algorithm<Scalar>::getLowest() {
    size_t lowestFullLevel = fullLevelHeap.top();
    fullLevelHeap.pop();
    fullLevelSet.erase(lowestFullLevel);
    
    std::vector<Buffer<Scalar>*> lowest = full[lowestFullLevel];
    full.erase(lowestFullLevel);
    return lowest;
}

template <typename Scalar>
void Manku99Algorithm<Scalar>::process(const Scalar & val, const double & hzVal) {
    if (empty.size()) {
        blockPushLambda(val, hzVal);
        
        if (elementBlock.size() == samplingRate) {
            // select random element from block, do unit conversion and push to elements
            ri = dist(mt);
            bufferElements.push(convertedValueLambda(ri));
            blockClearLambda();
            
            // create new buffer whenever elements reach buffer capacity
            // we'll do it once more at the end to account for a possible partial buffer
            if (bufferElements.size() == bufferCapacity) { // NEW operation
                Buffer<Scalar>* newBuffer = opNew();
                addFullBuffer(newBuffer);
            }
        }
    } else { // COLLAPSE operation
        // Find full buffers with the lowest level
        std::vector<Buffer<Scalar>*> lowest = getLowest();
        
        // If there is only one,
        if (lowest.size() == 1) {
            // add the next lowest buffer(s) and promote the lowest buffer
            std::vector<Buffer<Scalar>*> secondLowest = getLowest();
            lowest[0]->level = secondLowest[0]->level;
            secondLowest.push_back(lowest[0]);
            lowest = std::move(secondLowest);
        }

        // perform the collapse
        opCollapse(lowest);

        // all buffers after the first are now empty
        for (size_t i = 1; i < lowest.size(); i++) {
            empty.push_back(lowest[i]);
        }

        // the first buffer is full
        addFullBuffer(lowest[0]);
        
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


template <typename Scalar>
std::map<double, Scalar> Manku99Algorithm<Scalar>::opOutput(const std::vector<double> quantiles) {
    // process a possible partial block
    if (elementBlock.size()) {
        // select random element from block, do unit conversion and append to elements
        ri = dist(mt);
        // we may not select any element from this incomplete block
        if (ri < elementBlock.size()){
            bufferElements.push(convertedValueLambda(ri));
        }
        elementBlock.clear();
        hzForBlock.clear();
    }
    
    // process a possible partial buffer
    Buffer<Scalar>* partial;

    if (bufferElements.size()) {        
        partial = opNew();
    }

    // Final set of buffers to be used in output operation
    std::vector<Buffer<Scalar>*> nonEmptyBuffers;
    
    while (!fullLevelHeap.empty()) {
        std::vector<Buffer<Scalar>*> lowest = getLowest();
        nonEmptyBuffers.insert(nonEmptyBuffers.end(), lowest.begin(), lowest.end());
    }

    // partial buffer goes at the end
    if (partial) {
        nonEmptyBuffers.push_back(partial);
    }

    // kW is the sum of the weight x actual size of all input buffers
    size_t kW = std::accumulate (begin(nonEmptyBuffers), end(nonEmptyBuffers), 0, [](size_t a, Buffer<Scalar>*& b){ return (b->size() * b->weight) + a; });

    // Quantiles close together may have the same index
    // We need to eliminate the duplicates to avoid breaking the merge algorithm
    std::set<size_t> indicesSet;
    min_heap<size_t> indices;
    std::map<size_t, std::vector<double> > quantilesForIndex;
    
    for (auto & phi : quantiles) {
        size_t index = (size_t) std::max(ceil(phi * kW) - 1, 0.0);
        if (indicesSet.find(index) == indicesSet.end()) {
            indicesSet.insert(index);
            indices.push(index);
        }
        quantilesForIndex[index].push_back(phi);
    }
    
    bool stopIteration(false);
    
    auto stopIterationLambda = [&stopIteration] () {
        return stopIteration;
    };

    auto nextIndexLambda = [&indices, &stopIteration] (size_t lastIndex) {
        Q_UNUSED(lastIndex);
        size_t next(0);
        if (indices.empty()) {
            stopIteration = true;
        } else {
            next = indices.top();
            indices.pop();
        }
        return next;
    };

    std::map<double, Scalar> values;

    auto processValueLambda = [&quantilesForIndex, &values] (size_t index, Scalar value) {
        for (auto& q : quantilesForIndex[index]) {
            values[q] = value;
        }
    };
    
    // perform the weighted merge
    size_t startIndex = indices.top();
    indices.pop();
    merge(nonEmptyBuffers, startIndex, stopIterationLambda, nextIndexLambda, processValueLambda);

    return values;
}

/*****************************************************************************/

template <typename Scalar>
class PercentileManku99 : public Carta::Lib::IPercentilesToPixels<Scalar> {
public:
    PercentileManku99(
        const size_t numBuffers, 
        const size_t bufferCapacity, 
        const size_t sampleAfter
    );
        
    std::map<double, Scalar> percentile2pixels(
        Carta::Lib::NdArray::TypedView < Scalar > & view,
        std::vector <double> percentiles,
        int spectralIndex,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter,
        std::vector<double> hertzValues
    ) override;
    
    void reconfigure(const QJsonObject config) override;

private:
    size_t numBuffers;
    size_t bufferCapacity;
    size_t sampleAfter;
};

// TODO: error is completely wrong; work out what it actually is
template <typename Scalar>
PercentileManku99<Scalar>::PercentileManku99(const size_t numBuffers, const size_t bufferCapacity, const size_t sampleAfter) :Carta::Lib:: IPercentilesToPixels<Scalar>(0.5, "Manku99 approximation", true), numBuffers(numBuffers), bufferCapacity(bufferCapacity), sampleAfter(sampleAfter) {
}

template <typename Scalar>
std::map<double, Scalar> PercentileManku99<Scalar>::percentile2pixels(
    Carta::Lib::NdArray::TypedView < Scalar > & view,
    std::vector <double> percentiles,
    int spectralIndex,
    Carta::Lib::IntensityUnitConverter::SharedPtr converter,
    std::vector<double> hertzValues
) {
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

    double hertzVal;

    Manku99Algorithm<Scalar> algorithm(numBuffers, bufferCapacity, sampleAfter, converter);
    
    // Enter all the values into buffers
    if (converter && converter->frameDependent) {
        // to avoid calculating the frame index repeatedly we use slices to iterate over the image one frame at a time

        for (size_t f = 0; f < hertzValues.size(); f++) {
            hertzVal = hertzValues[f];
            Carta::Lib::NdArray::Double viewSlice = Carta::Lib::viewSliceForFrame(view, spectralIndex, f);
            
            // iterate over the frame
            viewSlice.forEach([&algorithm, &hertzVal] ( const Scalar & val ) {
                if (std::isfinite(val)) {
                    algorithm.process(val, hertzVal);
                }
            });
        }
    } else {
        auto view_lambda = [&algorithm] ( const Scalar & val ) {
            if (std::isfinite(val)) {
                algorithm.process(val, -1);
            }
        };
        
        // we can loop over the flat image
        view.forEach(view_lambda);
    }

    // call output to handle the end of the data and find the quantiles

    std::map <double, Scalar> result = algorithm.opOutput(percentiles);

    CARTA_ASSERT( result.size() == percentiles.size());

    return result;
}

template <typename Scalar>
void PercentileManku99<Scalar>::reconfigure(const QJsonObject config) {
    this->numBuffers = config["numBuffers"].toInt();
    this->bufferCapacity = config["bufferCapacity"].toInt();
    this->sampleAfter = config["sampleAfter"].toInt();
}
