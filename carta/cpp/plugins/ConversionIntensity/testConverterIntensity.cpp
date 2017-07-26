#include "ConverterIntensity.h"
#include <QtTest/QtTest>

// Why is the conversion between the same base units so imprecise?
// This should be as lossless as the test code.
const double EPS = 1e-8;

class TestConverterIntensity: public QObject
{
    Q_OBJECT
private slots:
    void convertWithinBaseUnits_data();
    void convertWithinBaseUnits();
};

void TestConverterIntensity::convertWithinBaseUnits_data()
{
    std::vector<double> values;
    std::vector<double> expected_values;
    
    QString from_units;
    QString to_units;

    double from_divisor;
    double to_divisor;
    double multiplier;

    std::vector<QString> base_units = {"Jy/beam", "Jy/arcsec^2", "mMJy/sr", "Kelvin"};
    std::map<QString, double> si_prefix = {{"p", 1e-12}, {"n", 1e-9}, {"u", 1e-6}, {"m", 1e-3}, {"", 1}, {"k", 1e3}, {"M", 1e6}, {"G", 1e9}};
    std::map<QString, double> num_prefix = {{"", 1}, {"10", 10}, {"100", 100}};

    QTest::addColumn<QString>("from_units");
    QTest::addColumn<QString>("to_units");
    QTest::addColumn<std::vector<double>>("values");
    QTest::addColumn<std::vector<double>>("expected_values");

    for (auto b : base_units) {
        
        for (auto& p1 : si_prefix) {
            for (auto& n1 : num_prefix) {
                
                if (p1.first == "G" && n1.first != "") {
                    continue; // not implemented in current plugin; remove after refactoring
                }
                
                for (auto& p2 : si_prefix) {
                    for (auto& n2 : num_prefix) {
                        
                        if (p2.first == "G" && n2.first != "") {
                            continue; // not implemented in current plugin; remove after refactoring
                        }
                        
                        from_units = n1.first + p1.first + b;
                        to_units = n2.first + p2.first + b;

                        from_divisor = n1.second * p1.second;
                        to_divisor = n2.second * p2.second;

                        multiplier = from_divisor / to_divisor;

                        values =  {1, 2, 3};
                        expected_values = {1 * multiplier, 2 * multiplier, 3 * multiplier};

                        QString row_name = from_units + " to " + to_units;

                        QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << expected_values;
                    }
                }
            }
        }
    }
}


void TestConverterIntensity::convertWithinBaseUnits()
{
    QFETCH(QString, from_units);
    QFETCH(QString, to_units);
    QFETCH(std::vector<double>, values);
    QFETCH(std::vector<double>, expected_values);

    double error;
    std::string failure_message;
    
    ConverterIntensity::convert( values, {1, 1, 1}, from_units, to_units, 3, from_units, 90, 0.0008 );
    
    for (size_t i = 0; i < values.size(); i++) {
        error = fabs(values[i] - expected_values[i]);
        failure_message = "Error is " + std::to_string(error);
        QVERIFY2(error < EPS, failure_message.c_str());
    }
}


QTEST_MAIN(TestConverterIntensity)
#include "testConverterIntensity.moc"
