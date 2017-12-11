#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include "CartaLib/IPercentileCalculator.h"

#include <QDebug>
#include <limits>
#include <algorithm>
#include <vector>
#include <cmath>
#include <numeric>
#include <QElapsedTimer>

template <typename Scalar>
class PercentileHistogram : public Carta::Lib::IPercentilesToPixels<Scalar> {
public:
    PercentileHistogram(const unsigned int numberOfBins);
    
    ~PercentileHistogram();
        
    std::map<double, Scalar> percentile2pixels(
        Carta::Lib::NdArray::TypedView < Scalar > & view,
        std::vector <double> percentiles,
        int spectralIndex,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter,
        std::vector<double> hertzValues
    ) override;
    
private:
    const unsigned int numberOfBins;
};


PercentileHistogram<Scalar>::PercentileHistogram(const unsigned int numberOfBins) : Carta::Lib::IPercentilesToPixels<Scalar>(1/numberOfBins, "Histogram approximation", true, true) : numberOfBins(numberOfBins) {
}


template <typename Scalar>
PercentileHistogram<Scalar>::percentile2pixels(
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
    
    std::vector<size_t> bins(numberOfBins+1, 0); // initialize the vector binss as 0

    double minIntensity = minMaxIntensities[0]; // get the minimum intensity
    double maxIntensity =  minMaxIntensities[1]; // get the maximum intensity
    // These should already have been provided in matching units, but we need to reverse any constant multiplier which may have been applied so that we can make valid comparisons to values inside the loop, which will also not have the constant multiplier applied. 
    if (converter) {
        minIntensity /= converter->multiplier;
        maxIntensity /= converter->multiplier;
    }
    double intensityRange = fabs(maxIntensity - minIntensity); // calculate the intensity range of the raw data
    unsigned int pixelIndex; // the index of vector bins

    // start timer for computing approximate percentiles
    QElapsedTimer timer;
    timer.start();

    // convert pixel values from raw data to 1-D histogram and save it in a vector
    if (converter && converter->frameDependent) {
        // we need to apply the frame-dependent conversion to each intensity value before using it        
        // to avoid calculating the frame index repeatedly we use slices to iterate over the image one frame at a time
        for (size_t f = 0; f < hertzValues.size(); f++) {
            hertzVal = hertzValues[f];
            
            Carta::Lib::NdArray::Double viewSlice = Carta::Lib::viewSliceForFrame(view, spectralIndex, f);

            // iterate over the frame
            viewSlice.forEach([&bins, &numberOfBins, &pixelIndex, &minIntensity, &intensityRange, &converter, &hertzVal] (const Scalar &val) {
                if (std::isfinite(val)) {
                    pixelIndex = static_cast<unsigned int>(round(numberOfBins * (converter->_frameDependentConvert(val, hertzVal) - minIntensity) / intensityRange));
                    bins[pixelIndex]++;
                }
            });
        }
    } else {
        // we don't have to do any conversions in the loop
        // and we can loop over the flat image
        view.forEach([&bins, &numberOfBins, &pixelIndex, &minIntensity, &intensityRange] (const Scalar &val) {
            if (std::isfinite(val)) {
                pixelIndex = static_cast<unsigned int>(round(numberOfBins * (val - minIntensity) / intensityRange));
                bins[pixelIndex]++;
            }
        });
    }
        
    // total number of finite values
    size_t finiteValueCount = std::accumulate(bins.begin(), bins.end(), 0);
    
    qDebug() << ", finite raw data number=" << finiteValueCount;

    // indicate bad clip if no finite numbers were found
    if ( finiteValueCount == 0 ) {
        qFatal( "The size of finite raw data is zero !!" );
    }

    std::map<double, Scalar> result;

    int sizeOfQuant = percentiles.size();
    size_t accumulateEvent = 0;
    size_t stopNo[sizeOfQuant];
    std::vector<bool> flag(sizeOfQuant, false);
    double pixelValue[sizeOfQuant];
    double pixelValueError = intensityRange/numberOfBins;

    // get accumulation numbers for histogram with respect to specific percentiles
    for (int j = 0; j < sizeOfQuant; j++) {
        stopNo[j] = percentiles[j] * finiteValueCount;
    }

    // convert histogram accumulation numbers to pixel values
    for (unsigned int i = 0; i < numberOfBins+1; i++) {
        accumulateEvent += bins[i];
        for (int j = 0; j < sizeOfQuant; j++) {
            if (accumulateEvent > stopNo[j] && flag[j] == false) {
                pixelValue[j] = (intensityRange * i / numberOfBins) + minIntensity;
                flag[j] = true;
            }
        }
    }

    // print out and save the results
    for (int j = 0; j < sizeOfQuant; j++) {
        qDebug() << "++++++++ for percentile=" << percentiles[j] << "intensity=" << pixelValue[j] << "+/-" << pixelValueError;
        result[percentiles[j]] = pixelValue[j];
    }

    // end of timer for loading the raw data
    int elapsedTime = timer.elapsed();
    if (CARTA_RUNTIME_CHECKS) {
        qCritical() << "<> Time to get the approximate value:" << elapsedTime << "ms";
    }

    return result;
}
