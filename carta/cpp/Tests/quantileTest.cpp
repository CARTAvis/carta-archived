#include "catch.h"
#include "TestRawView.h"
#include "core/Algorithms/percentileAlgorithms.h"
#include "core/Algorithms/percentileManku99.h"
#include <QDebug>

TEST_CASE( "Quantile algorithm test", "[quantile]" ) {
    std::vector<double> data = {1,2,3,4,5,6,7,8,9,10,11,12};
    std::vector<int> dims = {2,3,2};
    
    Carta::Lib::NdArray::RawViewInterface * rawView = new TestRawView<double>(data, dims);
    Carta::Lib::NdArray::Double doubleView(rawView, false);

    qDebug() << "view dims" << doubleView.dims();
    
    const std::vector<double> percentiles = {0, 0.01, 0.1, 0.9, 0.99, 1};

    SECTION( "Exact algorithm") {
        Carta::Core::Algorithms::percentile2pixels(doubleView, percentiles);
        //REQUIRE( ??? );
    }

    //SECTION( "Manku 99 algorithm") {
        //Carta::Core::Algorithms::percentile2pixels_approximate_manku99(doubleView, percentiles);
        //REQUIRE( ??? );
    //}

    delete rawView;
}
