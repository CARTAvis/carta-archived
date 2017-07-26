#include "ConverterIntensity.h"
#include <QtTest/QtTest>

const double EPS = 1e-8;

class TestConverterIntensity: public QObject
{
    Q_OBJECT
private slots:
    void convert();
};

void TestConverterIntensity::convert()
{
    std::vector<double> values = {1, 2, 3};
    std::vector<double> expected_values = {0.1, 0.2, 0.3};
    QString old_units = "Kelvin";
    QString new_units = "10Kelvin";
    
    ConverterIntensity::convert( values, {1, 1, 1}, old_units, new_units, 3, old_units, 90, 0.0008 );
    
    for (int i = 0; i < values.size(); i++) {
        QVERIFY(fabs(values[i] - expected_values[i]) < EPS);
    }
}

QTEST_MAIN(TestConverterIntensity)
#include "testConverterIntensity.moc"
