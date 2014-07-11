/**
 *
 **/


#include "PluginManager.h"
#include "Algorithms/Graphs/TopoSort.h"

#include <QDirIterator>
#include <QImage>
#include <QPluginLoader>
#include <QLibrary>
#include <QtGlobal>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

PluginManager::PluginManager()
{
//    qDebug() << "Initializing PluginManager...";

//    // TODO: this is a hack for now to load casacore libraries on which some plugins
//    // depend. This should be removed once we have a proper plugin system in place.
//    QStringList libs;
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_casa.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_scimath_f.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_scimath.so");
//    libs.append( "/home/pfederl/Software/cfitsio3360shared/lib/libcfitsio.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_tables.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_measures.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_fits.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_coordinates.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_components.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_mirlib.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_lattices.so");
//    libs.append( "/home/pfederl/Software/casacore-1.5.0-shared/lib/libcasa_images.so");

//    for( auto fname : libs) {
//        qDebug() << "loading " << fname;
//        QLibrary lib( fname);
//        if( ! lib.load()) {
//            qDebug() << " error:" << lib.errorString();
//        }
//        else {
//            qDebug() << " success";
//        }

//    }
}

void PluginManager::setPluginSearchPaths(const QStringList & pathList)
{
    m_pluginSearchPaths = pathList;
}


void PluginManager::loadPlugins()
{

    // first handle staticly linked plugins (if any)
    // TODO: disabling this for now...
//    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
//        processLoadedCppPlugin(plugin);
//    }

    // find all plugins in the provided search paths
    // The plugins are not loaded in this step, only parsing is performed.
    m_discoveredPlugins = findAllPlugins();

    qDebug() << "Total plugins found:" << m_discoveredPlugins.size();
    for( size_t ind = 0 ; ind < m_discoveredPlugins.size() ; ++ ind) {
        qDebug() << "  " << ind << m_discoveredPlugins[ind].name;
    }

    // build a dependency to index lookup
    std::map< QString, int > dep2ind;
    for( size_t i = 0 ; i < m_discoveredPlugins.size() ; i ++ ) {
        PluginInfo & pInfo = m_discoveredPlugins[i];
        dep2ind[ pInfo.name] = i;
    }

    // figure out loading dependencies
    Algorithms::Graphs::TopoSort tsort( m_discoveredPlugins.size());

    // for every native plugin, insert arrows into toposort graph for every dependency
    // it has
    for( size_t i = 0 ; i < m_discoveredPlugins.size() ; i ++ ) {
        PluginInfo & pInfo = m_discoveredPlugins[i];
        // skip plugins that already have errors
        if( ! pInfo.errors.empty()) {
            continue;
        }
        // skip non-native plugins
        if( pInfo.typeString != "c++" && pInfo.typeString != "lib") {
            continue;
        }
        // process dependencies
        for( QString & dep : pInfo.depends) {
            // convert dependency to index...
            auto it = dep2ind.find( dep);
            if( it == dep2ind.end()) {
                pInfo.errors << "Cannot satisfy dependency '" + dep + "'";
                qDebug() << "Cannot find dependency" << pInfo.name << "/" << dep;
                break;
            }
            else {
                tsort.addArrow( it-> second, i);
                qDebug() << "adding arrow " << i << it-> second;
            }
        }
    }

    // figure out the order
    qDebug() << "toposort";
    auto loadingOrder = tsort.compute();

    // now try to load the plugins in this order
    if( loadingOrder.size() != m_discoveredPlugins.size()) {
        // could not figure out order, must have a loop
        qCritical() << "Could not figure out loading order, must have a dependency loop!";
    }
    else {
        qDebug() << "Loading order:";
        for( auto & ind : loadingOrder) {
            qDebug() << "  " << ind << m_discoveredPlugins[ind].name;
        }
        for( auto & ind : loadingOrder) {
            loadNativePlugin( m_discoveredPlugins[ind]);
        }
    }

    // filter out native plugins & non-native plugins
//    decltype(m_discoveredPlugins) nativePlugins, foreignPlugins;
//    for( auto & pInfo : m_discoveredPlugins) {
//        if( pInfo.typeString == "c++" || pInfo.typeString == "lib") {
//            nativePlugins.push_back( pInfo);
//        }
//        else {
//            foreignPlugins.push_back( pInfo);
//        }
//    }

//    qDebug() << "Native plugins found:" << nativePlugins.size();

//    // for all c++ and lib plugins, figure out dependencies and topologically order
//    // them for loading
//    std::vector<int> nativeLoadingOrder;
//    {
//        // TODO: do the actual dependency ordering
//        // for now the order is the same as discovery
//        for( size_t i = 0 ; i < nativePlugins.size() ; i ++ ) {
//            nativeLoadingOrder.push_back( i);
//        }
//    }

//    // now load all c++ and lib plugins in the determined order
//    for( auto ind : nativeLoadingOrder) {
//        qDebug() << "Loading plugin #" << ind;
////        loadNativePlugin( na)
//    }



    // now load user installed plugins
  /*  for( auto dirPath : m_pluginSearchPaths) {
        qDebug() << "Looking for plugins in:" << dirPath;
        QDir dir( dirPath);
        if( ! dir.exists()) {
            qWarning() << "Skipping non-existant plugin directory:" << dirPath;
            continue;
        }
        QDirIterator dit( dir.absolutePath(), QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
        while (dit.hasNext()) {
            dit.next();
            if( ! dit.fileInfo().isFile()) continue;
            if( ! dit.fileInfo().fileName().endsWith( ".so")) continue;
            auto absoluteFilePath = dit.fileInfo().absoluteFilePath();
            qDebug() << "trying " << absoluteFilePath;
            QPluginLoader loader( absoluteFilePath);
            QObject * plugin = loader.instance();
            if( plugin) {
                processLoadedCppPluginOld( plugin, absoluteFilePath);
            } else {
                qDebug() << "QPluginLoader error = " << loader.errorString();
                // QPluginLoader is not very verbose with error messages, so let's see
                // if we can get QLibrary get us more detailed message
                QLibrary lib( absoluteFilePath);
                if( ! lib.load()) {
                    qDebug() << "QLibrary error:" << lib.errorString();
                } else {
                    qDebug() << "QLibrary loaded the file fine";
                }

            }
        }
    }
*/

}

//const std::vector<PluginManager::PluginInfo *> & PluginManager::getInfoList()
//{
//    return m_allLoadedPlugins;
//}

const std::vector<PluginManager::PluginInfo> & PluginManager::getInfoList()
{
    return m_discoveredPlugins;
}

std::vector<PluginManager::PluginInfo> PluginManager::findAllPlugins()
{
    qDebug() << "Looking for plugins...";

    std::vector<PluginManager::PluginInfo> list;

    for( auto dirPath : m_pluginSearchPaths) {
        qDebug() << "  processing path:" << dirPath;
        QDir dir( dirPath);
        if( ! dir.exists()) {
            qWarning() << "  Skipping non-existant plugin directory";
            continue;
        }
        QDirIterator dit( dir.absolutePath(), QDirIterator::FollowSymlinks);
        while (dit.hasNext()) {
            dit.next();
            // skip "." and ".." entries
            if( dit.fileName() == "." || dit.fileName() == "..") {
                continue;
            }
            // skip non-directories
            if( ! dit.fileInfo().isDir()) {
                continue;
            }
            qDebug() << "    examining:" << dit.fileInfo().fileName();
            PluginInfo info = parsePluginDir( dit.filePath());
            if( ! info.errors.empty()) {
                qWarning() << "Could not load plugin from:" << dit.filePath()
                           << "\n  - reason: " << info.errors.join("\n")
                           << "\n================================";
            }
            else {
                list.push_back( info);
            }
        }
    }

    qDebug() << "Done looking for plugins. Found: " << list.size();
    return list;

}

PluginManager::PluginInfo PluginManager::parsePluginDir(const QString & dirName)
{
    PluginInfo info;
    info.dirPath = dirName;

    // to avoid duplication of parse error message:
    info.errors << "Could not parse plugin in: " + dirName;

    // try to open the json file
    QString jsonFname = dirName + "/plugin.json";
    QFile file( jsonFname);
    if( ! file.open( QFile::ReadOnly)) {
        info.errors << "...because: could not open: plugin.json";
        info.errors << "...because: " + file.errorString();
        return info;
    }

    // read in the contents of the file
    auto fileContents = file.readAll();

    // parse json
    QJsonParseError jsonErrors;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( fileContents, & jsonErrors);
    if( jsonDoc.isNull()) {
        info.errors << "...because could not parse" + jsonFname;
        info.errors << "...because: " + jsonErrors.errorString();
        return info;
    }
    QJsonObject json = jsonDoc.object();

    info.name = json["name"].toString();
    if( info.name.isNull()) {
        info.errors << "...'name' was not specified in plugin.json";
        return info;
    }
    info.version = json["version"].toString().toLower().trimmed();
    info.typeString = json["type"].toString().toLower().trimmed();
    if( info.typeString.isNull()) {
        info.errors << "...'type' was not specified in plugin.json";
        return info;
    }
    info.description = json["description"].toString();
    info.about = json["about"].toString();
    if( ! json["depends"].isArray()) {
        info.errors << "...'depends' must be an array of strings in plugin.json";
        info.errors << QJsonDocument( json).toJson();
        return info;
    }

    // process "depends" list
    {
        auto jsonArray = json["depends"].toArray();
        for( auto entry : jsonArray) {
            QString dep = entry.toString().trimmed();
            if( dep.isNull()) {
                info.errors << "...null dependency in plugin.json?";
                return info;
            }
            info.depends.append( dep);
        }
    }

    // if the plugin type is c++, make sure the plugin has .so file
    if( info.typeString == "c++") {
        QFileInfo soInfo( dirName + "/libplugin.so");
        if( ! soInfo.exists()) {
            info.errors << "...c++ plugin must have libplugin.so";
            return info;
        }
        if( ! soInfo.isExecutable()) {
            info.errors << "...c++ plugin must have executable libplugin.so";
            return info;
        }
        info.soPath = soInfo.filePath();
    }

    // if the plugin type is c++ or lib, find all libraries under libs subdirectory
    if( info.typeString == "c++" || info.typeString == "lib") {
        qDebug() << "Looking for libs..."<<dirName+"/libs";
        QDirIterator dit(
                    dirName + "/libs");
                    //{ "*.so", "*.so.*" },
                    //QDir::Files | QDir::Executable,
                    //QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
        while( dit.hasNext()) {
            dit.next();
            qDebug() << "...found:" << dit.fileInfo().fileName();
            qDebug() << "......fullPath:" << dit.filePath();
            info.libPaths.append( dit.filePath());
        }
    }

    // if we got this far, clear the errors and return the parsed info
    info.errors.clear();
    return info;
}

void PluginManager::loadNativePlugin(PluginManager::PluginInfo & pInfo)
{
    // skip non-native plugins
    if( pInfo.typeString != "c++" && pInfo.typeString != "lib" ) {
        return;
    }

    // skip plugins with errors
    if( ! pInfo.errors.empty()) {
    	qDebug() << "Skipping load of plugin="<<pInfo.name<<" because it has errors";
        return;
    }

    qDebug() << "Trying to load plugin" << pInfo.name;

    // if the plugin has libs, try to load them in heuristically
    std::vector<int> libsToLoad;
    for( int i = 0 ; i < pInfo.libPaths.size() ; i ++ ) {
        libsToLoad.push_back( i);
    }

    qDebug() << "  - plugin has" << pInfo.libPaths.size() << " libraries, trying to load them";
    while( ! libsToLoad.empty()) {
        qDebug() << "  - heuristic loop start with" << libsToLoad.size() << " remaining";
        // remember the old size
        auto oldSize = libsToLoad.size();

        // see if we can load any of the remainging libraries, the ones
        // that cannot be loaded are kept on the list
        auto listCopy = libsToLoad;
        libsToLoad.clear();
        qDebug() << "listCopy size="<<listCopy.size();
        for( auto ind : listCopy) {
            auto libPath = pInfo.libPaths[ ind];
            qDebug() << "LibPath="<<libPath;
            qDebug() << "    " + QFileInfo( libPath).fileName();
            QLibrary lib( libPath);
            if( ! lib.load()) {
                qDebug() << "      error:" + lib.errorString();
                libsToLoad.push_back( ind);
            } else {
                qDebug() << "      success";
            }
        }

        // if we made progress, continue
        if( oldSize > libsToLoad.size()) {
            continue;
        }
        else {
            // otherwise there is not need to continue
            break;
        }
    }

    if( ! libsToLoad.empty()) {
        QStringList msg;
        msg << "Could not make progress with loading libraries:";
        for( auto lib : libsToLoad) {
            msg << "  " + pInfo.libPaths[lib];
        }
        qCritical() << msg.join( "\n");
        pInfo.errors << "Could not load libraries";
        return;
    }

    // now only continue with c++ plugins
    if( pInfo.typeString != "c++") {
        return;
    }

    // for cpp plugins, try to load in the actual plugin shared library
    qDebug() << "pinfo.soPath="<<pInfo.soPath;
    QPluginLoader loader( pInfo.soPath);
    QObject * plugin = loader.instance();
    if( ! plugin) {
        qDebug() << "QPluginLoader error = " << loader.errorString();
        pInfo.errors << "QPluginLoader error: " + loader.errorString();
        // QPluginLoader is not very verbose with error messages, so let's see
        // if we can get QLibrary get us more detailed message
        QLibrary lib( pInfo.soPath);
        if( ! lib.load()) {
            qDebug() << "QLibrary error:" << lib.errorString();
            pInfo.errors << "QLibrary error:" + lib.errorString();
        } else {
            // nope, no extra insight from QLibrary
        }
        return;
    }

    // try to cast the loaded qobject to our carta plugin interface
    IPlugin * cartaPlugin = qobject_cast<IPlugin *>( plugin);
    if( ! cartaPlugin) {
        // not a carta plugin, ignore it
        qDebug() << pInfo.name << ": not a carta plugin";
        pInfo.errors << "not a CARTA plugin";
        return;
    }

    // add info about this plugin to our list
    pInfo.rawPlugin = cartaPlugin;
//    m_allLoadedPlugins.push_back( & pInfo);

    // find out what hooks this plugin wants to listen to
    auto hooks = cartaPlugin-> getInitialHookList();

    // for each hook the plugin wants to listen to, add it to the appropriate
    // lookup slot in m_hook2plugin
    for( auto id : hooks) {
        m_hook2plugin[id].push_back( & pInfo);
    }

}



/// process the loaded CPP plugin
/*void PluginManager::processLoadedCppPluginOld(QObject *plugin, QString path)
{
    IPlugin * cartaPlugin = qobject_cast<IPlugin *>( plugin);
    if( ! cartaPlugin) {
        // not a carta plugin, ignore it
        qDebug() << "not a carta plugin";
        return;
    }

    qDebug() << "yup, a carta plugin" << plugin;

    // add info about this plugin to our list
    PluginInfo * info = new PluginInfo;
    info-> rawPlugin = cartaPlugin;
    info-> soPath = path;
    info-> name = QFileInfo( path).baseName();
    m_allLoadedPlugins.push_back( info);

    // find out what hooks this plugin wants to listen to
    auto hooks = cartaPlugin-> getInitialHookList();

    // for each hook the plugin wants to listen to, add it to the appropriate
    // lookup slot in m_hook2plugin
    for( auto id : hooks) {
        m_hook2plugin[id].push_back( info);
    }
}


// API testing

void fakeMain()
{
    PluginManager pm;

    QString filename = "file.json";
    QImage testImage;

    // configure the manager from a filename
    // JSON or INI format?
    pm.loadConfig( filename);

    // load all plugins
    pm.loadPlugins();

    // execute a hook that calls all plugins, with some parameters, and a return type
    // the result is an array of the results
    //    std::vector<QImage> images = pm.hookAll<Render>(8, "Hello", testImage);

    // get info about all plugins
    const std::vector<PluginManager::PluginInfo *> & infoList = pm.getInfoList();
    for( const auto & entry : infoList) {
        // print out info about each plugin
        // ...
        []( decltype(entry) ) {};
    }

    // execute a hook that calls all plugins, with no parameters and no return type
    pm.prepare<Initialize>();

    // execute a hook that calls all plugins, with some parameters and no return type
    //    pm.hookAll<Render>( 8, "Hello", testImage);

    // execute a hook that calls first plugin that answers and returns the result
    //    auto res2 = pm.hookFirst<GetRandomNumber>();

    // execute a hook with some parameters and a result, and execute a closure
    // for each result
    auto helper = pm.prepare<PreRender>( "Test", & testImage);
    helper.forEachCond( [] (PreRender::ResultType) -> bool { return true; });

    //    pm.hookAll2<Render>( 3, "Test", testImage).forEach(
    //                [] ( const Render::ResultType & )
    //                );



}*/




