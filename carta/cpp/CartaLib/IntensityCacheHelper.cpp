#include "IntensityCacheHelper.h"
#include "CartaLib/Algorithms/cacheUtils.h"

IntensityValue::IntensityValue(double value, double error) : value(value), error(error) {
}

IntensityCacheHelper::IntensityCacheHelper(std::shared_ptr<Carta::Lib::IPCache> diskCache) : m_diskCache(diskCache) {
}

std::pair<double, double>IntensityValue * IntensityCacheHelper::get(int frameLow, int frameHigh, double percentile, int stokeFrame, QString transformationLabel) {
    QString intensityKey = QString("%1/%2/%3/%4/%5/%6/intensity").arg(m_fileName).arg(frameLow).arg(frameHigh).arg(stokeFrame).arg(percentile).arg(transformationLabel);
        
    QByteArray intensityVal, intensityError;
    bool intensityInCache = m_diskCache->readEntry(intensityKey.toUtf8(), intensityVal, intensityError);
    
    if (intensityInCache) {
        value = qb2d(intensityVal);
        error = qb2d(intensityError);
        return &IntensityValue(value, error);
    }
    
    return nullptr;
}

void IntensityCacheHelper::set(double intensity, double error, int frameLow, int frameHigh, double percentile, int stokeFrame, QString transformationLabel) {
    QString intensityKey = QString("%1/%2/%3/%4/%5/%6/intensity").arg(m_fileName).arg(frameLow).arg(frameHigh).arg(stokeFrame).arg(percentile).arg(transformationLabel);
    
    m_diskCache->setEntry(intensityKey.toUtf8(), d2qb(intensity), d2qb(error));
}