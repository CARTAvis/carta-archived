/**
 * Miscelaneous algorithms related to computing percentiles
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include <QDebug>
#include <limits>
#include <algorithm>
#include <vector>
#include <cmath>
#include <numeric>
#include <QElapsedTimer>

namespace Carta
{
namespace Core
{
namespace Algorithms
{
/// compute requested percentiles
/// \param view the input dataset
/// \param quant which percentiles to compute
/// \return the computed percentiles. If all inputs are nans, the result will also be nans.
///
/// Example: [0.1] will compute a value such that 10% of all values are smaller than the returned
/// value.
///
/// \note this is a dumb algorithm using quickselect. It really only works on datasets that
/// are small enough to store in memory. For really big datasets we need a lot more sophisticated
/// algorithm.
///
/// \note NANs are treated as if they did not exist
///
/// \note for best performance, the supplied list of percentiles should be sorted small->large
template < typename Scalar >
static
typename std::map < double, Scalar >
percentile2pixels(
    Carta::Lib::NdArray::TypedView < Scalar > & view,
    std::vector < double > quant
    )
{
    //qDebug() << "++++++++ raw data shape for computing percentile per frame is" << view.dims();

    // basic preconditions
    if ( CARTA_RUNTIME_CHECKS ) {
        for ( auto q : quant ) {
            CARTA_ASSERT( 0.0 <= q && q <= 1.0 );
            Q_UNUSED(q);
        }
    }

    // read in all values from the view into memory so that we can do quickselect on it
    std::vector < Scalar > allValues;
    view.forEach(
        [& allValues] ( const Scalar & val ) {
            // check if the value from raw data is finite
            if ( std::isfinite( val ) ) {
                allValues.push_back( val );
            }
        }
    );

    // indicate bad clip if no finite numbers were found
    if ( allValues.size() == 0 ) {
        qFatal( "The size of raw data is zero !!" );
    }

    std::map < double, Scalar > result;

    // for every input percentile, do quickselect and store the result

    for ( double q : quant ) {
        // we clamp to incremented values and decrement at the end because size_t cannot be negative
        size_t x1 = Carta::Lib::clamp<size_t>(allValues.size() * q , 1, allValues.size()) - 1;
        CARTA_ASSERT( 0 <= x1 && x1 < allValues.size() );
        std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end() );
        result[q] = allValues[x1];
    }
    CARTA_ASSERT( result.size() == quant.size());

    return result;
} // computeClips


///
/// compute the percentile and return pixel values
/// with respect to their spectral channels
///
template < typename Scalar >
static
std::map<double, std::pair<int,Scalar>>
percentile2pixels_precise(
    Carta::Lib::NdArray::TypedView < Scalar > & view,
    int spectralIndex,
    std::vector < double > quant
    )
{
    // basic preconditions
    if ( CARTA_RUNTIME_CHECKS ) {
        for ( auto q : quant ) {
            CARTA_ASSERT( 0.0 <= q && q <= 1.0 );
            Q_UNUSED(q);
        }
    }

    // read in all values from the view into memory so that we can do quickselect on it
    size_t index = 0;
    std::vector<std::pair<int,Scalar>> allValues;
    std::map<double, std::pair<int,Scalar>> result;

    std::vector<int> dims = view.dims();
    size_t total_size = 1;
    for ( auto d : dims ) total_size *= d;
    qDebug() << "++++++++ raw data totalsize=" << total_size << "pixel elements, for raw data shape" << dims;

    size_t divisor = total_size;
    if (spectralIndex != -1) divisor /= dims[spectralIndex];
    qDebug() << "++++++++ raw data size per channel=" << divisor;

    // Preallocate space to avoid running out of memory unnecessarily through dynamic allocation
    allValues.reserve(total_size);

    // start timer for loading the raw data
    QElapsedTimer timer;
    timer.start();

    view.forEach(
        [&allValues, &index] ( const Scalar &val ) {
            // check if the value from raw data is finite
            if ( std::isfinite( val ) ) {
                allValues.push_back( std::make_pair(index, val) );
            }
            index++;
        }
    );
    qDebug() << "++++++++ raw data index number=" << index;

    // end of timer for loading the raw data
    int elapsedTime = timer.elapsed();
    if (CARTA_RUNTIME_CHECKS) {
        qCritical() << "<> Time to load the raw data:" << elapsedTime << "ms";
    }

    // indicate bad clip if no finite numbers were found
    if ( allValues.size() == 0 ) {
        qFatal( "The size of raw data is zero !!" );
    }

    // for every input percentile, do quickselect and store the result
    // only compare the intensity values and ignore the indices
    auto compareIntensityTuples = [] (const std::pair<int,Scalar>& lhs, const std::pair<int,Scalar>& rhs) { return lhs.second < rhs.second; };

    for ( double q : quant ) {
        // x1 is the locationIndex used for quantile calculation
        // we clamp to incremented values and decrement at the end because size_t cannot be negative
        size_t x1 = Carta::Lib::clamp<size_t>(allValues.size() * q , 1, allValues.size()) - 1;
        CARTA_ASSERT( 0 <= x1 && x1 < allValues.size() );

        // start timer for getting the nth_element from raw data
        QElapsedTimer timer2;
        timer2.start();

        std::nth_element( allValues.begin(), allValues.begin()+x1, allValues.end(), compareIntensityTuples );

        // end of timer for getting the nth_element from the raw data
        int elapsedTime2 = timer2.elapsed();
        if (CARTA_RUNTIME_CHECKS) {
            qCritical() << "<> Time to get std::nth_element=" << q << "from raw data:" << elapsedTime2 << "ms";
        }

        result[q] = std::make_pair(allValues[x1].first/divisor, allValues[x1].second);
    }

    return result;
}


///
/// C.C. Chiang: compute the approximation of percentile and return
///              pixel values (w or w/o their spectral channels)
///
template < typename Scalar >
static
std::map<double, std::pair<int,Scalar>>
percentile2pixels_approximation(
    Carta::Lib::NdArray::TypedView <Scalar> & view,
    int spectralIndex,
    std::vector<std::pair<int,double>> minMaxIntensities,
    unsigned int pixelDividedNo,
    bool getLocation,
    std::vector<double> quant
    )
{
    // basic preconditions
    if ( CARTA_RUNTIME_CHECKS ) {
        for ( auto q : quant ) {
            CARTA_ASSERT( 0.0 <= q && q <= 1.0 );
            Q_UNUSED(q);
        }
    }

    size_t index = 0; // define raw data index
    size_t indexOfFinite = 0; // define raw data index with finite value
    std::map<double, std::pair<int,Scalar>> result; // define the output results

    std::vector<int> dims = view.dims();
    size_t total_size = 1;
    for ( auto d : dims ) total_size *= d;
    qDebug() << "++++++++ raw data totalsize=" << total_size << "pixel elements, for raw data shape" << dims;

    size_t divisor = total_size;
    if (spectralIndex != -1) divisor /= dims[spectralIndex];
    qDebug() << "++++++++ raw data size per channel=" << divisor;

    std::vector<size_t> element(pixelDividedNo+1, 0); // initialize the vector elements as 0

    double minIntensity = minMaxIntensities[0].second; // get the minimum intensity
    double maxIntensity =  minMaxIntensities[1].second; // get the maximum intensity
    double intensityRange = fabs(maxIntensity - minIntensity); // calculate the intensity range of the raw data
    unsigned int pixelIndex; // the index of vector element

    // start timer for computing approximate percentiles
    QElapsedTimer timer;
    timer.start();

    // convert pixel values from raw data to 1-D histogram and save it in a vector
    view.forEach(
        [&index, &indexOfFinite, &element, &pixelDividedNo, &pixelIndex, &minIntensity, &intensityRange] (const Scalar &val) {
            if (std::isfinite(val)) {
                pixelIndex = static_cast<unsigned int>(round(pixelDividedNo*(val-minIntensity)/intensityRange));
                element[pixelIndex]++;
                indexOfFinite++;
            }
            index++;
        }
    );
    qDebug() << "++++++++ raw data number=" << index << ", finite raw data number=" << indexOfFinite;

    // indicate bad clip if no finite numbers were found
    if ( index == 0 ) {
        qFatal( "The size of raw data is zero !!" );
    }

    int sizeOfQuant = quant.size();
    size_t accumulateEvent = 0;
    size_t stopNo[sizeOfQuant];
    std::vector<bool> flag(sizeOfQuant, false);
    double pixelValue[sizeOfQuant];
    double pixelValueError = intensityRange/pixelDividedNo;
    std::vector<size_t> pixelValueLocation(sizeOfQuant, 0);
    std::vector<unsigned int> count(sizeOfQuant, 0);

    // get accumulation numbers for histogram with respect to specific percentiles
    for (int j = 0; j < sizeOfQuant; j++) {
        stopNo[j] = quant[j] * indexOfFinite;
    }

    // convert histogram accumulation numbers to pixel values
    for (unsigned int i = 0; i < pixelDividedNo+1; i++) {
        accumulateEvent += element[i];
        for (int j = 0; j < sizeOfQuant; j++) {
            if (accumulateEvent > stopNo[j] && flag[j] == false) {
                pixelValue[j] = (intensityRange * i / pixelDividedNo) + minIntensity;
                flag[j] = true;
            }
        }
    }

    // this step (get location) will spend more time, so by default it is *false*.
    // we may skip this step if it would not affect the colormap setting.
    if (getLocation == true) {
        // get location index with respect to the approximate pixel value
        int quantIndex;
        size_t locationIndex = 0;
        view.forEach(
            [&pixelValue, &pixelValueLocation, &quantIndex, &sizeOfQuant, &count, &locationIndex, &pixelValueError] (const Scalar &val) {
                // check if the value from raw data is finite
                if (std::isfinite(val)) {
                    for (quantIndex = 0; quantIndex < sizeOfQuant; quantIndex++) {
                        if (fabs(val-pixelValue[quantIndex]) <= 2.0*pixelValueError) {
                            pixelValueLocation[quantIndex] = locationIndex;
                            count[quantIndex]++;
                        }
                    }
                }
                locationIndex++;
            }
        );
        for (int j = 0; j < sizeOfQuant; j++) {
            qDebug() << "++++++++ for percentile=" << quant[j]
                     << "location=" << pixelValueLocation[j]/divisor << "(channel)" << "candidates within error bar=" << count[j];
        }
    }

    // print out and save the results
    for (int j = 0; j < sizeOfQuant; j++) {
        qDebug() << "++++++++ for percentile=" << quant[j] << "intensity=" << pixelValue[j] << "+/-" << pixelValueError;
        result[quant[j]] = std::make_pair(pixelValueLocation[j]/divisor, pixelValue[j]);
    }

    // end of timer for loading the raw data
    int elapsedTime = timer.elapsed();
    if (CARTA_RUNTIME_CHECKS) {
        qCritical() << "<> Time to get the approximate value:" << elapsedTime << "ms";
    }

    return result;
}


///
/// C.C. Chiang: compute the minimum and maximum pixel values
///              with respect to their spectral channels
///
template < typename Scalar >
static
std::map<double, std::pair<int,Scalar>>
minMax2pixels(
    Carta::Lib::NdArray::TypedView < Scalar > & view,
    int spectralIndex,
    std::vector < double > quant
    )
{
    // basic preconditions
    if ( CARTA_RUNTIME_CHECKS ) {
        for ( auto q : quant ) {
            CARTA_ASSERT( 0.0 <= q && q <= 1.0 );
            Q_UNUSED(q);
        }
    }

    size_t index = 0;
    size_t indexOfFinite = 0;
    std::map<double, std::pair<int,Scalar>> result;
    double minPixel = 0.0;
    double maxPixel = 0.0;
    size_t indexOfMinPixel = 0;
    size_t indexOfMaxPixel = 0;

    std::vector<int> dims = view.dims();
    size_t total_size = 1;
    for ( auto d : dims) total_size *= d;
    qDebug() << "++++++++ raw data size:" << total_size << "pixel elements, for raw data shape" << dims;

    size_t divisor = total_size;
    if (spectralIndex != -1) divisor /= dims[spectralIndex];
    qDebug() << "++++++++ raw data size per channel=" << divisor;

    // start timer for scanning the raw data
    QElapsedTimer timer;
    timer.start();

    // scan the raw data from the view to get the minimum and maximum pixel values
    // with respect to their spectral channels
    view.forEach(
        [&index, &indexOfFinite, &minPixel, &maxPixel, &indexOfMinPixel, &indexOfMaxPixel] ( const Scalar &val ) {
            // check if the value from raw data is finite
            if ( std::isfinite( val ) ) {
                if (indexOfFinite == 0) {
                    minPixel = val;
                    maxPixel = val;
                } else {
                    if (val < minPixel) {
                        minPixel = val;
                        indexOfMinPixel = index;
                    }
                    if (val > maxPixel) {
                        maxPixel = val;
                        indexOfMaxPixel = index;
                    }
                }
                indexOfFinite++;
            }
            index++;
        }
    );
    qDebug() << "++++++++ raw data index number=" << index << ", raw data finite index number=" << indexOfFinite;

    // end of timer for loading the raw data
    int elapsedTime = timer.elapsed();
    if (CARTA_RUNTIME_CHECKS) {
        qCritical() << "<> Time to scan the raw data:" << elapsedTime << "ms";
    }

    // check if the data size is zero
    if (index == 0) {
        qFatal( "The size of raw data is zero !!" );
    }

    for ( double q : quant ) {
        if (q==0) result[q] = std::make_pair(indexOfMinPixel/divisor, minPixel);
        if (q==1) result[q] = std::make_pair(indexOfMaxPixel/divisor, maxPixel);
    }

    return result;
}


}
}
}
