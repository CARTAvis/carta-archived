#include "PercentileManku99.h"
#include <QtTest/QtTest>

class TestPercentileManku99: public QObject
{
    Q_OBJECT
private slots:
    void test_quantiles_data();
    void test_quantiles();
};

// TO CONVERT -- PASTED FROM OLD TEST

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
