#include "PercentileManku99.h"
#include "Tests/quantileTestCommon.h"
#include <QtTest/QtTest>

class TestPercentileManku99: public QObject
{
    Q_OBJECT
private slots:
    void test_quantiles_data();
    void test_quantiles();
};
