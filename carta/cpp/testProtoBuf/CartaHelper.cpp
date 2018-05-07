#include "core/Globals.h"

// we want to check percentile functions from this code
#include "core/Algorithms/percentileAlgorithms.h"

// get unit conversion headers
#include "CartaLib/Hooks/ConversionSpectralHook.h"
#include "CartaLib/Hooks/ConversionIntensityHook.h"

#include <QDebug>
#include <cmath>

using namespace std;

const std::vector<double> percentile = {0.0005, 0.0025, 0.005, 0.01, 0.015,
                                        0.02, 0.025, 0.0375, 0.05, 0.95,
                                        0.9625, 0.975, 0.98, 0.985, 0.99,
                                        0.995, 0.9975, 0.9995};

typedef Carta::Lib::AxisInfo AxisInfo;

int getAxisIndex(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                 Carta::Lib::AxisInfo::KnownType axisType) {
    int index = -1;
    if (m_image){
        std::shared_ptr<CoordinateFormatterInterface> cf(m_image->metaData()->coordinateFormatter()->clone());
        int axisCount = cf->nAxes();
        for ( int i = 0; i < axisCount; i++ ){
            Carta::Lib::AxisInfo axisInfo = cf->axisInfo(i);
            if (axisInfo.knownType() == axisType){
                index = i;
                break;
            }
        }
    }
    return index;
}

Carta::Lib::NdArray::RawViewInterface* getRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                                  int frameStart, int frameEnd, int stokeSliceIndex) {
    Carta::Lib::NdArray::RawViewInterface* rawData = nullptr;
    int m_axisIndexX = getAxisIndex(m_image, AxisInfo::KnownType::DIRECTION_LON);
    int m_axisIndexY = getAxisIndex(m_image, AxisInfo::KnownType::DIRECTION_LAT);
    int stokeIndex = getAxisIndex(m_image, AxisInfo::KnownType::STOKES);
    int spectralIndex = getAxisIndex(m_image, AxisInfo::KnownType::SPECTRAL);
    qDebug() << "++++++++ X Index:" << m_axisIndexX;
    qDebug() << "++++++++ Y Index:" << m_axisIndexY;
    qDebug() << "++++++++ Stoke Index:" << stokeIndex;
    qDebug() << "++++++++ Spectral Index:" << spectralIndex;

    if (m_axisIndexX != 0 || m_axisIndexY != 1) {
        qFatal("Can not find x or y axises");
    }

    if (m_image) {
        int imageDim = m_image->dims().size();
        qDebug() << "++++++++ the dimension of image:" << imageDim;

        SliceND frameSlice = SliceND().next();

        for (int i = 0; i < imageDim; i++) {
            if (i != m_axisIndexX && i != m_axisIndexY) {

                // get the number of slice (e.q. channel) in this dimension
                int sliceSize = m_image -> dims()[i];

                // make slice cuts for the raw data
                SliceND & slice = frameSlice.next();

                // If it is the spectral axis
                if (i == spectralIndex) {
                   if (0 <= frameStart && frameStart < sliceSize &&
                       0 <= frameEnd && frameEnd < sliceSize){
                       slice.start(frameStart);
                       slice.end(frameEnd + 1);
                   } else {
                       slice.start(0);
                       slice.end(sliceSize);
                   }

                // If it is the stoke axis
                } else if (i == stokeIndex) {
                    if (stokeSliceIndex >= 0 && stokeSliceIndex <= 3) {
                        qDebug() << "++++++++ we only consider the stoke" << stokeSliceIndex <<
                                    "(-1: no stoke, 0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V) for percentile calculation" ;
                        slice.start(stokeSliceIndex);
                        slice.end(stokeSliceIndex + 1);
                    } else {
                        slice.start(0);
                        slice.end(sliceSize);
                    }

                // For the other axises
                } else {
                    slice.start(0);
                    slice.end(sliceSize);
                }

                slice.step(1);
            }
        }
        rawData = m_image->getDataSlice(frameSlice);
    }
    return rawData;
}

std::vector<double> getHertzValues(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                   std::vector<int> dims, int spectralIndex) {
    std::vector<double> hertzValues;

    if (spectralIndex >= 0) { // multiple frames
        std::vector<double> Xvalues;

        for (int i = 0; i < dims[spectralIndex]; i++) {
            Xvalues.push_back((double)i);
        }

        // convert frame indices to Hz
        auto result = Globals::instance() -> pluginManager() -> prepare<Carta::Lib::Hooks::ConversionSpectralHook>(m_image, "", "Hz", Xvalues );

        auto lam = [&hertzValues] (const Carta::Lib::Hooks::ConversionSpectralHook::ResultType &data) {
            hertzValues = data;
        };

        result.forEach(lam);

    } else {
        qWarning() << "Could not calculate Hertz values. This image has no spectral axis.";
    }

    return hertzValues;
}

void testMinMax(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage) {
    // set stoke index for getting the raw data (0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V)
    int stokeIndex = 0;

    // get raw data as the double type
    Carta::Lib::NdArray::RawViewInterface* rawData = getRawData(astroImage, -1, -1, stokeIndex);
    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view(rawData);
    Carta::Lib::NdArray::Double doubleView(view.get(), false);

    //get the index of spectral axis
    int spectralIndex = getAxisIndex(astroImage, AxisInfo::KnownType::SPECTRAL);

    // TODO: set a unit converter for tests
    // TODO: let the user specify this in parameters;then it becomes the user's responsibility to make it match the file
    // TODO: pass it in from outside
    // TODO: one for all files? one per file?
    //Carta::Lib::IntensityUnitConverter::SharedPtr converter(ConverterIntensity::converters(from_units, to_units, max_value, max_units, BEAM_AREA));
    Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr;

    // get Hertz values
    std::vector<double> hertzValues = {};
    if (converter && converter->frameDependent) {
        hertzValues = getHertzValues(astroImage, doubleView.dims(), spectralIndex);
    }

    // calculate Min and Max percentiles: new way
    Carta::Lib::IPercentilesToPixels<double>::SharedPtr minMaxCalc = std::make_shared<Carta::Core::Algorithms::MinMaxPercentiles<double> >();
    std::map<double, double> clips_MinMax1 = minMaxCalc->percentile2pixels(doubleView, {0, 1}, spectralIndex, converter, hertzValues);
    double intensity_min_new = clips_MinMax1[0];
    double intensity_max_new = clips_MinMax1[1];

    // calculate Min and Max percentiles: original way
    Carta::Lib::IPercentilesToPixels<double>::SharedPtr exactCalc = std::make_shared<Carta::Core::Algorithms::PercentilesToPixels<double> >();
    std::map<double, double> clips_MinMax2 = exactCalc->percentile2pixels(doubleView, {0, 1}, spectralIndex, converter, hertzValues);
    double intensity_min_original = clips_MinMax2[0];
    double intensity_max_original = clips_MinMax2[1];

    // check the difference of Min/Max intensity with new and original algorithms
    double delta_min = fabs(intensity_min_new-intensity_min_original);
    double delta_max = fabs(intensity_max_new-intensity_max_original);
    if (delta_min < 1.0e-9 && delta_max < 1.0e-9) {
        qCritical() << "[PASS] for Min/Max intensity =" << intensity_min_new << "/" << intensity_max_new;
    } else {
        qCritical() << "[FAIL !!] for Min/Max intensity =" << intensity_min_new << "/" << intensity_max_new << "(new way)"
                    << "!=" << intensity_min_original << "/" << intensity_max_original << "(original way)";
    }
}

void testPercentileHistogram(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage, Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator, int numberOfBins) {
    // set stoke index for getting the raw data (0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V)
    int stokeIndex = 0;

    // get raw data as the double type
    Carta::Lib::NdArray::RawViewInterface* rawData = getRawData(astroImage, -1, -1, stokeIndex);
    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view(rawData);
    Carta::Lib::NdArray::Double doubleView(view.get(), false);

    //get the index of spectral axis
    int spectralIndex = getAxisIndex(astroImage, AxisInfo::KnownType::SPECTRAL);

    // TODO: set a unit converter for tests
    // TODO: let the user specify this in parameters;then it becomes the user's responsibility to make it match the file
    // TODO: pass it in from outside
    // TODO: one for all files? one per file?
    //Carta::Lib::IntensityUnitConverter::SharedPtr converter(ConverterIntensity::converters(from_units, to_units, max_value, max_units, BEAM_AREA));
    Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr;

    // get Hertz values
    std::vector<double> hertzValues={};
    if (converter && converter->frameDependent) {
        hertzValues = getHertzValues(astroImage, doubleView.dims(), spectralIndex);
    }

    // calculate Min and Max percentiles: new way
    Carta::Lib::IPercentilesToPixels<double>::SharedPtr minMaxCalc = std::make_shared<Carta::Core::Algorithms::MinMaxPercentiles<double> >();
    std::map<double, double> clips_MinMax1 = minMaxCalc->percentile2pixels(doubleView, {0, 1}, spectralIndex, converter, hertzValues);

    // calculate the intensity range
    double intensity_range = fabs(clips_MinMax1[1] - clips_MinMax1[0]);

    // calculate precise percentiles to intensities
    Carta::Lib::IPercentilesToPixels<double>::SharedPtr exactCalc = std::make_shared<Carta::Core::Algorithms::PercentilesToPixels<double> >();
    std::map<double, double> clips_map1 = exactCalc->percentile2pixels(doubleView, percentile, spectralIndex, converter, hertzValues);

    calculator->setMinMax({clips_MinMax1[0], clips_MinMax1[1]});
    // override number of bins
    QJsonObject config;
    config["numberOfBins"] = numberOfBins;
    calculator->reconfigure(config);

    std::map<double, double> clips_map2 = calculator->percentile2pixels(doubleView, percentile, spectralIndex, converter, hertzValues);

    // expected intensity error
    double expected_error = 100/(double)numberOfBins; // represented as %

    // check the difference of percentile to intensity with new and original algorithms
    int percentile_number = percentile.size();
    qCritical() << "HISTOGRAM APPROXIMATION";
    qCritical() << "--------------------------------------------------------------------------------";
    qCritical() << "For bin numbers:" << numberOfBins;
    for (int i = 0; i < percentile_number; i++) {
        double intensity_precise = clips_map1[percentile[i]];
        double intensity_approximation = clips_map2[percentile[i]];
        double error = 100*fabs(intensity_approximation - intensity_precise)/intensity_range; // represented as %
        if (error <= expected_error) {
            qCritical() << "[PASS] for percentile" << 100*percentile[i] << "(%), intensity error =" << error << "<=" << expected_error << "(%)";
        } else {
            qCritical() << "[FAIL !!] for percentile" << 100*percentile[i] << "(%), intensity error =" << error << ">" << expected_error << "(%)";
        }
    }

}

std::vector<double> extractRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage) {
    // set stoke index for getting the raw data (0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V)
    int stokeIndex = 0;

    // get raw data as the double type
    Carta::Lib::NdArray::RawViewInterface* rawData = getRawData(astroImage, 0, 1, stokeIndex);
    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view(rawData);
    Carta::Lib::NdArray::Double doubleView(view.get(), false);

    //get the index of spectral axis
    int spectralIndex = getAxisIndex(astroImage, AxisInfo::KnownType::SPECTRAL);

    // TODO: set a unit converter for tests
    // TODO: let the user specify this in parameters;then it becomes the user's responsibility to make it match the file
    // TODO: pass it in from outside
    // TODO: one for all files? one per file?
    // unit converter
    Carta::Lib::IntensityUnitConverter::SharedPtr converter = nullptr;

    // get Hertz values
    std::vector<double> hertzValues={};
    if (converter && converter->frameDependent) {
        hertzValues = getHertzValues(astroImage, doubleView.dims(), spectralIndex);
    }

    // read in all values from the view into memory so that we can do quickselect on it
    std::vector<double> allValues;
    double hertzVal;

    if (converter && converter->frameDependent) {
        // we need to apply the frame-dependent conversion to each intensity value before copying it
        // to avoid calculating the frame index repeatedly we use slices to iterate over the image one frame at a time
        for (size_t f = 0; f < hertzValues.size(); f++) {
            hertzVal = hertzValues[f];

            Carta::Lib::NdArray::Double viewSlice = Carta::Lib::viewSliceForFrame(doubleView, spectralIndex, f);

            // iterate over the frame
            viewSlice.forEach([&allValues, &converter, &hertzVal](const double & val) {
                if (std::isfinite(val)) {
                    allValues.push_back(converter->_frameDependentConvert(val, hertzVal));
                } else {
                    allValues.push_back(-1);
                }
            });
        }
    } else {
        // we don't have to do any conversions in the loop
        // and we can loop over the flat image
        doubleView.forEach([& allValues] (const double & val) {
            if (std::isfinite(val)) {
                allValues.push_back(val);
            } else {
                allValues.push_back(-1);
            }
        });
    }

    cout << "raw data size: " << allValues.size() << endl;
    //for (int i = 0; i < allValues.size(); i++) {
    //    cout << allValues[i] << " ";
    //}
    //cout << endl;

    return allValues;

} // end of extractRawData()

void testPercentileManku99(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage, Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator, int numBuffers, int bufferCapacity, int sampleAfter) {
    // set stoke index for getting the raw data (0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V)
    int stokeIndex = 0;

    // get raw data as the double type
    Carta::Lib::NdArray::RawViewInterface* rawData = getRawData(astroImage, -1, -1, stokeIndex);
    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view(rawData);
    Carta::Lib::NdArray::Double doubleView(view.get(), false);

    //get the index of spectral axis
    int spectralIndex = getAxisIndex(astroImage, AxisInfo::KnownType::SPECTRAL);

    // TODO: set a unit converter for tests
    // TODO: let the user specify this in parameters;then it becomes the user's responsibility to make it match the file
    // TODO: pass it in from outside
    // TODO: one for all files? one per file?
    //Carta::Lib::IntensityUnitConverter::SharedPtr converter(ConverterIntensity::converters(from_units, to_units, max_value, max_units, BEAM_AREA));
    Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr;

    // get Hertz values
    std::vector<double> hertzValues={};
    if (converter && converter->frameDependent) {
        hertzValues = getHertzValues(astroImage, doubleView.dims(), spectralIndex);
    }

    // calculate Min and Max percentiles: new way
    Carta::Lib::IPercentilesToPixels<double>::SharedPtr minMaxCalc = std::make_shared<Carta::Core::Algorithms::MinMaxPercentiles<double> >();
    std::map<double, double> clips_MinMax1 = minMaxCalc->percentile2pixels(doubleView, {0, 1}, spectralIndex, converter, hertzValues);

    // calculate the intensity range
    double intensity_range = fabs(clips_MinMax1[1] - clips_MinMax1[0]);

    // calculate precise percentiles to intensities
    Carta::Lib::IPercentilesToPixels<double>::SharedPtr exactCalc = std::make_shared<Carta::Core::Algorithms::PercentilesToPixels<double> >();
    std::map<double, double> clips_map1 = exactCalc->percentile2pixels(doubleView, percentile, spectralIndex, converter, hertzValues);

    // override parameters
    QJsonObject config;
    config["numBuffers"] = numBuffers;
    config["bufferCapacity"] = bufferCapacity;
    config["sampleAfter"] = sampleAfter;
    calculator->reconfigure(config);

    std::map<double, double> clips_map2 = calculator->percentile2pixels(doubleView, percentile, spectralIndex, converter, hertzValues);

    // expected intensity error
//     double expected_error = ???; // represented as %

    // check the difference of percentile to intensity with new and original algorithms
    qCritical() << "MANKU 99 APPROXIMATION";
    qCritical() << "--------------------------------------------------------------------------------";
    qCritical() << "Parameters: buffers" << numBuffers << "buffer capacity" << bufferCapacity << "sample after" << sampleAfter;

    for (auto& p : percentile) {
        double intensity_precise = clips_map1[p];
        double intensity_approximation = clips_map2[p];
        double error = 100*fabs(intensity_approximation - intensity_precise)/intensity_range; // represented as %
        qCritical() << "For percentile" << 100*p << "(%), intensity error =" << error << "(%)";
    }

}
