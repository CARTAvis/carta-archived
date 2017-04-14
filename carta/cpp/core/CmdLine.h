/**
 * Command line & environment variable parser for CartaVis.
 *
 **/

#pragma once

#include <QString>
#include <QStringList>

namespace CmdLine {

class ParsedInfo {

public:

    /// returns the path to the configuaration file
    /// set either through '--cfg file' option or $CARTAVIS_CONFIG environment
    /// if config file was not provided, the default is used, which is:
    /// $HOME/.cartavis/config.json (1st option) or $(carta build)/config/config.json (2nd option)
    QString configFilePath() const;

    /// returns the path to the html file, only used by the desktop version
    /// set by '--html path' option
    QString htmlPath() const;

    /// return a list of files to open
    const QStringList & fileList() const;

    /// return the port number on which to listen for commands
    /// -1 indicates no port was specified
    int scriptPort() const;

protected:

    friend ParsedInfo parse( const QStringList & argv);
    QString m_configFilePath;
    QString m_htmlPath;
    QStringList m_fileList;
    int m_scriptPort = -1;

};

/// parse the command line, which is extracted from QCoreApplication::arguments()
/// if error occurred, or user asks for '-v' or '-h' options, usage info is printed
/// to stdout and application is terminated.
ParsedInfo parse( const QStringList & argv);

} // namespace CmdLine
