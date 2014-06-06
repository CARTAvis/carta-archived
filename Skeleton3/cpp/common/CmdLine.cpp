/**
 *
 **/


#include "CmdLine.h"
#include <QDebug>
#include <QCommandLineParser>
#include <QDir>

static QString cartaGetEnv( const QString & name)
{
    std::string fullName = ("CARTAVIS_" + name).toStdString();
    return qgetenv( fullName.c_str());
}

namespace CmdLine {

ParsedInfo parse(const QStringList & argv)
{
    QCommandLineParser parser;
//    parser.setApplicationDescription("Skeleton3 program");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument( "input-files", "list of files to open", "[input files]");
    QCommandLineOption configFileOption(
        { "config", "cfg"}, "config file path", "configFilePath");
    parser.addOption( configFileOption);
    QCommandLineOption htmlPathOption(
        "html", "development option for desktop version, path to html to load", "htmlPath"
                );
    parser.addOption( htmlPathOption);

    // Process the actual command line arguments given by the user, exit if
    // command line arguments have a syntax error, or the user asks for -h or -v
    parser.process( argv);

    // try to get config file path from command line
    ParsedInfo info;
    info.m_configFilePath = parser.value( configFileOption);
    // if command line was not used to set the config file path, try environment var
    if( info.m_configFilePath.isEmpty()) {
        info.m_configFilePath = cartaGetEnv( "CONFIG");
    }
    // if the config file was not specified neither through command line or environment
    // assign a default value
    if( info.m_configFilePath.isEmpty()) {
        info.m_configFilePath = QDir::homePath() + "/.cartavis/config.json";
    }
    qDebug() << "Config file path=" << info.configFilePath();

    // get html path
    if( parser.isSet( htmlPathOption)) {
        info.m_htmlPath = parser.value( htmlPathOption);
    }
    qDebug() << "html path=" << info.htmlPath();

    // get a list of files to open
    info.m_fileList = parser.positionalArguments();
    qDebug() << "list of files to open:" << info.m_fileList;

    return info;
}

QString ParsedInfo::configFilePath() const
{
    return m_configFilePath;
}

QString ParsedInfo::htmlPath() const
{
    return m_htmlPath;
}

const QStringList &ParsedInfo::fileList() const
{
    return m_fileList;
}

} // namespace CmdLine
