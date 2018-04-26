#include "PercentileManku99.h"
#include "Tests/quantileTestCommon.h"
#include <QtTest/QtTest>

class TestPercentileManku99: public QObject
{
    Q_OBJECT
private slots:
    void test_quantiles();
};

void TestPercentileManku99::test_quantiles() {
    std::vector<QuantileTestData> testCases = commonTestCases();
    
    Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator = std::make_shared<PercentileManku99<double> >(10, 1000, 10);
    
    for (auto& testCase : testCases) {
        std::map<double, double> intensities = calculator->percentile2pixels(testCase.view, testCase.percentiles, testCase.spectralIndex, testCase.converter, testCase.hzValues);
        
        QVERIFY(intensities.size() == testCase.percentiles.size());
        
        for (auto& p : testCase.percentiles) {
            int error = fabs(testCase.expected[p] - intensities[p]);
            double fractionalError = (double)error / testCase.expected[p];
            QVERIFY(fractionalError < 0.03); // 3%; overly generous, but covers expected spikes
        }
    }
}

QTEST_MAIN(TestPercentileManku99)
#include "testPercentileManku99.moc"
