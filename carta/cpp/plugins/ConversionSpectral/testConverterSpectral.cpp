#include "Converter.h"
#include <QtTest/QtTest>

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
    const std::set<QString> ALLOWED_UNITS =
        "Hz", "10Hz", "100Hz", "KHz", "10KHz", "100KHz", "MHz", 
        "10MHz", "100MHz", "GHz",
        
        "Angstrom", "nm", "10nm", "100nm", "um", "10um", "100um", "mm", 
        "cm", "dm", "m",

        "m/s", "10m/s", "100m/s", "km/s"
    };
    
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
                            
                            // ....
                        }
                    }
                }
            }
        }
    }

}

void TestConverterSpectral::convert()
{
    QVERIFY(3 == 3);
}
QTEST_MAIN(TestConverterSpectral)
#include "testConverterSpectral.moc"
