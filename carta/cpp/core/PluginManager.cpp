/**
 *
 **/

#include "PluginManager.h"
#include "Algorithms/Graphs/TopoSort.h"
#include "CartaLib/HtmlString.h"
#include "CartaLib/Hooks/LoadPlugin.h"
#include "Globals.h"
#include "MainConfig.h"
#include <QDirIterator>
#include <QImage>
#include <QPluginLoader>
#include <QLibrary>
#include <QtGlobal>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

namespace Internal
{
static
QJsonValue
recursiveValue( const QJsonObject & obj, const QString & path )
{
    const int indexOfDot = path.indexOf( '.' );
    const QString propertyName = path.left( indexOfDot );
    const QString subPath = indexOfDot > 0 ? path.mid( indexOfDot + 1 ) : QString();
    QJsonValue subValue = obj[propertyName];
    if ( subPath.isEmpty() ) {
        return subValue;
    }
    else {
        QJsonObject obj = subValue.toObject();
        return recursiveValue( obj, subPath );
    }
}
}

PluginManager::PluginManager()
{
//    qDebug() << "Initializing PluginManager...";
}

void
PluginManager::setPluginSearchPaths( const QStringList & pathList )
{
    m_pluginSearchPaths = pathList;
}

// find and load all plugins
void
PluginManager::loadPlugins()
{
    // first handle staticly linked plugins (if any)
    // TODO: disabling this for now...
//    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
//        processLoadedCppPlugin(plugin);
//    }

    // find all plugins in the provided search paths
    // The plugins are not loaded in this step, only parsing is performed.
    // all plugins are put into this list, whether they are native, or not
    m_discoveredPlugins = findAllPlugins();

    qDebug() << "Total plugins found:" << m_discoveredPlugins.size();
    for ( size_t ind = 0 ; ind < m_discoveredPlugins.size() ; ++ind ) {
        qDebug() << "  " << ind << m_discoveredPlugins[ind].json.name;
    }

    // assign a unique integer for each plugin found
    std::map < QString, int > dep2ind;
    for ( size_t i = 0 ; i < m_discoveredPlugins.size() ; i++ ) {
        PluginInfo & pInfo = m_discoveredPlugins[i];
        dep2ind[pInfo.json.name] = i;
    }

    //
    // figure out loading dependencies
    //
    Carta::Core::Algorithms::Graphs::TopoSort tsort( m_discoveredPlugins.size() );

    // for every native plugin, insert arrows into toposort graph for every dependency
    // it has
    for ( size_t i = 0 ; i < m_discoveredPlugins.size() ; i++ ) {
        PluginInfo & pInfo = m_discoveredPlugins[i];

        // skip plugins that already have errors
        if ( ! pInfo.errors.empty() ) {
            continue;
        }

        // for every dependency add appropriate arrow to toposort
        for ( QString & dep : pInfo.json.depends ) {
            // convert dependency to index...
            auto it = dep2ind.find( dep );
            if ( it == dep2ind.end() ) {
                pInfo.errors << "Cannot satisfy dependency '" + dep + "'";
                qCritical() << "Cannot find dependency" << pInfo.json.name << "/" << dep;
                break;
            }
            else {
                tsort.addArrow( it-> second, i );
                //qDebug() << "adding arrow " << i << it-> second;
            }
        }
    }

    // figure out the order
    qDebug() << "toposort";
    auto loadingOrder = tsort.compute();

    // now try to load the plugins in this order
    if ( loadingOrder.size() != m_discoveredPlugins.size() ) {
        // could not figure out order, must have a loop
        qCritical() << "Could not figure out loading order, must have a dependency loop!";
    }
    else {
        qDebug() << "Loading order:";
        for ( auto & ind : loadingOrder ) {
            qDebug() << "  " << ind << m_discoveredPlugins[ind].json.name;
        }
        for ( auto & ind : loadingOrder ) {
            PluginInfo & pInfo = m_discoveredPlugins[ind];
            qDebug() << QString( "Loading plugin %1[%2]" )
                .arg( pInfo.json.name ).arg( pInfo.json.typeString );

            // skip plugins that already have errors
            if ( ! pInfo.errors.empty() ) {
                qDebug() << "...skipping due to previous errors";
                continue;
            }

            // attempt to load native plugin using native method
            if ( pInfo.json.typeString == "c++" || pInfo.json.typeString == "lib" ) {
                bool success = loadNativePlugin( pInfo );
                if ( ! success ) {
                    qCritical() << QString( "Failed to load plugin %1[%2]" )
                        .arg( pInfo.json.name ).arg( pInfo.json.typeString );
                    qCritical() << "...reasons: " << pInfo.errors.join( "\n" );
                    continue;
                }

                // if this plugin is a lib, we can skip to the next plugin
                if ( pInfo.json.typeString == "lib" ) {
                    continue;
                }
            }
            else {
                // let's see if any of the existing plugins can load this plugin
                // via the LoadPlugin hook
                Nullable < IPlugin * > iPlug = prepare < Carta::Lib::Hooks::LoadPlugin > (
                    pInfo.dirPath, pInfo.json ).first();
                if ( iPlug.isSet() ) {
                    pInfo.rawPlugin = iPlug.val();
                }
            }

            // if we failed to make a raw plugin, report an error
            if ( ! pInfo.errors.isEmpty() || ! pInfo.rawPlugin ) {
                qCritical() << "Failed to load plugin using plugins" << pInfo.json.name;
                qCritical() << "...reasons: " << pInfo.errors.join( "\n" );
                pInfo.errors << "Unknown type perhaps?";
                continue;
            }

            // call plugins' initialize()
            qDebug() << "Calling plugin's initialize";
            IPlugin::InitInfo initInfo;
            initInfo.pluginPath = pInfo.dirPath;
            auto json = Globals::instance()-> mainConfig()-> json();
            initInfo.json = json["plugins"].toObject()[pInfo.json.name].toObject();
            pInfo.rawPlugin->initialize( initInfo );

            // find out what hooks this plugin wants to listen to
            qDebug() << "Calling plugin's getInitialHookList";
            auto hooks = pInfo.rawPlugin-> getInitialHookList();

            // for each hook the plugin wants to listen to, add it to the appropriate
            // lookup slot in m_hook2plugin
            for ( auto id : hooks ) {
                m_hook2plugin[id].push_back( & pInfo );
            }
            qDebug() << "Plugin initialized";
        }
    }
} // loadPlugins

const std::vector < PluginManager::PluginInfo > &
PluginManager::getInfoList()
{
    return m_discoveredPlugins;
}

std::vector < PluginManager::PluginInfo >
PluginManager::findAllPlugins()
{
    qDebug() << "Looking for plugins...";

    std::vector < PluginManager::PluginInfo > list;

    // get a list of disabled plugins from json
    QStringList disabledPlugins = Internal::recursiveValue(
        Globals::instance()->mainConfig()->json(),
        "disabledPlugins" ).toVariant().toStringList();
    qDebug() << "Disabled plugins:" << disabledPlugins;

    for ( auto dirPath : m_pluginSearchPaths ) {
        qDebug() << "  processing path:" << dirPath;
        QDir dir( dirPath );
        if ( ! dir.exists() ) {
            qWarning() << "  Skipping non-existant plugin directory";
            continue;
        }
        QDirIterator dit( dir.absolutePath(), QDirIterator::FollowSymlinks );
        while ( dit.hasNext() ) {
            dit.next();

            // skip "." and ".." entries
            if ( dit.fileName() == "." || dit.fileName() == ".." ) {
                continue;
            }

            // skip non-directories
            if ( ! dit.fileInfo().isDir() ) {
                continue;
            }
            qDebug() << "    examining:" << dit.fileInfo().fileName();
            PluginInfo info = parsePluginDir( dit.filePath() );
            if ( ! info.errors.empty() ) {
                qWarning() << "Could not load plugin from:" << dit.filePath()
                           << "\n  - reason: " << info.errors.join( "\n" )
                           << "\n================================";
            }
            else {
                // skip black-listed plugins right now
                if ( disabledPlugins.contains( info.json.name ) ) {
                    qDebug() << "Ignoring disabled plugin:" << dit.filePath();
                }
                else {
                    list.push_back( info );
                }
            }
        }
    }

    qDebug() << "Done looking for plugins. Found: " << list.size();
    return list;
} // findAllPlugins

PluginManager::PluginInfo
PluginManager::parsePluginDir( const QString & dirName )
{
    PluginInfo info;
    info.dirPath = dirName;

    // to avoid duplication of parse error message:
    info.errors << "Could not parse plugin in: " + dirName;

    // try to open the json file
    QString jsonFname = dirName + "/plugin.json";
    QFile file( jsonFname );
    if ( ! file.open( QFile::ReadOnly ) ) {
        info.errors << "...because: could not open: plugin.json";
        info.errors << "...because: " + file.errorString();
        return info;
    }

    // read in the contents of the file
    auto fileContents = file.readAll();

    // parse json
    QJsonParseError jsonErrors;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( fileContents, & jsonErrors );
    if ( jsonDoc.isNull() ) {
        info.errors << "...because could not parse" + jsonFname;
        info.errors << "...because: " + jsonErrors.errorString();
        return info;
    }
    QJsonObject json = jsonDoc.object();

    info.json.name = json["name"].toString();
    if ( info.json.name.isNull() ) {
        info.errors << "...'name' was not specified in plugin.json";
        return info;
    }
    info.json.version = json["version"].toString().toLower().trimmed();
    info.json.typeString = json["type"].toString().toLower().trimmed();
    if ( info.json.typeString.isNull() ) {
        info.errors << "...'type' was not specified in plugin.json";
        return info;
    }

    /// for convenience description could be a single string or an array of strings
    /// in which case we join them into one...
    if ( json["description"].isArray() ) {
        auto list = json["description"].toArray();
        for ( auto entry : list ) {
            info.json.description.append( entry.toString() );
        }
    }
    else {
        info.json.description = json["description"].toString();
    }
    info.json.about = json["about"].toString();
    if ( ! json["depends"].isArray() ) {
        info.errors << "...'depends' must be an array of strings in plugin.json";
        info.errors << QJsonDocument( json ).toJson();
        return info;
    }

    // parse out "depends" list
    {
        auto jsonArray = json["depends"].toArray();
        for ( auto entry : jsonArray ) {
            QString dep = entry.toString().trimmed();
            if ( dep.isNull() ) {
                info.errors << "...null dependency in plugin.json?";
                return info;
            }
            info.json.depends.append( dep );
        }
    }

    // if the plugin type is c++, make sure the plugin has .so file
    if ( info.json.typeString == "c++" ) {
        QFileInfo soInfo( dirName + "/libplugin.so" );
        QFileInfo dylibInfo( dirName + "/libplugin.dylib" );
        if ( ! soInfo.exists() && ! dylibInfo.exists() ) {
            info.errors << "...c++ plugin must have libplugin.so or libplugin.dylib";
            return info;
        }
        if ( ! soInfo.isExecutable() && ! dylibInfo.isExecutable() ) {
            info.errors << "...c++ plugin must have executable libplugin.so or libplugin.dylib";
            return info;
        }
        if ( soInfo.exists() ) {
            info.soPath = soInfo.filePath();
        }
        else {
            info.soPath = dylibInfo.filePath();
        }
    }

    // if the plugin type is c++ or lib, find all libraries under libs subdirectory
    if ( info.json.typeString == "c++" || info.json.typeString == "lib" ) {
        qDebug() << "Looking for libs...";
        QDirIterator dit(
            dirName + "/libs",
            { "*.so", "*.so.*", "*.dylib" },
            QDir::Files | QDir::Executable,
            QDirIterator::Subdirectories | QDirIterator::FollowSymlinks );
        while ( dit.hasNext() ) {
            dit.next();
            qDebug() << "...found:" << dit.fileInfo().fileName();
            qDebug() << "......fullPath:" << dit.filePath();
            info.libPaths.append( dit.filePath() );
        }
    }

    // if we got this far, clear the errors and return the parsed info
    info.errors.clear();
    return info;
} // parsePluginDir

// attempts to load a native plugin described in pInfo, returns true if successful
//
// if successful, pInfo.rawPlugin will contain a pointer to the loaded plugin, unless
// the plugin is a lib-type plugin, in which case it will be nullptr
//
// the plugin is not initialized
bool
PluginManager::loadNativePlugin( PluginManager::PluginInfo & pInfo )
{
//    // skip plugins that already have errors
//    if( ! pInfo.errors.empty()) {
//        return;
//    }

//    // skip non-native plugins
//    if( pInfo.json.typeString != "c++" && pInfo.json.typeString != "lib" ) {
//        return;
//    }

    qDebug() << "Trying to load native plugin" << pInfo.json.name;

    // if the plugin has libs, try to load them in heuristically
    std::vector < int > libsToLoad;
    for ( int i = 0 ; i < pInfo.libPaths.size() ; i++ ) {
        libsToLoad.push_back( i );
    }

    qDebug() << "  - heuristics to load libraries:" << pInfo.libPaths.size();
    while ( ! libsToLoad.empty() ) {
        qDebug() << "  - heuristic loop start with" << libsToLoad.size() << " remaining";

        // remember the old size
        auto oldSize = libsToLoad.size();

        // see if we can load any of the remainging libraries, the ones
        // that cannot be loaded are kept on the list
        auto listCopy = libsToLoad;
        libsToLoad.clear();
        for ( auto ind : listCopy ) {
            auto libPath = pInfo.libPaths[ind];
            qDebug() << "    " + QFileInfo( libPath ).fileName();
            QLibrary lib( libPath );
            // if we don't use ExportExternalSymbolsHint, the seg fault on exit bug seems to
            // disappear
            lib.setLoadHints(QLibrary::ResolveAllSymbolsHint);
//            lib.setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
            if ( ! lib.load() ) {
                qDebug() << "      error:" + lib.errorString();
                libsToLoad.push_back( ind );
            }
            else {
                qDebug() << "      success";
            }
        }

        // check if we made progress
        if ( oldSize > libsToLoad.size() ) {
            // yup, progress made, let's continue with loading more libraries
            continue;
        }
        else {
            // we didn't make progress on this iteration, so there is no need
            // to keep trying
            return false;
        }
    }

    // did we load all libraries?
    if ( ! libsToLoad.empty() ) {
        // we didn't load all libraries, report an error
        QStringList msg;
        msg << "Could not make progress with loading libraries:";
        for ( auto lib : libsToLoad ) {
            msg << "  " + pInfo.libPaths[lib];
        }
        qCritical() << msg.join( "\n" );
        pInfo.errors << "Could not load libraries";
        return false;
    }

    // if this was a lib plugin, we are done
    if ( pInfo.json.typeString != "c++" ) {
        return true;
    }

    // for cpp plugins, try to load in the actual plugin shared library
    QPluginLoader loader( pInfo.soPath );
//    loader.setLoadHints( QLibrary::ResolveAllSymbolsHint );
    loader.setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
    QObject * plugin = loader.instance();
    if ( ! plugin ) {
        qDebug() << "QPluginLoader error = " << loader.errorString();
        pInfo.errors << "QPluginLoader error: " + loader.errorString();

        // QPluginLoader is not very verbose with error messages, so let's see
        // if we can get QLibrary get us more detailed message
        QLibrary lib( pInfo.soPath );
        lib.setLoadHints( loader.loadHints() );
        if ( ! lib.load() ) {
            qDebug() << "QLibrary error:" << lib.errorString();
            pInfo.errors << "QLibrary error:" + lib.errorString();
        }
        else {
            // nope, no extra insight from QLibrary
        }
        return false;
    }
    qDebug() << "Raw plugin loaded.";

    // try to cast the loaded qobject to our carta plugin interface
    IPlugin * cartaPlugin = qobject_cast < IPlugin * > ( plugin );
    if ( ! cartaPlugin ) {
        // not a carta plugin, ignore it
        qDebug() << pInfo.json.name << ": not a carta plugin";
        pInfo.errors << "not a CARTA plugin";
        return false;
    }

    // add info about this plugin to our list
    pInfo.rawPlugin = cartaPlugin;

    qDebug() << "Carta plugin loaded.";

    return true;
} // loadNativePlugin

#ifdef DONT_COMPILE

/// process the loaded CPP plugin
void
PluginManager::processLoadedCppPluginOld( QObject * plugin, QString path )
{
    IPlugin * cartaPlugin = qobject_cast < IPlugin * > ( plugin );
    if ( ! cartaPlugin ) {
        // not a carta plugin, ignore it
        qDebug() << "not a carta plugin";
        return;
    }

    qDebug() << "yup, a carta plugin" << plugin;

    // add info about this plugin to our list
    PluginInfo * info = new PluginInfo;
    info-> rawPlugin = cartaPlugin;
    info-> soPath = path;
    info-> name = QFileInfo( path ).baseName();
    m_allLoadedPlugins.push_back( info );

    // find out what hooks this plugin wants to listen to
    auto hooks = cartaPlugin-> getInitialHookList();

    // for each hook the plugin wants to listen to, add it to the appropriate
    // lookup slot in m_hook2plugin
    for ( auto id : hooks ) {
        m_hook2plugin[id].push_back( info );
    }
} // processLoadedCppPluginOld

// API testing

void
fakeMain()
{
    PluginManager pm;

    QString filename = "file.json";
    QImage testImage;

    // configure the manager from a filename
    // JSON or INI format?
    pm.loadConfig( filename );

    // load all plugins
    pm.loadPlugins();

    // execute a hook that calls all plugins, with some parameters, and a return type
    // the result is an array of the results
    //    std::vector<QImage> images = pm.hookAll<Render>(8, "Hello", testImage);

    // get info about all plugins
    const std::vector < PluginManager::PluginInfo * > & infoList = pm.getInfoList();
    for ( const auto & entry : infoList ) {
        // print out info about each plugin
        // ...
        [] ( decltype( entry ) ) { };
    }

    // execute a hook that calls all plugins, with no parameters and no return type
    pm.prepare < Initialize > ();

    // execute a hook that calls all plugins, with some parameters and no return type
    //    pm.hookAll<Render>( 8, "Hello", testImage);

    // execute a hook that calls first plugin that answers and returns the result
    //    auto res2 = pm.hookFirst<GetRandomNumber>();

    // execute a hook with some parameters and a result, and execute a closure
    // for each result
    auto helper = pm.prepare < PreRender > ( "Test", & testImage );
    helper.forEachCond([] (PreRender::ResultType) -> bool { return true;
                       }
                       );

    //    pm.hookAll2<Render>( 3, "Test", testImage).forEach(
    //                [] ( const Render::ResultType & )
    //                );
} // fakeMain

#endif // ifdef DONT_COMPILE
