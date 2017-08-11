#include "Converter.h"
#include <QtTest/QtTest>

class TestConverterSpectral: public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
};

void TestConverterSpectral::convert_data()
{
    // TODO: should be kHz, not KHz -- query this and (probably) fix it
    const std::map<QString, std::tuple<QString, int> > WAVELENGTH_UNITS =
        {
            {"Hz", {"Hz", 0}},
            {"10Hz", {"Hz", 1}},
            {"100Hz", {"Hz", 2}},
            {"KHz", {"Hz", 3}},
            {"10KHz", {"Hz", 4}},
            {"100KHz", {"Hz", 5}},
            {"MHz", {"Hz", 6}},
            {"10MHz", {"Hz", 7}},
            {"100MHz", {"Hz", 8}},
            {"GHz", {"Hz", 9}}
        };

    const std::map<QString, std::tuple<QString, int> > VELOCITY_UNITS =
        {
            {"Angstrom", {"m", -10}},
            {"nm", {"m", -9}},
            {"10nm", {"m", -8}},
            {"100nm", {"m", -7}},
            {"um", {"m", -6}},
            {"10um", {"m", -5}},
            {"100um", {"m", -4}},
            {"mm", {"m", -3}},
            {"cm", {"m", -2}},
            {"dm", {"m", -1}},
            {"m", {"m", 0}}
        };

    const std::map<QString, std::tuple<QString, int> > FREQUENCY_UNITS =
        {
            {"m/s", {"m/s", 0}},
            {"10m/s", {"m/s", 1}},
            {"100m/s", {"m/s", 2}},
            {"km/s", {"m/s", 3}}
        };
}

void TestConverterSpectral::convert()
{
    QVERIFY(3 == 3);
}
QTEST_MAIN(TestConverterSpectral)
#include "testConverterSpectral.moc"
