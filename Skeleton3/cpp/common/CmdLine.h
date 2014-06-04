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

    /// returns the path to the configuaration file, if provided
    /// set either through --cfg file or CARTAVIS_CONFIG environment
    QString configFilePath() const;

    /// returns the path to the html file
    /// set by --html path option
    QString htmlPath() const;

    /// return a list of files to open
    const QStringList & fileList() const;

protected:

    friend void CmdLine::doParse();
    QString m_configFilePath;
    QString m_htmlPath;
    QStringList m_fileList;
};

/// parse the command line, which is extracted form QCoreApplication::arguments()
/// it can be called repeatedly, but the parsing is only done once
const ParsedInfo & parse();

} // namespace CmdLine
