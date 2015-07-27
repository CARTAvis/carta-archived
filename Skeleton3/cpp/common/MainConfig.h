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

    /**
     * Returns any valid user set histogram maximum bin count or -1 if no
     * valid user supplied value has been provided.
     * @return the maximum allowed value for the histogram bin count or -1 is
     *   no valid value has been specified.
     */
    int getHistogramBinCountMax() const;

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
    int m_histogramBinCountMax = -1;

    friend ParsedInfo parse( const QString & filePath);
};

///
/// \brief parse the given config file
/// \param path to the JSON file containing the configuration
/// \return parsed config file
///
ParsedInfo parse( const QString & filePath);

}
