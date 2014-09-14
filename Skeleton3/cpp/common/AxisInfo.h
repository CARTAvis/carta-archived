#include <QString>

#pragma once

/// description of an axis in an image
class AxisInfo {

public:
    QString shortLabel() const { return m_shortLabel; }
    AxisInfo& setShortLabel(const QString& shortLabel) {
        m_shortLabel = shortLabel;
        return *this;
    }

    QString longLabel() const {
        return m_longLabel;
    }
    AxisInfo& setLongLabel(const QString& longLabel) {
        m_longLabel = longLabel;
        return *this;
    }

    QString suffix() const { return m_suffix; }
    AxisInfo& setSuffix(const QString& suffix) {
        m_suffix = suffix;
        return *this;
    }

protected:
    QString m_shortLabel, m_longLabel, m_suffix;
};

