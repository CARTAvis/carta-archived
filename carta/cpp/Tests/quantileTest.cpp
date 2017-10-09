#include "catch.h"
#include "TestRawView.h"
#include "core/Algorithms/percentileAlgorithms.h"
#include "core/Algorithms/percentileManku99.h"
#include <random>

#include "CartaLib/Slice.h"

class TestRawViewSliceStub
    : public TestRawView<double>
{
public:
    TestRawViewSliceStub(std::vector<double> data, std::vector<int> dims) : TestRawView(data, dims) {
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

TEST_CASE( "Quantile algorithm test", "[quantile]" ) {
    
    //std::uniform_real_distribution<double> dist(0, 100);
    //std::default_random_engine gen;

    std::vector<int> dims = {200, 200, 10};
    int size = std::accumulate (begin(dims), end(dims), 1, [](int a, int& b){ return b*a; });
    std::vector<double> data(size);
    
    std::iota(data.begin(), data.end(), 1); // we start at 1 because we can't calculate a percentile error if the expected value is 0
    std::shuffle(data.begin(), data.end(), std::mt19937{std::random_device{}()});
    
    // TODO: add a seed to make this deterministic
    // TODO: or replace by randomly shuffled known values
    //std::generate(data.begin(), data.end(), [&]() { return dist(gen); });
    
    Carta::Lib::NdArray::RawViewInterface * rawView = new TestRawView<double>(data, dims);
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
        
    SECTION( "Exact algorithm, no conversion") {
        std::map <double, double> intensities = Carta::Core::Algorithms::percentile2pixels(doubleView, percentiles);
        REQUIRE(intensities.size() == percentiles.size());
        REQUIRE(intensities == exactSolution);
        for (auto& kv : intensities) {
            qDebug() << kv.first << ":" << kv.second;
        }
    }

    SECTION( "Manku 99 algorithm, no conversion") {
        std::map <double, double> intensities =  Carta::Core::Algorithms::percentile2pixels_approximate_manku99(doubleView, percentiles);
        REQUIRE(intensities.size() == percentiles.size());
        for (auto& p : percentiles) {
            int error = abs(exactSolution[p] - intensities[p]);
            double percentageError = 100 * error / size;
            qDebug() << error << percentageError;
        }
    }
    
    SECTION("TestRawViewSliceStub implementation") {
        std::vector<double> smallData(2 * 3 * 4);
        std::iota(smallData.begin(), smallData.end(), 0);
        std::vector<int> smallDims = {2, 3, 4};
        
        std::vector<double> expectedFrameData(2 * 3);
        std::iota(expectedFrameData.begin(), expectedFrameData.end(), 12);
        
        SliceND frameSlice;        
        frameSlice.next().next().index(2);
        
        Carta::Lib::NdArray::RawViewInterface * smallView = new TestRawViewSliceStub(smallData, smallDims);
        Carta::Lib::NdArray::RawViewInterface * smallFrame = smallView->getView(frameSlice);
                
        REQUIRE( ((TestRawViewSliceStub *) smallFrame)->data == expectedFrameData );
        
        delete smallView;
    }

    delete rawView;
}
