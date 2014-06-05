/**
 *
 **/


#include "GlobalSettings.h"
#include "Globals.h"

struct GlobalSettings::PIMPL {

    QStringList pluginDirectories;
};

GlobalSettings::GlobalSettings()
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

GlobalSettings::~GlobalSettings()
{
    delete m_imp;
}

QStringList GlobalSettings::pluginDirectories()
{
    return QStringList();
}
