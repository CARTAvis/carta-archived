#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include "FileBrowserService.h"
#include "PureWeb.h"

FileBrowserService::FileBrowserService( QObject * parent, QString rootDir)
    : QObject( parent)
{
    if( rootDir.isNull ()) {
        // TODO: centralize the place for locating the ini file
        QSettings qs( QDir::homePath () + "/.sfviewer.config", QSettings::IniFormat);
        qs.beginGroup ( "fileBrowser");
        rootDir = qs.value ( "rootDir", "/n/viz/demos").toString ();
    }
    m_rootDir = rootDir;
    dbg(1) << "Root dir = " << m_rootDir << "\n";

    GetStateManager().CommandManager().AddUiHandler(
                "FileBrowser", CSI::Bind(this, & FileBrowserService::onPureWebCmd));

}

//void FileBrowserService::processCommand(CSI::Typeless command)
//{
////    std::string cmd = command["/command"].As<std::string>();
////    dbg(1) << "Processing command " << cmd << "\n";

////    if( cmd == "reset") {
////        m_currDir = QDir( "/n/ras/cyberska/");

////    } else {
////        dbg(1) << "Unknown command sent to file browser: " << cmd << "\n";
////    }
//}

std::string FileBrowserService::type2str( EntryType t)
{
    if( t == Reset) return "reset";
    if( t == Directory) return "dir";
    if( t == FileFits) return "fits";
    if( t == FileIQU) return "iqu";
    if( t == FileCasa) return "casa";
    if( t == FileUnknown) return "file";
    return "err";
}

void
FileBrowserService::reset (Instance &instance)
{
    dbg(1) << "Resetting instance " << instance.id
           << " cd = " << instance.currDir << "\n";
    instance.currDir = m_rootDir;
    instance.entries.clear ();
    instance.pathState.clear ();
    instance.pathStateIndex = 0;

    // update the state for clients
    sendState ( instance);

}

void FileBrowserService::sendState(FileBrowserService::Instance &instance)
{
    QDir dir( instance.currDir);
//    dir.setPath ( instance.currDir);
    instance.entries.clear ();
    QFileInfoList lst = dir.entryInfoList ( QDir::AllEntries | QDir::NoDotAndDotDot);
    for( int i = 0 ; i < lst.length () ; i ++ ) {
        QFileInfo & inf = lst[i];
        if( ! inf.isReadable ()) continue;
        Entry e;
        e.fname = inf.fileName ();
        e.description = "Some entry.";
        if( inf.isDir ()) e.type = Directory;
        else if( inf.suffix ().toLower () == "fits") e.type = FileFits;
        else if( inf.suffix ().toLower () == "iqu") e.type = FileIQU;
        else e.type = FileUnknown;
        e.size = inf.size ();
        if( e.type == Directory) e.size = 0;
        instance.entries.push_back ( e);
    }

    // update the current state number
    instance.stateNumber ++;

    // output the state
    std::ostringstream out;

    // first entry in state is the state number
    out << instance.stateNumber << "|";

    // next entry is the path state
    out << instance.pathStateIndex;
    for( int i = 0 ; i < instance.pathState.size () ; i ++)
        out << " " << instance.pathState[i].toAscii ().toBase64 ().data ();
    out << "|";

    // the rest are the directory entries
    for( size_t i = 0 ; i < instance.entries.size () ; i ++ ) {
        Entry & e = instance.entries[i];
        QString fname64 = e.fname.toAscii ().toBase64 ().data ();
        QString description64 = e.description.toAscii ().toBase64 ().data ();
        out << type2str(e.type) << " "
            << fname64 << " "
            << description64 << " "
            << e.size;
        if( i < instance.entries.size () -1) out << "|";
    }

    // send the state to the clients
    dbg(1) << "sending state: " << out.str () << "\n";

    QString keyName = "/FileBrowser/" + instance.id + "/list";
    dbg(1) << "key = " << keyName.toStdString () << "\n";
//    GetStateManager ().XmlStateManager ().SetValue (
//                keyName.toStdString(),
//                out.str());

    pwset( keyName, out.str());
}


void
FileBrowserService::onPureWebCmd (
        CSI::Guid /*sessionid*/, CSI::Typeless commandArray, CSI::Typeless /*responses*/)
{
    dbg(1) << "OnFileBrowser...\n";

//    s_dataloaded = true;
//    std::string name = commandArray["/name"].As<std::string>();
    QString name = commandArray["/name"].As<QString>();

    Instance & instance = m_instances[name];
    if( instance.id != name) {
        dbg(1) << "Creating new instance " << name << "\n";
        instance.id = name;
        instance.currDir = m_rootDir;
    }

    std::string command = commandArray["/command"].As<std::string>();

    dbg(1) << "name = " << name
           << " cmd = " << command
           << "\n";

    // reset command is handled regardless of state number
    if( command == "reset") {
        reset( instance);
        return;
    }

    // get the state number - the rest of the commands must have a matching state number
    qint64 sn = commandArray["/stateNumber"].ValueOr<qint64>( -1);
    if( sn != instance.stateNumber) {
        dbg(1) << ConsoleColors::cyanBg () << ConsoleColors::warning ()
               << "state number mismatch " << sn << " vs " << instance.stateNumber
               << ConsoleColors::resetln ();
        return;
    }

    if( command == "entry") {
        int entryIndex = commandArray["/index"].ValueOr(-1);
        if( entryIndex < 0 || entryIndex > int(instance.entries.size ())-1) {
            dbg(1) << ConsoleColors::cyanBg () << ConsoleColors::warning ()
                   << "invalid entry index " << entryIndex
                   << ConsoleColors::resetln ();
        }
        Entry & e = instance.entries[entryIndex];
        if( e.type == Directory) {
            instance.currDir += "/" + e.fname;
            while( instance.pathState.size () > instance.pathStateIndex)
                instance.pathState.removeLast ();
            instance.pathState.push_back ( e.fname);
            instance.pathStateIndex = instance.pathState.length ();
            sendState ( instance);
            return;
        }

        emit openFile ( instance.currDir + "/" + instance.entries[entryIndex].fname);
        return;
    }

    if( command == "path") {
        int ind = commandArray["/index"].ValueOr(-1);
        if( ind < 0 || ind > instance.pathState.size ()) {
            dbg(1) << ConsoleColors::cyanBg () << ConsoleColors::warning ()
                   << "invalid path index " << ind
                   << ConsoleColors::resetln ();
        }
        instance.pathStateIndex = ind;
        instance.currDir = m_rootDir;
        for( int i = 0 ; i < ind ; i ++) {
            instance.currDir += "/" + instance.pathState[i];
        }
        sendState ( instance);
        return;
    }

    dbg(1) << ConsoleColors::cyanBg () << ConsoleColors::warning ()
           << "Invalid command " << command
           << ConsoleColors::resetln ();
}

/*

  State format

  <state_number> | <path state> | <entry1> | <entry2> | ... | <last_entry

  <state number> - increasing number

  <path state> format:
     current_id " " part1-base-64 " " part2-base-64 " " ... " " last_part-base-64

  <entry> format:
     type " " fname-base-64 " " description-base-64 " " size


  Command format
     command : "reset" or "path" or "entry"
     stateNumber : state number


  */


