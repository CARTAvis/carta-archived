#include "core/Globals.h"

// we want to check percentile functions from this code
#include "core/Algorithms/percentileAlgorithms.h"

// get unit conversion headers
#include "CartaLib/Hooks/ConversionSpectralHook.h"
#include "CartaLib/Hooks/ConversionIntensityHook.h"

#include <QDebug>
#include <cmath>

using namespace std;

typedef Carta::Lib::AxisInfo AxisInfo;

const double NAN_VALUE = -9999;

int getAxisIndex(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image, AxisInfo::KnownType axisType) {
    int index = -1;
    if (m_image){
        std::shared_ptr<CoordinateFormatterInterface> cf(m_image->metaData()->coordinateFormatter()->clone());
        int axisCount = cf->nAxes();
        for ( int i = 0; i < axisCount; i++ ){
            AxisInfo axisInfo = cf->axisInfo(i);
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
    //qDebug() << "++++++++ X Index:" << m_axisIndexX;
    //qDebug() << "++++++++ Y Index:" << m_axisIndexY;
    //qDebug() << "++++++++ Stoke Index:" << stokeIndex;
    //qDebug() << "++++++++ Spectral Index:" << spectralIndex;

    if (m_axisIndexX != 0 || m_axisIndexY != 1) {
        qFatal("Can not find x or y axises");
    }

    if (m_image) {
        int imageDim = m_image->dims().size();
        //qDebug() << "++++++++ the dimension of image:" << imageDim;

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
                        //qDebug() << "++++++++ we only consider the stoke" << stokeSliceIndex <<
                        //            "(-1: no stoke, 0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V) for percentile calculation" ;
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
        auto result = Globals::instance()->pluginManager()->prepare<Carta::Lib::Hooks::ConversionSpectralHook>(m_image, "", "Hz", Xvalues );

        auto lam = [&hertzValues] (const Carta::Lib::Hooks::ConversionSpectralHook::ResultType &data) {
            hertzValues = data;
        };

        result.forEach(lam);

    } else {
        qWarning() << "Could not calculate Hertz values. This image has no spectral axis.";
    }

    return hertzValues;
}

bool downSampling(std::vector<double> &rawData, int x_size, int y_size, int mip) {
    // check if the setting value of x- or y- dim matches the raw data size and fits the down sampling requirement
    if (rawData.size() != x_size * y_size || mip > x_size || mip > y_size || x_size <= 0 || y_size <= 0) {
        qCritical() << "The input value of x- or y- dim does not match the raw data size or down sampling requirement!";
        return false;
    }
    if (mip < 1) {
        qCritical() << "The mip value should be greater than or equal to 1!";
        return false;
    } else if (mip == 1) {
        qDebug() << "The mip value is equal to one, return the original raw data.";
        return true;
    } else {
        // do the down sampling procedure
        int reduced_x_size = x_size / mip;
        int reduced_y_size = y_size / mip;
        int residual_x = x_size % mip;
        int residual_y = y_size % mip;
        // collapse the column size first
        for (int i = y_size - 1; i >= 0; i--) { // start from the last row
            for (int j = reduced_x_size - 1; j >= 0; j--) { // start from the last column
                // calculate the reatin index
                int retain_index = x_size * i + mip * j;
                // calculate the average of the pixel value in the "mip" range
                int denominator = mip;
                for (int k = 1; k < mip; k++) {
                    if (rawData[retain_index] < NAN_VALUE + 1) {
                        denominator -= 1;
                    } else {
                        rawData[retain_index] += rawData[retain_index + k];
                    }
                }
                if (denominator == 0) {
                    rawData[retain_index] = NAN_VALUE;
                } else {
                    rawData[retain_index] /= denominator;
                }
                // erase last elements in the mip range except the ratain element
                int remove_index_begin = retain_index + 1;
                int remove_index_end = remove_index_begin + mip - 1;
                // erase residual columns on the right hand side
                if (j == reduced_x_size - 1) {
                    remove_index_end += residual_x;
                }
                rawData.erase(rawData.begin() + remove_index_begin, rawData.begin() + remove_index_end);
            }
        }
        // then collapse the row size
        for (int i = reduced_y_size - 1; i >= 0; i--) { // start from the last row
            for (int j = reduced_x_size - 1; j >= 0; j--) { // start from the last column
                // calculate the reatin index
                int retain_index = mip * i * reduced_x_size + j;
                // calculate the average of the pixel value in the "mip" range
                int denominator = mip;
                for (int k = 1; k < mip; k++) {
                    if (rawData[retain_index] < NAN_VALUE + 1) {
                        denominator -= 1;
                    } else {
                        rawData[retain_index] += rawData[retain_index + k * reduced_x_size];
                    }
                }
                if (denominator == 0) {
                    rawData[retain_index] = NAN_VALUE;
                } else {
                    rawData[retain_index] /= denominator;
                }
            }
            // erase last elements in the mip range except the ratain element
            int remove_index_begin = (mip * i + 1) * reduced_x_size;
            int remove_index_end = remove_index_begin + (mip - 1) * reduced_x_size;
            // erase residual rows on the bottom
            if (i == reduced_y_size - 1) {
                remove_index_end += residual_y * reduced_x_size;
            }
            rawData.erase(rawData.begin() + remove_index_begin, rawData.begin() + remove_index_end);
        }
        return true;
    }
}

std::vector<double> extractRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage,
                                   int frameStart, int frameEnd, int stokeIndex, int mip) {
    // get raw data as the double type
    Carta::Lib::NdArray::RawViewInterface* rawData = getRawData(astroImage, frameStart, frameEnd, stokeIndex);
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
                    allValues.push_back(NAN_VALUE);
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
                allValues.push_back(NAN_VALUE);
            }
        });
    }

    //qDebug() << "++++++++ the size of X-axis:" << astroImage->dims()[0];
    //qDebug() << "++++++++ the size of Y-axis:" << astroImage->dims()[1];
    int x_size = astroImage->dims()[0];
    int y_size = astroImage->dims()[1];

    bool isDownSampling = downSampling(allValues, x_size, y_size, mip);
    if (isDownSampling) {
        qDebug() << "++++++++ down sampling is success!";
    } else {
        qDebug() << "++++++++ down sampling is failed! Return the original raw data.";
    }

    //cout << "raw data size: " << allValues.size() << endl;
    //for (int i = 0; i < allValues.size(); i++) {
    //    cout << allValues[i] << " ";
    //}
    //cout << endl;

    return allValues;

} // end of extractRawData()
