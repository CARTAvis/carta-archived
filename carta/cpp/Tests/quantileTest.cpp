#include "catch.h"
#include "quantileTestCommon.h"
#include "core/Algorithms/percentileAlgorithms.h"
// #include "core/Algorithms/percentileManku99.h"
#include <random>

TEST_CASE( "Quantile algorithm test", "[quantile]" ) {

//     std::vector<int> dims = {200, 200, 10};
//     int size = std::accumulate (begin(dims), end(dims), 1, [](int a, int& b){ return b*a; });
//     std::vector<double> data(size);
//     
//     std::iota(data.begin(), data.end(), 1); // we start at 1 because we can't calculate a percentile error if the expected value is 0
//     // shuffle randomly because ordered data breaks the Manku algorithm
//     // but don't seed the generator, to make the shuffle deterministic for the frame-dependent conversion test
//     std::shuffle(data.begin(), data.end(), std::mt19937{});
//     
//     Carta::Lib::NdArray::RawViewInterface * rawView = new TestRawViewSliceStub(data, dims);
//     Carta::Lib::NdArray::Double doubleView(rawView, false);
//         
//     const std::vector<double> percentiles = {0, 0.01, 0.1, 0.9, 0.99, 1};
// 
//     std::map <double, double> exactSolution = {
//         {0, 1},
//         {0.01, 4000},
//         {0.1, 40000},
//         {0.9, 360000},
//         {0.99, 396000},
//         {1, 400000}
//     };
// 
//     std::map <double, double> exactSolutionFrameDependent = {
//         {0, 0.015625},
//         {0.01, 103.3},
//         {0.1, 1035.14},
//         {0.9, 80052},
//         {0.99, 360171},
//         {1, 399994}
//     };
//         
//     std::vector<double> dummyHzValues(10);
//     std::iota(dummyHzValues.begin(), dummyHzValues.end(), 1); // start with 1 because we divide by these in the dummy converter
//     int spectralIndex = 2;

    SECTION( "Exact algorithm, no conversion") {
        std::map <double, double> intensities = Carta::Core::Algorithms::percentile2pixels(doubleView, percentiles);
        REQUIRE(intensities.size() == percentiles.size());
        REQUIRE(intensities == exactSolution);
    }
        
    SECTION( "Exact algorithm, frame-dependent conversion") {
        Carta::Lib::IntensityUnitConverter::SharedPtr converter = std::make_shared<FrameDependentConverter>();
        
        std::map <double, double> intensities = Carta::Core::Algorithms::percentile2pixels(doubleView, percentiles, spectralIndex, converter, dummyHzValues);
        REQUIRE(intensities.size() == percentiles.size());
        for (auto& p : percentiles) {
            REQUIRE(fabs(exactSolutionFrameDependent[p] - intensities[p]) < 1e-10);
        }
    }
        
    SECTION( "Exact algorithm, frame-independent conversion") {
        Carta::Lib::IntensityUnitConverter::SharedPtr converter = std::make_shared<FrameIndependentConverter>();

        std::map <double, double> intensities = Carta::Core::Algorithms::percentile2pixels(doubleView, percentiles, spectralIndex, converter, dummyHzValues);
        REQUIRE(intensities.size() == percentiles.size());
        // This should remain unchanged; the constant multiplier is not applied within the quantile function
        REQUIRE(intensities == exactSolution);
    }
/*
    SECTION( "Manku 99 algorithm, no conversion") {
        std::map <double, double> intensities =  Carta::Core::Algorithms::percentile2pixels_approximate_manku99(doubleView, percentiles);
        REQUIRE(intensities.size() == percentiles.size());
        for (auto& p : percentiles) {
            int error = fabs(exactSolution[p] - intensities[p]);
            double fractionalError = (double)error / size;
            //qDebug() << error << fractionalError;
            REQUIRE(fractionalError < 0.03); // 3%; overly generous, but covers expected spikes
        }
    }

    // This is terrible. Why? Ordered data?
    // Probably; the algorithm is weighted towards earlier data.
    // How to fix this in the test?
    // Add a scaling factor to the data after scrambling.
    SECTION( "Manku 99 algorithm, frame-dependent conversion") {
        Carta::Lib::IntensityUnitConverter::SharedPtr converter = std::make_shared<FrameDependentConverter>();
        
        std::map <double, double> intensities =  Carta::Core::Algorithms::percentile2pixels_approximate_manku99(doubleView, percentiles, spectralIndex, converter, dummyHzValues);
        REQUIRE(intensities.size() == percentiles.size());
        for (auto& p : percentiles) {
            int error = fabs(exactSolutionFrameDependent[p] - intensities[p]);
            double fractionalError = (double)error / size;
            qDebug() << exactSolutionFrameDependent[p] << intensities[p] << error << fractionalError;
            //REQUIRE(fractionalError < 0.03); // 3%; overly generous, but covers expected spikes
        }
    }

    SECTION( "Manku 99 algorithm, frame-independent conversion") {
        Carta::Lib::IntensityUnitConverter::SharedPtr converter = std::make_shared<FrameIndependentConverter>();
        
        std::map <double, double> intensities =  Carta::Core::Algorithms::percentile2pixels_approximate_manku99(doubleView, percentiles, spectralIndex, converter, dummyHzValues);
        REQUIRE(intensities.size() == percentiles.size());
        for (auto& p : percentiles) {
            int error = fabs(exactSolution[p] - intensities[p]);
            double fractionalError = (double)error / size;
            //qDebug() << error << fractionalError;
            REQUIRE(fractionalError < 0.03); // 3%; overly generous, but covers expected spikes
        }
    }*/
    
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
