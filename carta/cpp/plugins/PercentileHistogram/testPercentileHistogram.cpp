#include "PercentileHistogram.h"
#include "Tests/quantileTestCommon.h"
#include <QtTest/QtTest>
#include <QDebug>

class TestPercentileHistogram: public QObject
{
    Q_OBJECT
private slots:
    void test_quantiles();
};

void TestPercentileHistogram::test_quantiles() {
    std::vector<QuantileTestData> testCases = commonTestCases();
    
    Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator = std::make_shared<PercentileHistogram<double> >(1000);
    
    for (auto& testCase : testCases) {
        calculator->setMinMax(testCase.minMax);
        std::map<double, double> intensities = calculator->percentile2pixels(testCase.view, testCase.percentiles, testCase.spectralIndex, testCase.converter, testCase.hzValues);
        
        QVERIFY(intensities.size() == testCase.percentiles.size());
        
        for (auto& p : testCase.percentiles) {
            int error = fabs(testCase.expected[p] - intensities[p]);
            double fractionalError = (double)error / testCase.expected[p];
            QVERIFY(fractionalError < 0.03); // 3%; overly generous, but covers expected spikes
        }
    }
}

QTEST_MAIN(TestPercentileHistogram)
#include "testPercentileHistogram.moc"
