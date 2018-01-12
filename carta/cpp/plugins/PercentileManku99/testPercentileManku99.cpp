#include "PercentileManku99.h"
#include "Tests/quantileTestCommon.h"
#include <QtTest/QtTest>

class TestPercentileManku99: public QObject
{
    Q_OBJECT
private slots:
    void test_quantiles_data();
    void test_quantiles();
};

void TestPercentileManku99::test_quantiles_data() {
    std::vector<QuantileTestData> testCases = commonTestCases();
    
    Q_DECLARE_METATYPE(Carta::Lib::NdArray::Double);
    Q_DECLARE_METATYPE(Carta::Lib::IntensityUnitConverter::SharedPtr);
    
    QTest::addColumn<Carta::Lib::NdArray::Double>("view");
    QTest::addColumn<std::vector<double> >("percentiles");
    QTest::addColumn<int>("spectral_index");
    QTest::addColumn<Carta::Lib::IntensityUnitConverter::SharedPtr>("converter");
    QTest::addColumn<std::vector<double> >("hz_values");
    
    QTest::addColumn<std::map<double, double> >("expected");
    
    for (auto& testCase : testCases) {
        QTest::newRow(testCase.name.c_str()) << testCase.view << testCase.percentiles << testCase.spectralIndex << testCase.converter << testCase.hzValues << testCase.expected;
    }
}

void TestPercentileManku99::test_quantiles() {
    QFETCH(Carta::Lib::NdArray::Double, view);
    QFETCH(std::vector<double>, percentiles);
    QFETCH(int, spectral_index);
    QFETCH(Carta::Lib::IntensityUnitConverter::SharedPtr, converter);
    QFETCH(std::vector<double>, hz_values);
    
    typedef std::map<double, double> DoubleMap;
    Q_DECLARE_METATYPE(DoubleMap);
    QFETCH(DoubleMap, expected);
    
    std::map <double, double> intensities = PercentileManku99<double>::percentile2pixels(view, percentiles, spectralIndex, converter, hzValues);
    
    QVERIFY(intensities.size() == percentiles.size());
    
    for (auto& p : percentiles) {
        int error = fabs(expected[p] - intensities[p]);
        double fractionalError = (double)error / size;
        //qDebug() << error << fractionalError;
        QVERIFY(fractionalError < 0.03); // 3%; overly generous, but covers expected spikes
    }
}
