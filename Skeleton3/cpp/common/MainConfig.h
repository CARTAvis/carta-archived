/**
 * Reaing global settings file at startup.
 *
 **/

#pragma once

#include <QStringList>
class QString;

namespace MainConfig {

///
/// \brief The ParsedInfo contains parsed data from the main configuration file
///
class ParsedInfo {

public:

    ///
    /// \brief get a list of plugin directories
    ///
    const QStringList & pluginDirectories() const;

protected:

    QStringList m_pluginDirectories;
    friend ParsedInfo parse( const QString & filePath);
};

///
/// \brief parse the given config file
/// \param path to the JSON file containing the configuration
/// \return parsed config file
///
ParsedInfo parse( const QString & filePath);

}
