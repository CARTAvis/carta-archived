/**
 * Interfaces and helper classes for percentile-to-pixel and pixel-to-percentile calculations
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include <QString>
#include <vector>
#include <QJsonObject>

namespace Carta {
namespace Lib {

struct RegionHistogramData {
    //int32_t channel;
    int32_t num_bins;
    double bin_width;
    double first_bin_center;
    std::vector<uint32_t> bins;
};

/// helper function for calculating a slice for a single 2D frame of the image
static Carta::Lib::NdArray::Double viewSliceForFrame(Carta::Lib::NdArray::Double view, int spectralIndex, int frameIndex) {
    // Create a slice for retrieving a single frame
    SliceND frame;
    for (size_t d = 0; d < view.dims().size(); d++) {
        if ((int)d == spectralIndex) {
            frame.index(frameIndex);
        } else {
            frame.next();
        }
    }

    // Create a new view for a single frame using this slice
    Carta::Lib::NdArray::Double viewSlice( view.rawView()->getView(frame), false ); // create a new view for this slice
    return viewSlice;
}


template <typename Scalar>
class IPercentilesToPixels {
    CLASS_BOILERPLATE( IPercentilesToPixels );
public:
    /**
     * Constructor.
     */
    IPercentilesToPixels(const double error, const QString label, const bool isApproximate=false, const bool needsMinMax=false);
        
    /** The error margin for the returned values. In future this may be deprecated in favour of independent per-value error margins, if necessary. */
    double error;
    
    /** The name of this algorithm, for logging purposes. */
    const QString label;
    
    /** This may need to be rethought, but for now it's the simplest way to achieve this without having to calculate the minimum and maximum unnecessarily. */
    void setMinMax(std::vector<Scalar> minMaxIntensities);
    
    /** This is a hook which allows tests to reconfigure algorithm parameters.
     It would really be better to do this at the plugin level. */
    virtual void reconfigure(const QJsonObject config);
    
    /** The actual calculator function */
    virtual std::map<double, Scalar> percentile2pixels(
        Carta::Lib::NdArray::TypedView < Scalar > & view,
        std::vector <double> percentiles,
        int spectralIndex,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter,
        std::vector<double> hertzValues
    );

    virtual RegionHistogramData pixels2histogram(
        Carta::Lib::NdArray::TypedView <Scalar> & view,
        double minIntensity,
        double maxIntensity,
        int numberOfBins,
        int spectralIndex,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter,
        std::vector<double> hertzValues
    );

    const bool isApproximate=false;
    const bool needsMinMax=false;
    std::vector<Scalar> minMaxIntensities;
};

template <typename Scalar>
IPercentilesToPixels<Scalar>::IPercentilesToPixels(const double error, const QString label, const bool isApproximate, const bool needsMinMax) : error(error), label(label), isApproximate(isApproximate), needsMinMax(needsMinMax) {
}

template <typename Scalar>
void IPercentilesToPixels<Scalar>::setMinMax(std::vector<Scalar> minMaxIntensities) {
    this->minMaxIntensities = minMaxIntensities;
}

template <typename Scalar>
std::map<double, Scalar> IPercentilesToPixels<Scalar>::percentile2pixels(
    Carta::Lib::NdArray::TypedView < Scalar > & view,
    std::vector <double> percentiles,
    int spectralIndex,
    Carta::Lib::IntensityUnitConverter::SharedPtr converter,
    std::vector<double> hertzValues
) {
    Q_UNUSED(view);
    Q_UNUSED(percentiles);
    Q_UNUSED(spectralIndex);
    Q_UNUSED(converter);
    Q_UNUSED(hertzValues);
    qFatal( "Unimplemented virtual function");
}

template <typename Scalar>
RegionHistogramData IPercentilesToPixels<Scalar>::pixels2histogram(
    Carta::Lib::NdArray::TypedView <Scalar> & view,
    double minIntensity,
    double maxIntensity,
    int numberOfBins,
    int spectralIndex,
    Carta::Lib::IntensityUnitConverter::SharedPtr converter,
    std::vector<double> hertzValues
) {
    Q_UNUSED(view);
    Q_UNUSED(minIntensity);
    Q_UNUSED(maxIntensity);
    Q_UNUSED(numberOfBins);
    Q_UNUSED(spectralIndex);
    Q_UNUSED(converter);
    Q_UNUSED(hertzValues);
    qFatal( "Unimplemented virtual function: pixels2histogram()");
}

template <typename Scalar>
void IPercentilesToPixels<Scalar>::reconfigure(const QJsonObject config) {
    Q_UNUSED(config);
    qFatal( "Unimplemented virtual function");
}

template <typename Scalar>    
class IPixelsToPercentiles {
    CLASS_BOILERPLATE( IPixelsToPercentiles );
public:
    /**
     * Constructor.
     */
    IPixelsToPercentiles(const double error, const QString label, const bool isApproximate=false);
    
    /** The error margin for the returned values. In future this may be deprecated in favour of independent per-value error margins, if necessary. */
    const double error;
    
    /** The name of this algorithm, for logging purposes. */
    const QString label;
    
    /** The actual calculator function */
    virtual std::vector<double> pixels2percentiles(
        Carta::Lib::NdArray::TypedView < Scalar > & view,
        std::vector <Scalar> pixels,
        int spectralIndex,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter,
        std::vector<double> hertzValues
    );

    const bool isApproximate=false;
};

template <typename Scalar>
IPixelsToPercentiles<Scalar>::IPixelsToPercentiles(const double error, const QString label, const bool isApproximate) : error(error), label(label), isApproximate(isApproximate) {
}

template <typename Scalar>
std::vector<double> IPixelsToPercentiles<Scalar>::pixels2percentiles(
    Carta::Lib::NdArray::TypedView < Scalar > & view,
    std::vector <Scalar> pixels,
    int spectralIndex,
    Carta::Lib::IntensityUnitConverter::SharedPtr converter,
    std::vector<double> hertzValues
) {
    Q_UNUSED(view);
    Q_UNUSED(pixels);
    Q_UNUSED(spectralIndex);
    Q_UNUSED(converter);
    Q_UNUSED(hertzValues);
    qFatal( "Unimplemented virtual function");
}

}
}
