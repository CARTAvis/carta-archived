/**
 *
 **/


#include "CmdLine.h"
#include <QDebug>
#include <QCommandLineParser>
#include <QCoreApplication>

static QString cartaGetEnv( const QString & name)
{
    std::string fullName = ("CARTAVIS_" + name).toStdString();
    return qgetenv( fullName.c_str());
}

namespace CmdLine {

static bool m_parsed = false;
static ParsedInfo m_parsedInfo;

/// do the actual parsing of the command line and environment variables
void doParse()
{
    m_parsedInfo.m_configFilePath = "hola";

    QCommandLineParser parser;
    parser.setApplicationDescription("Skeleton3 program");
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

    qDebug() << "Help options:"
             << parser.helpText()
             << "----";

    // Process the actual command line arguments given by the user, exit if
    // command line arguments have a syntax error
    parser.process( * QCoreApplication::instance());

    // get config file path
    if( parser.isSet( configFileOption)) {
        m_parsedInfo.m_configFilePath = parser.value( configFileOption);
    }
    else {
        // try the environment variable
        m_parsedInfo.m_configFilePath = cartaGetEnv( "CONFIG");
    }
    qDebug() << "Config file path=" << m_parsedInfo.configFilePath();

    // get html path
    if( parser.isSet( htmlPathOption)) {
        m_parsedInfo.m_htmlPath = parser.value( htmlPathOption);
    }
    qDebug() << "html path=" << m_parsedInfo.htmlPath();

    // get a list of files to open
    m_parsedInfo.m_fileList = parser.positionalArguments();
    qDebug() << "list of files to open:" << m_parsedInfo.m_fileList;
}

const ParsedInfo & parse()
{
    if( ! m_parsed) {
        doParse();
        m_parsed = true;
    }

    return m_parsedInfo;
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
