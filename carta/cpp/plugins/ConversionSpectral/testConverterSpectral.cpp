#include "Converter.h"
#include <QtTest/QtTest>
#include <set>

const std::vector<QString> BASE_UNITS = {"Hz", "m", "m/s"};
const std::map<QString, int> SI_PREFIX = {{"p", -12}, {"n", -9}, {"u", -6}, {"m", -3}, {"", 0}, {"k", 3}, {"M", 6}, {"G", 9}};
const std::map<QString, int> NUM_PREFIX = {{"", 0}, {"10", 1}, {"100", 2}};

class TestConverterSpectral: public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
};

void TestConverterSpectral::convert_data()
{
    // TODO: should be kHz, not KHz -- fix the implementation; see if special handling is necessary
    // TODO: eliminate this entirely after reimplementing
    const std::set<QString> ALLOWED_UNITS = {
        "Hz", "10Hz", "100Hz", "KHz", "10KHz", "100KHz", "MHz", 
        "10MHz", "100MHz", "GHz",
        
        "Angstrom", "nm", "10nm", "100nm", "um", "10um", "100um", "mm", 
        "cm", "dm", "m",

        "m/s", "10m/s", "100m/s", "km/s"
    };
    
    std::vector<double> values = {1, 2, 3};
    std::vector<double> expected_values = {0, 0, 0};

    QString from_units;
    QString to_units;

    int from_power;
    int to_power;
    double prefix_multiplier;
    
    QTest::addColumn<QString>("from_units");
    QTest::addColumn<QString>("to_units");
    QTest::addColumn<std::vector<double>>("values");
    QTest::addColumn<std::vector<double>>("expected_values");
    QTest::addColumn<bool>("check_absolute_error");
    QTest::addColumn<double>("eps");

    QString row_name;

    for (auto& p1 : SI_PREFIX) {
        for (auto& n1 : NUM_PREFIX) {
            for (auto& p2 : SI_PREFIX) {
                for (auto& n2 : NUM_PREFIX) {

                    // common to all subsequent tests

                    from_power = n1.second + p1.second;
                    to_power = n2.second + p2.second;
                    prefix_multiplier = pow(10, from_power - to_power);
                                        
                    for (auto& b1 : BASE_UNITS) {
                        for (auto& b2 : BASE_UNITS) {
                            
                            from_units = n1.first + p1.first + b1;
                            to_units = n2.first + p2.first + b2;
                            
                            // TODO: temporarily change to wrong khz here
                            from_units.replace("kHz", "KHz");
                            to_units.replace("kHz", "KHz");
                            // TODO: temporarily check for allowed units here
                            if (!(ALLOWED_UNITS.count(from_units) && ALLOWED_UNITS.count(to_units))) {
                                continue;
                            }
                            
                            double eps = 1e-12;
                            bool check_absolute_error = (b1 == b2 || from_power == to_power) ? 1 : 0;
                            
                            if (b1 == b2) { // Within same base units
                                for (int i = 0; i < 3; i++) {
                                    expected_values[i] = values[i] * prefix_multiplier;
                                }
                            } else { // Between different base units
                                // TODO: fill these in
                                continue;
                            }

                            row_name = from_units + " to " + to_units;
                            QTest::newRow(row_name.toLatin1().data()) << from_units << to_units  << values << expected_values << check_absolute_error << eps;
                        }
                    }
                }
            }
        }
    }

}

void TestConverterSpectral::convert()
{
    QFETCH(QString, from_units);
    QFETCH(QString, to_units);
    QFETCH(std::vector<double>, values);
    QFETCH(std::vector<double>, expected_values);
    QFETCH(bool, check_absolute_error);
    QFETCH(double, eps);

    double error;
    double percentage_error;
    
    QString failure_message;
    
    Converter* converter = Converter::getConverter(from_units, to_units);
    // TODO this probably needs to be initialised with the right type, etc., which is why it is fetched from the image data.
    casacore::SpectralCoordinate sc;
    std::vector<double> actual_values = converter->convert((casacore::Vector<double>)values, sc).tovector();
    
    for (size_t i = 0; i < actual_values.size(); i++) {
        error = fabs(actual_values[i] - expected_values[i]);
        percentage_error = error * 100 / expected_values[i];
        failure_message = "Expected: " + QString::number(expected_values[i], 'e', 2) + " Actual: " + QString::number(actual_values[i], 'e', 2) + "Error is " + QString::number(error, 'e', 2) + " (" + QString::number(percentage_error, 'e', 2) + "%)";
        // Checking the percentile error for all conversions
        QVERIFY2(percentage_error < eps, failure_message.toLatin1().data());
        // Check absolute error for some conversions which should not accumulate too much loss of precision
        if (check_absolute_error) {
            QVERIFY2(error < eps, failure_message.toLatin1().data());
        }
    }
}
QTEST_MAIN(TestConverterSpectral)
#include "testConverterSpectral.moc"
