#include "catch.h"
#include "TestRawView.h"
#include "core/Algorithms/percentileAlgorithms.h"
#include "core/Algorithms/percentileManku99.h"
#include <random>

TEST_CASE( "Quantile algorithm test", "[quantile]" ) {
    
    std::uniform_real_distribution<double> dist(0, 100);
    std::default_random_engine gen;

    std::vector<int> dims = {200, 200, 10};
    std::vector<double> data(200 * 200 * 10);
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

    delete rawView;
}
