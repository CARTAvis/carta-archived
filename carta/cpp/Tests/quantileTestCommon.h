/**
 * Common stubs and data for testing quantile algorithms
 * To be used in exact algorithm unit tests and plugin unit tests
 **/

#pragma once

#include "CartaLib/IImage.h"
#include "CartaLib/Slice.h"
#include "CartaLib/IntensityUnitConverter.h"
#include <random>

/// Implementing the bare minimum; excluding slices for now

template < typename PType >
class TestRawView
    : public Carta::Lib::NdArray::RawViewInterface
{
public:
    TestRawView(const std::vector<PType> data, const VI dims) {
        this->data = std::move(data);
        m_viewDims = dims;
        m_pixelType = Carta::Lib::Image::CType2PixelType < PType >::type;
    }

    virtual PixelType
    pixelType() override {
        return m_pixelType;
    }

    virtual const VI &
    dims() override {
        return m_viewDims;
    }

     // we don't need this
    virtual const char *
    get( const VI & pos ) override {
        Q_UNUSED(pos);
        qFatal( "not implemented");
    }

    virtual void
    forEach( std::function < void (const char *) > func, Traversal traversal ) override {
        if ( traversal != Carta::Lib::NdArray::RawViewInterface::Traversal::Sequential ) {
            qFatal( "sorry, not implemented yet" );
        }
        
        for ( const auto & val : data ) {
            func( reinterpret_cast < const char * > ( & val ) );
        }
    }

    virtual const VI &
    currentPos() override {
        qFatal( "not implemented");
    }
    
    // we'll need to implement this to test the algorithms with unit conversion
    virtual RawViewInterface *
    getView(const SliceND & sliceInfo) override {
        Q_UNUSED(sliceInfo);
        qFatal( "not implemented");
    }

    virtual int64_t
    read( int64_t buffSize, char * buff,
          Traversal traversal = Traversal::Sequential ) override {
        Q_UNUSED( buffSize);
        Q_UNUSED( buff);
        Q_UNUSED( traversal);
        qFatal( "not implemented");
    }

    virtual void
    seek(int64_t ind) override
    {
        Q_UNUSED( ind);
        qFatal( "not implemented");
    }

    virtual int64_t
    read( int64_t chunk, int64_t buffSize, char * buff,
          Traversal traversal = Traversal::Sequential ) override
    {
        Q_UNUSED( chunk );
        Q_UNUSED( buffSize );
        Q_UNUSED( buff );
        Q_UNUSED( traversal );
        qFatal( "not implemented" );
    }

    virtual void
    forEach(
        int64_t buffSize,
        std::function < void (const char *, int64_t count) > func,
        char * buff = nullptr,
        Traversal traversal = Traversal::Sequential ) override
    {
        Q_UNUSED( buffSize );
        Q_UNUSED( func );
        Q_UNUSED( buff );
        Q_UNUSED( traversal );
        qFatal( "not implemented" );
    }

    std::vector<PType> data;
    VI m_viewDims;
    Carta::Lib::Image::PixelType m_pixelType;
};

/** Dummy view with basic slice support */

class TestRawViewSliceStub
    : public TestRawView<double>
{
public:
    TestRawViewSliceStub(const std::vector<double> data, const std::vector<int> dims) : TestRawView(data, dims) {
    }

    virtual RawViewInterface *
    getView(const SliceND & sliceInfo) override {
        // this is hardcoded for this test case only
        // it implements exactly one kind of slice, which is what is needed by the quantile algorithm
        // i.e. one frame, and we assume that the image is 3D and the 3rd dimension is the spectral axis
        // this is also not particularly efficient and should not be used for large test data
        SliceND::ApplyResult ar = sliceInfo.apply(m_viewDims);
        
        size_t frame = ar.dims()[2].start;
        size_t frameSize = m_viewDims[0] * m_viewDims[1];
        std::vector<int> frameDims = {m_viewDims[0], m_viewDims[1]};
        
        size_t start = frameSize * frame;
        size_t end = frameSize * (frame + 1);
        
        std::vector<double> frameData;
        
        for (size_t i = start; i < end; i++) {
            frameData.push_back(data[i]);
        }
        
        return new TestRawViewSliceStub(frameData, frameDims);
    }
};

/** Dummy converters */

class FrameDependentConverter : public Carta::Lib::IntensityUnitConverter {
public:
    FrameDependentConverter() 
        : Carta::Lib::IntensityUnitConverter("DUMMY_FROM", "DUMMY_TO", 2, true, "DUMMY_DEPENDENT_CONVERSION") {
    }

    double _frameDependentConvert(const double y_val, const double x_val) override {
        return y_val / pow(x_val, 2);
    }

    double _frameDependentConvertInverse(const double y_val, const double x_val) override {
        return y_val * pow(x_val, 2);
    }
};

class FrameIndependentConverter : public Carta::Lib::IntensityUnitConverter {
public:
    FrameIndependentConverter() 
        : Carta::Lib::IntensityUnitConverter("DUMMY_FROM", "DUMMY_TO", 2, false, "NONE") {
    }
};

class QuantileTestData {
public:
    QuantileTestData(
        std::string name,
        Carta::Lib::NdArray::Double view,
        std::vector<double> percentiles,
        int spectralIndex,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter,
        std::vector<double> hzValues,
        std::vector<double> minMax,
        std::map <double, double> expected
    ) :  name(name), view(view), percentiles(percentiles), spectralIndex(spectralIndex), converter(converter), hzValues(hzValues), minMax(minMax), expected(expected) {
    }

    std::string name;
    Carta::Lib::NdArray::Double view;
    std::vector<double> percentiles;
    int spectralIndex;
    Carta::Lib::IntensityUnitConverter::SharedPtr converter;
    std::vector<double> hzValues;
    std::vector<double> minMax;
    std::map <double, double> expected;
};

std::vector<QuantileTestData> commonTestCases() {
    std::vector<int> dims = {200, 200, 10};
    int size = std::accumulate (begin(dims), end(dims), 1, [](int a, int& b){ return b*a; });
    std::vector<double> data(size);
    
    std::iota(data.begin(), data.end(), 1); // we start at 1 because we can't calculate a percentile error if the expected value is 0
    // shuffle randomly because ordered data breaks the Manku algorithm
    // but don't seed the generator, to make the shuffle deterministic for the frame-dependent conversion test
    std::shuffle(data.begin(), data.end(), std::mt19937{});
    
    Carta::Lib::NdArray::RawViewInterface * rawView = new TestRawViewSliceStub(data, dims);
    Carta::Lib::NdArray::Double doubleView(rawView, false);
        
    const std::vector<double> percentiles = {0, 0.01, 0.1, 0.9, 0.99, 1};

    std::map <double, double> exactSolution = {
        {0, 1},
        {0.01, 4000},
        {0.1, 40000},
        {0.9, 360000},
        {0.99, 396000},
        {1, 400000}
    };

    std::map <double, double> exactSolutionFrameDependent = {
        {0, 0.015625},
        {0.01, 103.3},
        {0.1, 1035.1358024691},
        {0.9, 80052},
        {0.99, 360171},
        {1, 399994}
    };
        
    std::vector<double> dummyHzValues(10);
    std::iota(dummyHzValues.begin(), dummyHzValues.end(), 1); // start with 1 because we divide by these in the dummy converter
    int spectralIndex = 2;
    
    std::vector<QuantileTestData> cases;
    
    cases.push_back(QuantileTestData("no conversion", doubleView, percentiles, -1, nullptr, {}, {1, 200*200*10}, exactSolution));
    cases.push_back(QuantileTestData("frame-dependent conversion", doubleView, percentiles, spectralIndex, std::make_shared<FrameDependentConverter>(), dummyHzValues, {1, 200*200*10}, exactSolutionFrameDependent));
    cases.push_back(QuantileTestData("frame-independent conversion", doubleView, percentiles, spectralIndex, std::make_shared<FrameIndependentConverter>(), dummyHzValues, {1, 200*200*10}, exactSolution));
    
    return cases;
}
