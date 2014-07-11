/**
 *
 **/


#include "MainConfig.h"
#include "Globals.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>

namespace MainConfig {

ParsedInfo parse(const QString & filePath)
{
    qDebug() << "Parsing global settings from" << filePath;

    // prepare result
    ParsedInfo info;

    // open file and read it in
    QFile file( filePath);
    if( ! file.open( QFile::ReadOnly)) {
        qWarning() << "Could not open main config file from" << filePath;
        return info;
    }
    auto fileContents = file.readAll();
    qDebug() << "Config file has" << fileContents.size() << "bytes";

    // parse json
    QJsonParseError errors;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( fileContents, & errors);
    if( jsonDoc.isNull()) {
        qWarning() << "Could not parse config file" << filePath;
        qWarning() << "Errors below:";
        qWarning() << errors.errorString();
        return info;
    }
    QJsonObject json = jsonDoc.object();

    // extract plugin directories
    auto pluginDirs = json[ "pluginDirs"].toArray().toVariantList();
    qDebug() << "Number of pluginDirs="<<pluginDirs.size();
    for( auto  dir : pluginDirs) {
        QString raw = dir.toString();
        qDebug() << "Raw="<<raw;
        // perform some substitutions
        // TODO: document these substitutions somewhere
        raw.replace( "$(HOME)", QDir::homePath());
        raw.replace( "$(APPDIR)", QCoreApplication::applicationDirPath());
        qDebug() << "After replace raw="<<raw;
        info.m_pluginDirectories.append( QDir::cleanPath(raw));
    }
//    qDebug() << "All dirs" << info.pluginDirectories();

    return info;
}

const QStringList & ParsedInfo::pluginDirectories() const
{
    return m_pluginDirectories;
}

} // namespace MainConfig


#ifdef DONT_COMPILE

class GlobalSettingsOld
{

public:

    /// determines location of the config file and parses it
    GlobalSettingsOld();

    /// cleanup
    ~GlobalSettingsOld();

    /// returns the list of conifgured plugin directories
    QStringList pluginDirectories();

protected:

    // implementation details
    struct PIMPL;
    PIMPL * m_imp;
};


struct GlobalSettingsOld::PIMPL {

    QStringList pluginDirectories;
};

GlobalSettingsOld::GlobalSettingsOld()
{
    m_imp = new PIMPL;

    QString configFName;
    // if command line option was given for the config file, use that
    // -config filename

    // otherwise if environment variable was defined, use that
    // $CARTAVIS_CONFIG_FILE
    if( configFName.isNull()) {

    }

    // otherwise use the default path
    if( configFName.isNull()) {

    }

}

GlobalSettingsOld::~GlobalSettingsOld()
{
    delete m_imp;
}

QStringList GlobalSettingsOld::pluginDirectories()
{
    return QStringList();
}

#endif
