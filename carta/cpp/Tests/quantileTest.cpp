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
    virtual RawViewInterface *
    getView(const SliceND & sliceInfo) override {
        // this is hardcoded for this test case only
        // it implements exactly one kind of slice, which is what is needed by the quantile algorithm
        // i.e. one frame, and we assume that the image is 3D and the 3rd dimension is the spectral axis
        // this is also not particularly efficient and should not be used for large test data
        SliceND::ApplyResult ar = sliceInfo.apply( dims);
        size_t frame = ar.dims()[2].start;
        
        std::vector<double> frame_data;
        // fill; return new
    }
};

TEST_CASE( "Quantile algorithm test", "[quantile]" ) {
    
    std::uniform_real_distribution<double> dist(0, 100);
    std::default_random_engine gen;

    std::vector<int> dims = {200, 200, 10};
    int size = std::accumulate (begin(dims), end(dims), 1, [](int a, int& b){ return b*a; });
    std::vector<double> data(size);
    
    // TODO: add a seed to make this deterministic
    std::generate(data.begin(), data.end(), [&]() { return dist(gen); });
    
    Carta::Lib::NdArray::RawViewInterface * rawView = new TestRawView<double>(data, dims);
    Carta::Lib::NdArray::Double doubleView(rawView, false);
        
    const std::vector<double> percentiles = {0, 0.01, 0.1, 0.9, 0.99, 1};
        
    SECTION( "Exact algorithm") {
        std::map <double, double> intensities = Carta::Core::Algorithms::percentile2pixels(doubleView, percentiles);
        REQUIRE(intensities.size() == percentiles.size());
        for (auto& kv : intensities) {
            qDebug() << kv.first << ":" << kv.second;
        }
    }

    SECTION( "Manku 99 algorithm") {
        std::map <double, double> intensities =  Carta::Core::Algorithms::percentile2pixels_approximate_manku99(doubleView, percentiles);
        REQUIRE(intensities.size() == percentiles.size());
        for (auto& kv : intensities) {
            qDebug() << kv.first << ":" << kv.second;
        }
    }
    
    //SECTION("testing frame slice implementation") {
        //SliceND frame;
        //int spectralIndex = 2;
        //int frameIndex = 5;
        
        //for (size_t d = 0; d < dims.size(); d++) {
            //if ((int)d == spectralIndex) {
                //frame.index(frameIndex);
            //} else {
                //frame.next();
            //}
        //}
        
        //qDebug() << frame.slice(0).start();
        //qDebug() << frame.slice(1).start();
        //qDebug() << frame.slice(2).start();
        
        //SliceND::ApplyResult ar = frame.apply( dims);
        
        //qDebug() << ar.toStr();
        //qDebug() << ar.dims()[0].toStr();
        //qDebug() << ar.dims()[1].toStr();
        //qDebug() << ar.dims()[2].toStr();
        //qDebug() << ar.dims()[2].start;
    //}

    delete rawView;
}
