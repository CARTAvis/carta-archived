#include "ConverterIntensity.h"
#include <QtTest/QtTest>

const double BOLTZMANN = 1.38e-23;
const double LIGHT_SPEED_FACTOR = 9e-10; // WHY?!
const double ARCSECONDS_SQUARED_PER_STERADIAN = pow(180 * 3600 / M_PI, 2);
const double JY_IN_MJY = 1e6;

const std::vector<QString> BASE_UNITS = {"Jy/beam", "Jy/arcsec^2", "MJy/sr", "Kelvin"};
const std::map<QString, int> SI_PREFIX = {{"p", -12}, {"n", -9}, {"u", -6}, {"m", -3}, {"", 0}, {"k", 3}, {"M", 6}, {"G", 9}};
const std::map<QString, int> NUM_PREFIX = {{"", 0}, {"10", 1}, {"100", 2}};

// Taken from a sample file; should be consistent
const double BMAJ = 1.88e-4;
const double BMIN = 1.53e-4;

const double BEAM_AREA = M_PI * BMAJ * BMIN / (4 * log(2));
const double BEAM_SOLID_ANGLE = BEAM_AREA / ARCSECONDS_SQUARED_PER_STERADIAN;

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

    std::vector<double> fop_values = {1.0/3, 2.0/3, 1.0};
    double max_value = 3;
        
    QString from_units;
    QString to_units;

    int from_power;
    int to_power;
    double prefix_multiplier;

    QTest::addColumn<QString>("from_units");
    QTest::addColumn<QString>("to_units");
    QTest::addColumn<QString>("max_units");
    
    QTest::addColumn<std::vector<double>>("values");
    QTest::addColumn<std::vector<double>>("x_values");
    QTest::addColumn<double>("max_value");
    
    QTest::addColumn<std::vector<double>>("expected_values");
    QTest::addColumn<bool>("check_absolute_error");
    QTest::addColumn<double>("eps");

    QString row_name;
    
    // frame-dependent conversion portion
    std::map<QString, std::map<QString, std::function<double(double, double)>> > lambdas;
    
    lambdas["Kelvin"]["Jy/beam"] = [](double y, double x){ return y * pow(x, 2); };
    lambdas["Jy/beam"]["Kelvin"] = [](double y, double x){return y / pow(x, 2); };
    
    lambdas["Jy/arcsec^2"]["Jy/beam"] = [](double y, double x){ std::ignore = x; return y; };
    lambdas["Jy/beam"]["Jy/arcsec^2"] = [](double y, double x){ std::ignore = x; return y; };
    
    lambdas["Jy/arcsec^2"]["MJy/sr"] = [](double y, double x){ std::ignore = x; return y; };
    lambdas["MJy/sr"]["Jy/arcsec^2"] = [](double y, double x){ std::ignore = x; return y; };
    
    lambdas["MJy/sr"]["Jy/beam"] = [](double y, double x){ std::ignore = x; return y; };
    lambdas["Jy/beam"]["MJy/sr"] = [](double y, double x){ std::ignore = x; return y; };
    
    lambdas["Jy/arcsec^2"]["Kelvin"] = [](double y, double x){ return y / pow(x, 2); };
    lambdas["Kelvin"]["Jy/arcsec^2"] = [](double y, double x){ return y * pow(x, 2); };
    
    lambdas["MJy/sr"]["Kelvin"] = [](double y, double x){ return y / pow(x, 2); };
    lambdas["Kelvin"]["MJy/sr"] = [](double y, double x){ return y * pow(x, 2); };
    
    // non-frame-dependent conversion portion, excluding unit prefixes
    std::map<QString, std::map<QString, double> > multipliers;
    
    multipliers["Kelvin"]["Jy/beam"] = (2 * BOLTZMANN * BEAM_SOLID_ANGLE) / LIGHT_SPEED_FACTOR;
    multipliers["Jy/beam"]["Kelvin"] = LIGHT_SPEED_FACTOR / (2 * BOLTZMANN * BEAM_SOLID_ANGLE);

    multipliers["Jy/arcsec^2"]["Jy/beam"] = BEAM_AREA;
    multipliers["Jy/beam"]["Jy/arcsec^2"] = 1 / BEAM_AREA;
    
    multipliers["Jy/arcsec^2"]["MJy/sr"] = ARCSECONDS_SQUARED_PER_STERADIAN / JY_IN_MJY;
    multipliers["MJy/sr"]["Jy/arcsec^2"] = JY_IN_MJY / ARCSECONDS_SQUARED_PER_STERADIAN;
    
    multipliers["MJy/sr"]["Jy/beam"] = BEAM_SOLID_ANGLE * JY_IN_MJY;
    multipliers["Jy/beam"]["MJy/sr"] = 1 / (BEAM_SOLID_ANGLE * JY_IN_MJY);
    
    multipliers["Jy/arcsec^2"]["Kelvin"] = (ARCSECONDS_SQUARED_PER_STERADIAN * LIGHT_SPEED_FACTOR) / (2 * BOLTZMANN);
    multipliers["Kelvin"]["Jy/arcsec^2"] = (2 * BOLTZMANN) / (ARCSECONDS_SQUARED_PER_STERADIAN * LIGHT_SPEED_FACTOR);
    
    multipliers["MJy/sr"]["Kelvin"] = (JY_IN_MJY * LIGHT_SPEED_FACTOR) / (2 * BOLTZMANN);
    multipliers["Kelvin"]["MJy/sr"] = (2 * BOLTZMANN) / (JY_IN_MJY * LIGHT_SPEED_FACTOR);
    

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

                    from_power = n1.second + p1.second;
                    to_power = n2.second + p2.second;
                    prefix_multiplier = pow(10, from_power - to_power);
                                        
                    for (auto& b1 : BASE_UNITS) {
                        for (auto& b2 : BASE_UNITS) {

                            from_units = n1.first + p1.first + b1;
                            to_units = n2.first + p2.first + b2;

                            double eps = 1e-12;
                            bool check_absolute_error = (b1 == b2 || from_power == to_power) ? 1 : 0;
                            
                            if (b1 == b2) { // Within same base units
                                for (int i = 0; i < 3; i++) {
                                    expected_values[i] = values[i] * prefix_multiplier;
                                }
                            } else { // Between different base units
                                for (int i = 0; i < 3; i++) {
                                    expected_values[i] = lambdas[b1][b2](values[i], x_values[i]) * multipliers[b1][b2] * prefix_multiplier;
                                }
                            }

                            row_name = from_units + " to " + to_units;
                            QTest::newRow(row_name.toLatin1().data()) << from_units << to_units << from_units << values << x_values << max_value << expected_values << check_absolute_error << eps;

                            row_name = "FOP " + from_units + " to " + to_units;
                            QTest::newRow(row_name.toLatin1().data()) <<  "Fraction of Peak" << to_units << from_units << fop_values << x_values << max_value << expected_values << check_absolute_error << eps;

                            if (from_units == to_units) { // currently we can only convert *to* FOP using the same units
                                row_name = from_units + " to FOP " + to_units;
                                QTest::newRow(row_name.toLatin1().data()) << from_units << "Fraction of Peak" << from_units << values << x_values << max_value << fop_values << check_absolute_error << eps;
                            }

                        }
                    }
                }
            }
        }
    }
}


void TestConverterIntensity::convert()
{
    QFETCH(QString, from_units);
    QFETCH(QString, to_units);
    QFETCH(QString, max_units);
    QFETCH(std::vector<double>, values);
    QFETCH(std::vector<double>, x_values);
    QFETCH(double, max_value);
    QFETCH(std::vector<double>, expected_values);
    QFETCH(bool, check_absolute_error);
    QFETCH(double, eps);

    double error;
    double percentage_error;
    
    QString failure_message;
    
    Carta::Lib::IntensityUnitConverter::SharedPtr converter(ConverterIntensity::converters(from_units, to_units, max_value, max_units, BEAM_AREA));

    values = converter->convert(values, x_values);
    
    for (size_t i = 0; i < values.size(); i++) {
        error = fabs(values[i] - expected_values[i]);
        percentage_error = error * 100 / expected_values[i];
        failure_message = "Expected: " + QString::number(expected_values[i], 'e', 2) + " Actual: " + QString::number(values[i], 'e', 2) + "Error is " + QString::number(error, 'e', 2) + " (" + QString::number(percentage_error, 'e', 2) + "%)";
        // Checking the percentile error for all conversions
        QVERIFY2(percentage_error < eps, failure_message.toLatin1().data());
        // Check absolute error for some conversions which should not accumulate too much loss of precision
        if (check_absolute_error) {
            QVERIFY2(error < eps, failure_message.toLatin1().data());
        }
    }
}

QTEST_MAIN(TestConverterIntensity)
#include "testConverterIntensity.moc"
