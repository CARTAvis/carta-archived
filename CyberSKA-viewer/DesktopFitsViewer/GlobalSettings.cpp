#include "GlobalSettings.h"
#include <QSettings>
#include <QString>
#include <QDir>

static QString globalConfigFilePath() {
    return QDir::homePath () + "/.sfviewer.config";
}

struct ParsedSettings
{
    bool isDebug;
    QString defaultImage;

    struct {
        double timeout;
        QString urlPattern;
    } vizMan;

    bool initialized;
    ParsedSettings() {
        initialized = false;
    }
};

static ParsedSettings m_parsedSettings;

static ParsedSettings & parsedSettings() {
    if( ! m_parsedSettings.initialized) {
        m_parsedSettings.initialized = true;

        QSettings settings( globalConfigFilePath(), QSettings::IniFormat);
        m_parsedSettings.isDebug = settings.value("debug", false).toBool();
        m_parsedSettings.defaultImage = settings.value("defaultImage", "/dev/null").toString();

        // vizman stuff
        settings.beginGroup( "vizMan");
        m_parsedSettings.vizMan.timeout = settings.value( "timeout", 5.0).toDouble();
        m_parsedSettings.vizMan.urlPattern = settings.value( "urlPattern", QString()).toString();
        settings.endGroup();
    }
    return m_parsedSettings;
}

bool GlobalSettings::isDebug()
{
    return parsedSettings().isDebug;
}

QString GlobalSettings::defaultImage()
{
    return parsedSettings().defaultImage;
}

double GlobalSettings::VizMan::timeout()
{
    return parsedSettings().vizMan.timeout;
}

QString GlobalSettings::VizMan::urlPattern()
{
    return parsedSettings().vizMan.urlPattern;
}
