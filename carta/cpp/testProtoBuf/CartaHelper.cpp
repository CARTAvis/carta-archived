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

const float NAN_VALUE = -9999;
const bool CHECK_DOWNSAMPLING_RESULT = false;

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

bool downVector(std::vector<float> &rawData, int x_size, int y_size, int mip) {
    // check if the setting value of x- or y- dim matches the raw data size and fits the down sampling requirement
    if (rawData.size() != x_size * y_size || mip > x_size || mip > y_size || mip < 1 || x_size <= 0 || y_size <= 0) {
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
                float denominator = 1.0 * mip;
                // check if the retain index of the raw data is NAN
                if (rawData[retain_index] < NAN_VALUE + 1) {
                    rawData[retain_index] = 0;
                    denominator -= 1;
                }
                // calculate the average of the pixel value in the "mip" range
                for (int k = 1; k < mip; k++) {
                    // check if the range retain index of the raw data is NAN
                    if (rawData[retain_index + k] < NAN_VALUE + 1) {
                        denominator -= 1;
                    } else {
                        rawData[retain_index] += rawData[retain_index + k];
                    }
                }
                rawData[retain_index] = (denominator < 1 ? NAN_VALUE : rawData[retain_index] / denominator);
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
                float denominator = 1.0 * mip;
                // check if the retain index of the raw data is NAN
                if (rawData[retain_index] < NAN_VALUE + 1) {
                    rawData[retain_index] = 0;
                    denominator -= 1;
                }
                for (int k = 1; k < mip; k++) {
                    // check if the range retain index of the raw data is NAN
                    if (rawData[retain_index + k * reduced_x_size] < NAN_VALUE + 1) {
                        denominator -= 1;
                    } else {
                        rawData[retain_index] += rawData[retain_index + k * reduced_x_size];
                    }
                }
                rawData[retain_index] = (denominator < 1 ? NAN_VALUE : rawData[retain_index] / denominator);
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

std::vector<float> downSampling(Carta::Lib::NdArray::RawViewInterface *view, int ilb, int iub, int jlb, int jub, int mip) {
    std::vector<float> allValues;
    int nRows = (jub - jlb + 1) / mip;
    int nCols = (iub - ilb + 1) / mip;

    if (nCols > view -> dims()[0] || nRows > view -> dims()[1] || nCols < 0 || nRows < 0 ||
        mip < 1 || iub < 0 || ilb < 0 || jub < 0 || jlb < 0) {
        qCritical() << "The input values of ilb, iub, jlb, jub or mip may be not correct!";
        return allValues;
    }

    int prepareCols = iub - ilb + 1;
    int prepareRows = mip;
    int area = prepareCols * prepareRows;
    std::vector<float> rawData(nCols), prepareArea(area);
    int nextRowToReadIn = jlb;

    auto updateRows = [&]() -> void {
        CARTA_ASSERT( nextRowToReadIn < view -> dims()[1] );

        SliceND rowSlice;
        int update = prepareRows;
        rowSlice.next().start( nextRowToReadIn ).end( nextRowToReadIn + update );
        auto rawRowView = view -> getView( rowSlice );

        // make a float view of this raw row view
        Carta::Lib::NdArray::Float fview( rawRowView, true );

        int t = 0;
        fview.forEach( [&] ( const float & val ) {
            // To improve the performance, the prepareArea also update only one row
            // by computing the module
            prepareArea[(t++) % area] = val;
        });

        // Calculate the mean of each block (mip X mip)
        for (int i = ilb; i < nCols; i++) {
            rawData[i] = 0;
            int elems = mip * mip;
            float denominator = 1.0 * elems;
            for (int e = 0; e < elems; e++) {
                int row = e / mip;
                int col = e % mip;
                int index = (row * prepareCols + col + i * mip) % area;
                //
                if (std::isfinite(prepareArea[index])) {
                    rawData[i] += prepareArea[index];
                } else {
                    denominator -= 1;
                }
            }
            rawData[i] = (denominator < 1 ? NAN_VALUE : rawData[i] / denominator);
            allValues.push_back(rawData[i]);
        }
        nextRowToReadIn += update;
    };

    for (int j = jlb; j < nRows; j++) {
        updateRows();
    }

    return allValues;
}

std::vector<float> fullSampling(Carta::Lib::NdArray::RawViewInterface *view) {
    Carta::Lib::NdArray::Float floatView(view, false);
    std::vector<float> allValues;

    // get all pixel elements
    floatView.forEach([& allValues] (const float & val) {
        if (std::isfinite(val)) {
            allValues.push_back(val);
        } else {
            allValues.push_back(NAN_VALUE);
        }
    });
    return allValues;
}

void compareVectors(std::vector<float> vec1, std::vector<float> vec2) {
    if (vec1.size() != vec2.size()) {
        qCritical() << "!! Two vector sizes are not the same: vec1.size()=" << vec1.size() << ", vec2.size()=" << vec2.size();
    } else {
        bool isConsistent = true;
        for (int i = 0; i < vec1.size(); i++) {
            if (fabs(vec1[i] - vec2[i]) > 1e-3) {
                qCritical() << "!! Two vector elements are not consistent:"
                            << "vec1["<< i << "]=" << vec1[i]
                            << ", vec2[" << i << "]=" << vec2[i]
                            << ", diff=" << ((vec1[i] - vec2[i]) / vec1[i]) * 100.0 << "%";
                isConsistent = false;
            }
        }
        if (isConsistent) {
            qDebug() << "Two vectors are the same.";
        }
    }
}

std::vector<float> extractRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage,
                                  int frameStart, int frameEnd, int stokeIndex,
                                  int ilb, int iub, int jlb, int jub, int mip) {
    // get raw data as the float type
    Carta::Lib::NdArray::RawViewInterface* rawData = getRawData(astroImage, frameStart, frameEnd, stokeIndex);
    int x_size = astroImage->dims()[0]; // the size of X-axis
    int y_size = astroImage->dims()[1]; // the size of Y-axis

    std::vector<float> resultValues;

    if (ilb == 0 && iub == x_size - 1 && jlb == 0 && jub == y_size - 1 && mip == 1) {
        resultValues = fullSampling(rawData);
    } else {
        resultValues = downSampling(rawData, ilb, iub, jlb, jub, mip);
    }

    // check if downsampling results are correct (only for selecting the full range of the raw data)
    if (CHECK_DOWNSAMPLING_RESULT) {
        std::vector<float> testValues = fullSampling(rawData);

        // downsample the hole pixel elements by the "mip" factor
        bool isDownSampling = downVector(testValues, x_size, y_size, mip);

        if (isDownSampling) {
            qDebug() << "++++++++ down sampling is success!";
        } else {
            qDebug() << "++++++++ down sampling is failed! Return the original raw data.";
        }

        // check if two vectors are the same
        compareVectors(testValues, resultValues);
    }

    return resultValues;

} // end of extractRawData()

