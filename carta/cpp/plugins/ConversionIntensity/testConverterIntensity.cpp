#include "ConverterIntensity.h"
#include <QtTest/QtTest>

const double BOLTZMANN = 1.38e-23;
const double LIGHT_SPEED_FACTOR = 9e-10; // WHY?!
const double ARCSECONDS_SQUARED_PER_STERADIAN = pow(180 * 3600 / M_PI, 2);
//const double ARCSECONDS_SQUARED_PER_STERADIAN = pow(206265, 2); // what is used in the plugin
const double JY_IN_MJY = 1e6;

std::vector<QString> BASE_UNITS = {"Jy/beam", "Jy/arcsec^2", "MJy/sr", "Kelvin"};
std::map<QString, double> SI_PREFIX = {{"p", 1e-12}, {"n", 1e-9}, {"u", 1e-6}, {"m", 1e-3}, {"", 1}, {"k", 1e3}, {"M", 1e6}, {"G", 1e9}};
std::map<QString, double> NUM_PREFIX = {{"", 1}, {"10", 10}, {"100", 100}};

const double BEAM_SOLID_ANGLE = 90;
const double BEAM_AREA = 0.0008;

const double KELVIN_TO_JY_BEAM = (2 * BOLTZMANN * BEAM_SOLID_ANGLE) / LIGHT_SPEED_FACTOR;
const double JY_BEAM_TO_KELVIN = LIGHT_SPEED_FACTOR / (2 * BOLTZMANN * BEAM_SOLID_ANGLE);
const double JY_PER_ARCSEC_SQUARED_TO_MJY_PER_STERADIAN = ARCSECONDS_SQUARED_PER_STERADIAN / JY_IN_MJY;
const double MJY_PER_STERADIAN_TO_JY_PER_ARCSEC_SQUARED = JY_IN_MJY / ARCSECONDS_SQUARED_PER_STERADIAN;

class TestConverterIntensity: public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
};

void TestConverterIntensity::convert_data()
{
    std::vector<double> values = {1, 2, 3};
    std::vector<double> x_values = {4, 5, 6};
    std::vector<double> expected_values = {0, 0, 0};

    // Why is the conversion between the same base units so imprecise?
    // This should be as lossless as the test code.
    // Checking the percentile error in the test for now, to distinguish actual failures from loss of precision.
    double eps;
        
    QString from_units;
    QString to_units;

    double from_divisor;
    double to_divisor;
    double multiplier;

    QTest::addColumn<QString>("from_units");
    QTest::addColumn<QString>("to_units");
    QTest::addColumn<std::vector<double>>("values");
    QTest::addColumn<std::vector<double>>("x_values");
    
    QTest::addColumn<std::vector<double>>("expected_values");
    QTest::addColumn<bool>("check_absolute_error");
    QTest::addColumn<double>("eps");

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

                    // common to all subsequent tests

                    from_divisor = n1.second * p1.second;
                    to_divisor = n2.second * p2.second;

                    eps = 1e-5;
                    bool check_absolute_error = (from_divisor == to_divisor && from_divisor == 1) ? 1 : 0;

                    // Within the same base units

                    for (auto b : BASE_UNITS) {

                        from_units = n1.first + p1.first + b;
                        to_units = n2.first + p2.first + b;

                        multiplier = from_divisor / to_divisor;
                    
                        for (int i = 0; i < 3; i++) {
                            expected_values[i] = values[i] * multiplier;
                        }

                        row_name = from_units + " to " + to_units;
                        QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << x_values << expected_values << check_absolute_error << eps;
                    }

                    // Kelvin to Jy/beam
                    
                    from_units = n1.first + p1.first + "Kelvin";
                    to_units = n2.first + p2.first + "Jy/beam";

                    multiplier = KELVIN_TO_JY_BEAM * from_divisor / to_divisor;

                    for (int i = 0; i < 3; i++) {
                        expected_values[i] = values[i] * multiplier * pow(x_values[i], 2);
                    }

                    row_name = from_units + " to " + to_units;

                    QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << x_values << expected_values << check_absolute_error << eps;

                    // Jy/beam to Kelvin
                    
                    from_units = n1.first + p1.first + "Jy/beam";
                    to_units = n2.first + p2.first + "Kelvin";

                    multiplier = JY_BEAM_TO_KELVIN * from_divisor / to_divisor;
                    
                    for (int i = 0; i < 3; i++) {
                        expected_values[i] = (values[i] * multiplier) / pow(x_values[i], 2);
                    }

                    row_name = from_units + " to " + to_units;
                    QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << x_values << expected_values << check_absolute_error << eps;

                    // Jy/arcsec^2 to Jy/Beam
                    
                    from_units = n1.first + p1.first + "Jy/arcsec^2";
                    to_units = n2.first + p2.first + "Jy/beam";

                    multiplier = BEAM_AREA * from_divisor / to_divisor;
                    
                    for (int i = 0; i < 3; i++) {
                        expected_values[i] = values[i] * multiplier;
                    }
                    
                    row_name = from_units + " to " + to_units;
                    QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << x_values << expected_values << check_absolute_error << eps;
                    
                    // Jy/Beam to Jy/arcsec^2
                    
                    from_units = n1.first + p1.first + "Jy/beam";
                    to_units = n2.first + p2.first + "Jy/arcsec^2";

                    multiplier = from_divisor / (BEAM_AREA * to_divisor);
                    
                    for (int i = 0; i < 3; i++) {
                        expected_values[i] = values[i] * multiplier;
                    }
                    
                    row_name = from_units + " to " + to_units;
                    QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << x_values << expected_values << check_absolute_error << eps;
                    
                    // Jy/arcsec^2 to MJy/sr
                    
                    from_units = n1.first + p1.first + "Jy/arcsec^2";
                    to_units = n2.first + p2.first + "MJy/sr";

                    multiplier = JY_PER_ARCSEC_SQUARED_TO_MJY_PER_STERADIAN * from_divisor / to_divisor;
                    //multiplier = pow(206.265, 2) * from_divisor / to_divisor;
                    
                    for (int i = 0; i < 3; i++) {
                        expected_values[i] = values[i] * multiplier;
                    }
                    
                    row_name = from_units + " to " + to_units;
                    QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << x_values << expected_values << check_absolute_error << 1e-3;
                    
                    // MJy/sr to Jy/arcsec^2
                    
                    from_units = n1.first + p1.first + "MJy/sr";
                    to_units = n2.first + p2.first + "Jy/arcsec^2";

                    multiplier = MJY_PER_STERADIAN_TO_JY_PER_ARCSEC_SQUARED * from_divisor / to_divisor;
                    //multiplier = from_divisor / (pow(206.265, 2) * to_divisor);
                    
                    for (int i = 0; i < 3; i++) {
                        expected_values[i] = values[i] * multiplier;
                    }
                    
                    row_name = from_units + " to " + to_units;
                    QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << values << x_values << expected_values << check_absolute_error << 1e-3;
                    
                    // MJy/sr to Jy/Beam
                    // Jy/Beam to MJy/sr
                    
                    // Jy/arcsec^2 to Kelvin
                    // Kelvin to Jy/arcsec^2
                    
                    // MJy/sr to Kelvin
                    // Kelvin to MJy/sr
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
    QFETCH(std::vector<double>, x_values);
    QFETCH(std::vector<double>, expected_values);
    QFETCH(bool, check_absolute_error);
    QFETCH(double, eps);

    double error;
    double percentage_error;
    
    std::string failure_message;
    
    ConverterIntensity::convert( values, x_values, from_units, to_units, 3, from_units, BEAM_SOLID_ANGLE, BEAM_AREA );
    
    for (size_t i = 0; i < values.size(); i++) {
        error = fabs(values[i] - expected_values[i]);
        percentage_error = error * 100 / expected_values[i];
        failure_message = "Error is " + std::to_string(error) + " (" + std::to_string(percentage_error) + "%)";
        // Checking the percentile error for now, to distinguish actual failures from loss of precision.
        QVERIFY2(percentage_error < eps, failure_message.c_str());
        // But also at least check absolute error for conversions between units with no prefix
        if (check_absolute_error) {
            QVERIFY2(error < eps, failure_message.c_str());
        }
    }
}

QTEST_MAIN(TestConverterIntensity)
#include "testConverterIntensity.moc"
