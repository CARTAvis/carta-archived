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
#include <cmath>

namespace MainConfig {

namespace {
void _storeBool( const QJsonValue& jsonValue, bool* storeLocation,
        const QString& typeDescription ){
    QString errorMsg;
    *storeLocation = ParsedInfo::toBool( jsonValue, errorMsg );
    if ( !errorMsg.isEmpty() ){
        qWarning() << "Error setting "<< typeDescription<<": "<<errorMsg;
    }
}
}

namespace {
void _storePositiveInt( const QJsonValue& jsonValue, int* storeLocation, const QString& typeDescription ){
    QString errorMsg;
    int val = ParsedInfo::toInt( jsonValue, errorMsg );
    if ( errorMsg.isEmpty() ){
        if ( val > 0 ){
            *storeLocation = val;
        }
        else if ( val < -1 ){
            qWarning()<<"Error "<<typeDescription<<"  must be a positive integer:"<<val;
        }
    }
    else {
        qWarning()<<"Error setting "<<typeDescription<<": "<<errorMsg;
    }
}
}

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
    info.m_json = json;

    // extract plugin directories
    auto pluginDirs = json[ "pluginDirs"].toArray().toVariantList();
    for( auto  dir : pluginDirs) {
        QString raw = dir.toString();
        // perform some substitutions
        // TODO: document these substitutions somewhere
        raw.replace( "$(HOME)", QDir::homePath());
        raw.replace( "$(APPDIR)", QCoreApplication::applicationDirPath());
        info.m_pluginDirectories.append( QDir::cleanPath(raw));
    }

    _storeBool( json["hacksEnabled"], &info.m_hacksEnabled, "hacks enabled");
    _storeBool( json["developerLayout"], &info.m_developerLayout, "developer layout");
    _storeBool( json["qtDecorations"], &info.m_developerDecorations, "developer decorations");

    _storePositiveInt( json["histogramBinCountMax"], &info.m_histogramBinCountMax, "histogram bin count max");
    _storePositiveInt( json["contourLevelCountMax"], &info.m_contourLevelCountMax, "contour level count max");

    return info;
}

const QStringList & ParsedInfo::pluginDirectories() const
{
    return m_pluginDirectories;
}

bool ParsedInfo::hacksEnabled() const
{
    qDebug() << "Hacks enabled retuning "<<m_hacksEnabled;
    return m_hacksEnabled;
}

bool ParsedInfo::isDeveloperDecorations() const {
    return m_developerDecorations;
}


bool ParsedInfo::isDeveloperLayout() const {
    return m_developerLayout;
}

int ParsedInfo::getContourLevelCountMax() const {
    return m_contourLevelCountMax;
}

int ParsedInfo::getHistogramBinCountMax() const {
    return m_histogramBinCountMax;
}

const QJsonObject &ParsedInfo::json() const
{
    return m_json;
}


bool ParsedInfo::toBool( const QJsonValue& jsonValue, QString& errorMsg ){
    bool val = false;
    if ( jsonValue.isString() ){
        QString lowerStr = jsonValue.toString().toLower();
        val = ( lowerStr == "yes" || lowerStr == "true" ||
                lowerStr == "1" || lowerStr == "y");
    }
    else if ( jsonValue.isBool() ){
        val = jsonValue.toBool();
    }
    else if ( !jsonValue.isUndefined() && !jsonValue.isNull() ){
        errorMsg = "Not a valid boolean.";
    }
    return val;
}

int ParsedInfo::toInt( const QJsonValue& jsonValue, QString& errorMsg ){
    int val = -1;
    if ( jsonValue.isString() ){
        QString valStr = jsonValue.toString();
        bool validInt = false;
        val = valStr.toInt( &validInt );
        if ( !validInt ){
            errorMsg = "Invalid integer type:"+valStr;
        }
    }
    else if ( jsonValue.isDouble() ){
        double doubleVal = jsonValue.toDouble();
        double integerPart;
        if ( std::modf( doubleVal, &integerPart) == 0 ){
            val = static_cast<int>( integerPart );
        }
        else {
            errorMsg = "Number must be an integer: "+ QString::number( doubleVal );
        }
    }
    else if ( !jsonValue.isUndefined() && !jsonValue.isNull() ){
        errorMsg = "Not a valid integer.";
    }
    return val;
}

QJsonObject::iterator ParsedInfo::insert(const QString &key, const QJsonValue &value) {
    return m_json.insert(key, value);
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
