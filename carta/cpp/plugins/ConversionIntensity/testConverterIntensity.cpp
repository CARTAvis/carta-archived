#include "ConverterIntensity.h"
#include <QtTest/QtTest>

// Why is the conversion between the same base units so imprecise?
// This should be as lossless as the test code.
// Checking the percentile error in the test for now, to distinguish actual failures from loss of precision.
const double EPS = 1e-5;
const double BOLTZMANN = 1.38e-23;
const double LIGHT_SPEED_FACTOR = 9e-10; // WHY?! 

std::vector<QString> BASE_UNITS = {"Jy/beam", "Jy/arcsec^2", "MJy/sr", "Kelvin"};
std::map<QString, double> SI_PREFIX = {{"p", 1e-12}, {"n", 1e-9}, {"u", 1e-6}, {"m", 1e-3}, {"", 1}, {"k", 1e3}, {"M", 1e6}, {"G", 1e9}};
std::map<QString, double> NUM_PREFIX = {{"", 1}, {"10", 10}, {"100", 100}};

const double BEAM_SOLID_ANGLE = 90;
const double BEAM_AREA = 0.0008;

const double KELVIN_TO_JY_BEAM_CONSTANT = (2 * BOLTZMANN * BEAM_SOLID_ANGLE) / LIGHT_SPEED_FACTOR;
const double JY_BEAM_TO_KELVIN_CONSTANT = LIGHT_SPEED_FACTOR / (2 * BOLTZMANN * BEAM_SOLID_ANGLE);

class TestConverterIntensity: public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
};

void TestConverterIntensity::convert_data()
{
    std::vector<double> values;
    std::vector<double> expected_values;
    
    QString from_units;
    QString to_units;

    double from_divisor;
    double to_divisor;
    double multiplier;

    QTest::addColumn<QString>("from_units");
    QTest::addColumn<QString>("to_units");
    QTest::addColumn<std::vector<double>>("values");
    QTest::addColumn<std::vector<double>>("expected_values");

    QString row_name;

    for (auto& p1 : SI_PREFIX) {
        for (auto& n1 : NUM_PREFIX) {
            
            if (p1.first == "G" && n1.first != "") {
                continue; // not implemented in current plugin; remove after refactoring
            }
            
            for (auto& p2 : SI_PREFIX) {
                for (auto& n2 : NUM_PREFIX) {
                    
                    if (p2.first == "G" && n2.first != "") {
                        continue; // not implemented in current plugin; remove after refactoring
                    }

                    // Within the same base units

                    for (auto b : BASE_UNITS) {

                        from_units = n1.first + p1.first + b;
                        to_units = n2.first + p2.first + b;

                        from_divisor = n1.second * p1.second;
                        to_divisor = n2.second * p2.second;

                        multiplier = from_divisor / to_divisor;

                        values =  {1, 2, 3};
                        expected_values = {1 * multiplier, 2 * multiplier, 3 * multiplier};

                        row_name = from_units + " to " + to_units;

                        QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << expected_values;
                    }

                    // Kelvin to Jy/beam
                    
                    from_units = n1.first + p1.first + "Kelvin";
                    to_units = n2.first + p2.first + "Jy/beam";

                    from_divisor = n1.second * p1.second;
                    to_divisor = n2.second * p2.second;

                    multiplier = KELVIN_TO_JY_BEAM_CONSTANT * from_divisor / to_divisor;

                    values =  {1, 2, 3};
                    expected_values = {1 * multiplier, 2 * multiplier, 3 * multiplier};

                    row_name = from_units + " to " + to_units;

                    QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << expected_values;

                    // Jy/beam to Kelvin
                    
                    from_units = n1.first + p1.first + "Jy/beam";
                    to_units = n2.first + p2.first + "Kelvin";

                    from_divisor = n1.second * p1.second;
                    to_divisor = n2.second * p2.second;

                    multiplier = JY_BEAM_TO_KELVIN_CONSTANT * from_divisor / to_divisor;

                    values =  {1, 2, 3};
                    expected_values = {1 * multiplier, 2 * multiplier, 3 * multiplier};

                    row_name = from_units + " to " + to_units;

                    QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << expected_values;
                }
            }
        }
    }
}


void TestConverterIntensity::convert()
{
    QFETCH(QString, from_units);
    QFETCH(QString, to_units);
    QFETCH(std::vector<double>, values);
    QFETCH(std::vector<double>, expected_values);

    double error;
    double percentage_error;
    
    std::string failure_message;
    
    ConverterIntensity::convert( values, {1, 1, 1}, from_units, to_units, 3, from_units, BEAM_SOLID_ANGLE, BEAM_AREA );
    
    for (size_t i = 0; i < values.size(); i++) {
        error = fabs(values[i] - expected_values[i]);
        percentage_error = error * 100 / expected_values[i];
        failure_message = "Error is " + std::to_string(error) + " (" + std::to_string(percentage_error) + "%)";
        // Checking the percentile error for now, to distinguish actual failures from loss of precision.
        QVERIFY2(percentage_error < EPS, failure_message.c_str());
    }
}

QTEST_MAIN(TestConverterIntensity)
#include "testConverterIntensity.moc"
