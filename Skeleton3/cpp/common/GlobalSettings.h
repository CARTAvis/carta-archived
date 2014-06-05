/**
 * This class holds configurable global settings for the CARTA vis.
 *
 * It also determines the location of the config file and parses it.
 **/


#pragma once

#include <QStringList>

class GlobalSettings
{

public:

    /// determines location of the config file and parses it
    GlobalSettings();

    /// cleanup
    ~GlobalSettings();

    /// returns the list of conifgured plugin directories
    QStringList pluginDirectories();

protected:

    // implementation details
    struct PIMPL;
    PIMPL * m_imp;
};

