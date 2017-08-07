#include "ConverterIntensity.h"
#include <QtTest/QtTest>

const double BOLTZMANN = 1.38e-23;
const double LIGHT_SPEED_FACTOR = 9e-10; // WHY?!
const double ARCSECONDS_SQUARED_PER_STERADIAN = pow(180 * 3600 / M_PI, 2);
//const double ARCSECONDS_SQUARED_PER_STERADIAN = pow(206265, 2); // what is used in the plugin
const double JY_IN_MJY = 1e6;

std::vector<QString> BASE_UNITS = {"Jy/beam", "Jy/arcsec^2", "MJy/sr", "Kelvin"};
std::map<QString, int> SI_PREFIX = {{"p", -12}, {"n", -9}, {"u", -6}, {"m", -3}, {"", 0}, {"k", 3}, {"M", 6}, {"G", 9}};
std::map<QString, int> NUM_PREFIX = {{"", 0}, {"10", 1}, {"100", 2}};

// Taken from a sample file; should be consistent
const double BMAJ = 1.88e-4;
const double BMIN = 1.53e-4;
const double BPA = 72.5; // check if this is in arcsecs squared

// From plugin code
const double HPBW_SQUARED = BMAJ * BMIN;
const double BEAM_SOLID_ANGLE = M_PI * HPBW_SQUARED / (ARCSECONDS_SQUARED_PER_STERADIAN * 4 * log(2)); // constant in plugin is misleadingly named
const double BEAM_AREA = BPA; // is this right?

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
    
    lambdas["Jy/arcsec^2"]["Jy/beam"] = [](double y, double x){ return y; };
    lambdas["Jy/beam"]["Jy/arcsec^2"] = [](double y, double x){ return y; };
    
    lambdas["Jy/arcsec^2"]["MJy/sr"] = [](double y, double x){ return y; };
    lambdas["MJy/sr"]["Jy/arcsec^2"] = [](double y, double x){ return y; };
    
    lambdas["MJy/sr"]["Jy/beam"] = [](double y, double x){ return y; };
    lambdas["Jy/beam"]["MJy/sr"] = [](double y, double x){ return y; };
    
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
    
    multipliers["MJy/sr"]["Jy/beam"] = (BEAM_AREA * JY_IN_MJY) / ARCSECONDS_SQUARED_PER_STERADIAN;
    multipliers["Jy/beam"]["MJy/sr"] = ARCSECONDS_SQUARED_PER_STERADIAN / (BEAM_AREA * JY_IN_MJY);
    
    multipliers["Jy/arcsec^2"]["Kelvin"] = (BEAM_AREA * LIGHT_SPEED_FACTOR) / (2 * BOLTZMANN * BEAM_SOLID_ANGLE);
    multipliers["Kelvin"]["Jy/arcsec^2"] = (2 * BOLTZMANN * BEAM_SOLID_ANGLE) / (BEAM_AREA * LIGHT_SPEED_FACTOR);
    
    multipliers["MJy/sr"]["Kelvin"] = (BEAM_AREA * JY_IN_MJY * LIGHT_SPEED_FACTOR) / (ARCSECONDS_SQUARED_PER_STERADIAN * 2 * BOLTZMANN * BEAM_SOLID_ANGLE);
    multipliers["Kelvin"]["MJy/sr"] = (ARCSECONDS_SQUARED_PER_STERADIAN * 2 * BOLTZMANN * BEAM_SOLID_ANGLE) / (BEAM_AREA * JY_IN_MJY * LIGHT_SPEED_FACTOR);
    

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
                    
                    bool check_absolute_error = (from_power == to_power && from_power == 0) ? 1 : 0;
                    
                    for (auto b1 : BASE_UNITS) {
                        for (auto b2 : BASE_UNITS) {
                            if (b1 == b2) { // Within same base units
                                for (int i = 0; i < 3; i++) {
                                    expected_values[i] = values[i] * prefix_multiplier;
                                }
                            } else { // Between different base units          
                                for (int i = 0; i < 3; i++) {
                                    expected_values[i] = lambdas[b1][b2](values[i], x_values[i]) * multipliers[b1][b2] * prefix_multiplier;
                                }
                            }

                            from_units = n1.first + p1.first + b1;
                            to_units = n2.first + p2.first + b2;
                            
                            // The arcsecond/steradian conversion factor in the plugin has low precision, so we lower this for now
                            double eps = (b1 == "MJy/sr" || b2 == "MJy/sr") ? 1e-3 : 1e-5;

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
    
    std::string failure_message;
    
    ConverterIntensity::convert( values, x_values, from_units, to_units, max_value, max_units, BEAM_SOLID_ANGLE, BEAM_AREA );
    
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
