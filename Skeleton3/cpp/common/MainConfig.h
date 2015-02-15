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

    /// whether hacks are enabled or not
    bool hacksEnabled() const;

    /**
     * Returns whether CARTA should come up with areas under
     * active development shown.
     */
    bool isDeveloperLayout() const;

protected:

    QStringList m_pluginDirectories;
    bool m_hacksEnabled = false;
    bool m_developerLayout = false;

    friend ParsedInfo parse( const QString & filePath);
};

///
/// \brief parse the given config file
/// \param path to the JSON file containing the configuration
/// \return parsed config file
///
ParsedInfo parse( const QString & filePath);

}
