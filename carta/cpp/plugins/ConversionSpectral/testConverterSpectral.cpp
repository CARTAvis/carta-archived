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
}

void TestConverterSpectral::convert()
{
    QVERIFY(3 == 3);
}
QTEST_MAIN(TestConverterSpectral)
#include "testConverterSpectral.moc"
