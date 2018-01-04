#include "catch.h"
#include "quantileTestCommon.h"
#include "core/Algorithms/percentileAlgorithms.h"
#include <random>

TEST_CASE( "Exact quantile algorithm test", "[quantile]" ) {

    std::vector<QuantileTestData> testCases = commonTestCases();

    Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator = std::make_shared<Carta::Core::Algorithms::PercentilesToPixels<double> >();
        
    for (auto& testCase : testCases) {
            
        SECTION( testCase.name ) {
            std::map<double, double> intensities = calculator->percentile2pixels(testCase.view, testCase.percentiles, testCase.spectralIndex, testCase.converter, testCase.hzValues);
            
            REQUIRE(intensities.size() == testCase.percentiles.size());
            
            for (auto& p : testCase.percentiles) {
                REQUIRE(fabs(testCase.expected[p] - intensities[p]) < 1e-10);
            }
        }
    }
}

TEST_CASE( "View stub test", "[quantile]" ){

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

}
