#include "RaiCache.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <typeinfo>
#include <time.h>
#include <fcntl.h>

#include <QByteArray>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <stdexcept>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSettings>

#include "NFSSafe.h"
#include "AFile.h"


namespace RaiLib {

namespace Cache {

/// determines whether profiles should be cached for this file using some simple criteria
///
/// every client should call this before inserting/getting any information about this
/// file in the db
bool shouldCacheZprofile( const FitsParser::HeaderInfo & hdr )
{
    // TODO: find a better algorithm for determining the conditions for caching profiles

    // anything with depth < 2 needs no profile cache
    if( hdr.totalFrames < 2) return false;

    // file of less than 1G should not be cached, because the whole file probably fits
    // in the system cache, and random access will not be an issue
    if( double(hdr.naxis1) * hdr.naxis2 * hdr.naxis3 * hdr.bitpixSize < 1e8) return false;

    // anything else should be cached...
    return true;
}

/// converts a path to a unique valid filename
QString manglePath( const QString & path)
{
    return path.toAscii ().toPercentEncoding ( "", "/", '+');

    // replace all '_' with '__'
    QString res = path;
    res.replace ( '_', "__");
    // replace all '/' with '_^'
    res.replace ( '/', "_^");

    return res;
}

/// unmangles the path
QString deManglePath( const QString & path)
{
    return QByteArray::fromPercentEncoding ( path.toAscii (), '+');
    QString res = path;

    // undo replace all '/' with '_^'
    res.replace ( "_^", "/");

    // undo replace all '_' with '__'
    res.replace ( "__", "_");

    return res;
}

/// get a prefix for the given path
QString getUniquePrefix( const QString & p)
{
    QByteArray a = p.toAscii ();
    quint16 n = qChecksum ( a.constData (), a.length ());
    return QByteArray( (char*)(&n), 2).toHex ();
}

//
// some functions for reading info from files
//

/// templated read for simple types
template <typename T>
inline void read_simple ( QFile & fp, T & val)
{
    if( sizeof(T) != fp.read( reinterpret_cast<char *>(& val), sizeof( T)))
    {
        std::string msg = "Could not read type <T> ";
        msg += std::string(typeid(T).name());
        msg += " error code = " + int(fp.error ());
        msg += " " + fp.errorString ().toStdString();
        msg += " while reading from file " + fp.fileName ().toStdString ();
        LTHROW( msg);
//        LTHROW( "Could not read type <T> " + std::string(typeid(T).name())
//                + " error code = " + fp.error () + " " + fp.errorString ().toStdString ());
//        throw std::runtime_error( "Could not read type <T> " + std::string(typeid(T).name()));
    }
}

/// templated write for simple types
template <typename T>
inline void write_simple ( QFile & fp, const T & val)
{
    if( sizeof(T) != fp.write( reinterpret_cast<const char *>(& val), sizeof( T)))
    {
        LTHROW( "Could not write type <T> " + std::string(typeid(T).name()));
//        throw std::runtime_error( "Could not write type <T> " + std::string(typeid(T).name()));
    }
}

/// templated write for simple types
template <typename T>
inline void write_simple ( AFile & fp, const T & val)
{
    if( sizeof(T) != fp.write( reinterpret_cast<const char *>(& val), sizeof( T)))
    {
        LTHROW( "Could not write type <T> " + std::string(typeid(T).name()));
//        throw std::runtime_error( "Could not write type <T> " + std::string(typeid(T).name()));
    }
}


/// Read HistogramInfo from an open file.
/// Throws exception if failed.
/// @todo optimize for a single read()
void read_HistogramInfo ( QFile & fp, HistogramInfo & h)
{
    read_simple( fp, h.min);
    read_simple( fp, h.min95);
    read_simple( fp, h.min98);
    read_simple( fp, h.min99);
    read_simple( fp, h.min995);
    read_simple( fp, h.min999);
    read_simple( fp, h.min9999);
    read_simple( fp, h.max);
    read_simple( fp, h.max95);
    read_simple( fp, h.max98);
    read_simple( fp, h.max99);
    read_simple( fp, h.max995);
    read_simple( fp, h.max999);
    read_simple( fp, h.max9999);
    read_simple( fp, h.valid);
}

/// Writes HistogramInfo to an open file.
/// Throws exception if failed.
/// @todo optimize for a single write()
void write_HistogramInfo ( QFile & fp, const HistogramInfo & h)
{
    write_simple( fp, h.min);
    write_simple( fp, h.min95);
    write_simple( fp, h.min98);
    write_simple( fp, h.min99);
    write_simple( fp, h.min995);
    write_simple( fp, h.min999);
    write_simple( fp, h.min9999);
    write_simple( fp, h.max);
    write_simple( fp, h.max95);
    write_simple( fp, h.max98);
    write_simple( fp, h.max99);
    write_simple( fp, h.max995);
    write_simple( fp, h.max999);
    write_simple( fp, h.max9999);
    write_simple( fp, h.valid);
}

void write_HistogramInfo ( AFile & fp, const HistogramInfo & h)
{
    write_simple( fp, h.min);
    write_simple( fp, h.min95);
    write_simple( fp, h.min98);
    write_simple( fp, h.min99);
    write_simple( fp, h.min995);
    write_simple( fp, h.min999);
    write_simple( fp, h.min9999);
    write_simple( fp, h.max);
    write_simple( fp, h.max95);
    write_simple( fp, h.max98);
    write_simple( fp, h.max99);
    write_simple( fp, h.max995);
    write_simple( fp, h.max999);
    write_simple( fp, h.max9999);
    write_simple( fp, h.valid);
}


/// Writes QByteArray to an open file.
/// Throws exception if failed.
void write_QByteArray ( QFile & fp, const QByteArray & a)
{
    if( fp.write( a) != a.length ())
        throw std::runtime_error( "Could not write zprofile");
}

/// Writes ByteBlock to an open file.
/// Throws exception if failed.
void write_ByteBlock ( QFile & fp, const ByteBlock & a)
{
    if( fp.write( & a[0], a.size ()) != qint64(a.size ()))
        throw std::runtime_error( "Could not write zprofile");
}


///////////////////////////////////////////////////////////////////////////////
// FileHandle
///////////////////////////////////////////////////////////////////////////////
FileHandle::FileHandle()
    : m_valid( false)
    , m_width( -1)
    , m_height( -1)
    , m_nframes( -1)
    , m_bitpixsize( -1)
    , m_fullProfileFp( new AFile())
{
}

FileHandle::FileHandle(const FitsFileLocation & floc)
    : m_valid( false)
    , m_floc( floc)
    , m_width( -1)
    , m_height( -1)
    , m_nframes( -1)
    , m_bitpixsize( -1)
    , m_fullProfileFp( new AFile())
{
}

FileHandle::~FileHandle()
{
}

qint64 FileHandle::width () const
{
    return m_width;
}

qint64 FileHandle::height () const
{
    return m_height;
}


qint64 FileHandle::nframes () const
{
    return m_nframes;
}

int FileHandle::bitpixsize () const
{
    return m_bitpixsize;
}


bool FileHandle::valid () const
{
    return m_valid;
}

const FitsFileLocation & FileHandle::floc() const
{
    return m_floc;
}

QString FileHandle::str () const
{
    return QString("FileHandle(prefix:%1,dir:%2,valid:%3,dims:%4x%5x%6/%7,floc:%8)")
            .arg(m_prefix)
            .arg(m_dirName)
            .arg(valid ())
            .arg(width ()).arg(height ()).arg(nframes ()).arg(bitpixsize ())
            .arg(floc ().uniqueId ());
}

///////////////////////////////////////////////////////////////////////////////
// FileInfo
///////////////////////////////////////////////////////////////////////////////
std::string FileInfo::str () const
{
    std::string stdstr = hinfo.str();
    return QString("FileInfo(valid:%1,nFrames:%2,hinfo:%3)")
            .arg(valid ()).arg(nFrames).arg(stdstr.c_str()).toStdString ();
}

///////////////////////////////////////////////////////////////////////////////
// FrameInfo
///////////////////////////////////////////////////////////////////////////////
std::string FrameInfo::str () const
{
    std::string stdstr = hinfo.str();
    return QString("FrameInfo(valid:%1,hinfo:%2)")
            .arg(valid ()).arg(stdstr.c_str()).toStdString ();
}

//////////////////////////////////////////////////////////////////////////////
// Controller
///////////////////////////////////////////////////////////////////////////////
class Controller::Implementation {
public:
    Implementation( const InitParameters & init);

    const InitParameters & getInitParameters();


    /// constants
    QString rootPath() { return m_rootDir; }

    /// Retrieves a file handle from the cache for a given file.
    /// If it does not exist, the returned handle will be invalid.
    FileHandle getFileHandle( const FitsFileLocation & floc);
    /// Retrieves a file handle from the cache for a given file.
    /// If it does not exist, an entry is created for the file.
    /// If the entry cannot be created, an invalid handle is returned.
    FileHandle createFileHandle( const FitsFileLocation & floc,
                                 qint64 width, qint64 height, qint64 nframes,
                                 int bitpixsize);

    /// will retrive info about a file
    FileInfo getFileInfo ( const FileHandle &);
    /// stores info about a file
    bool setFileInfo( const FileHandle &, const FileInfo &);

    /// Retrieve a z-profile.
    /// Empty result indicates there was no profile.
    ByteBlock getRawZprofile( const FileHandle &, int x, int y);
    bool getRawZprofileExist( const FileHandle &, int x, int y);
    void getZprofileMap(
            const FileHandle &,
            std::vector< std::vector< bool > > & map);
    ByteBlock getZprofileMap( const FileHandle &);
    /// Store a z-profile.
    bool setRawZprofile( const FileHandle &, int x, int y, const ByteBlock &);
    bool setRawZprofileBatch( FileHandle & h, qint64 pos, const ByteBlock & );

    /// retrieve information about a frame
    FrameInfo getFrameInfo( const FileHandle &, qint64 frame);

    /// get a map of frames (completed frames)
    std::vector<bool> getFrameMap( FileHandle &);

    /// get all frames (possibly faster than get 1 frame at a time)
    /// @todo not implemented yet
    std::vector<FrameInfo> getAllFrames( const FileHandle &);

    /// attempt to optimize the cache entry (i.e. if all information is present
    /// in the partial files, it is moved to final files, to speed up read access)
    void optimize( FileHandle &);

    /// get some information about the status of the file
    QStringList status( FileHandle &);


    /// store frame info
    bool setFrameInfo ( const FileHandle &, qint64 frame, const FrameInfo &);

    /// list all files (debugging)
    void listAllFiles( std::ostream & out );

    /// convenience functio to return the path into the handle's directory
    QString path( const FileHandle & handle, const QString & fname = QString());

protected:
    /// cached version of prepdb
    bool prepDB( bool forced = false);
    /// creates the directory structure for storing the cache
    bool prepDBraw();
    /// returns whether the db is operational
    bool isWorking();

    int getStoredVersion();

    /// current version of the database
    static const int CurrentVersion = 4;

    /// root of the db
    QString m_rootDir;

    /// did we run prepdb
    bool prepDbWasRunBefore;
    /// if dbPrep was run before, what was it's result
    bool prepDbCachedResult;
    /// mutex guarding prepdb
    QMutex prepDbMutex;

    /// copy of the init parameters
    InitParameters m_initParameters;
};

/// Retrieves a file handle from the cache for a given file.
/// If it does not exist, the returned handle will be invalid.
FileHandle
Controller
::getFileHandle( const FitsFileLocation & floc)
{
    return impl_-> getFileHandle ( floc);
}
FileHandle
Controller::Implementation
::getFileHandle( const FitsFileLocation & floc)
{
    try {
        if( ! isWorking()) return FileHandle();
        FileHandle h;
        h.m_floc = floc;
        h.m_prefix = getUniquePrefix(floc.getLocalFname ());
        h.m_dirName = manglePath ( floc.getLocalFname ());
        QFile dfp( path(h, "dimensions"));
        if( ! dfp.open( QFile::ReadOnly)) return h;
        read_simple( dfp, h.m_width);
        read_simple( dfp, h.m_height);
        read_simple( dfp, h.m_nframes);
        read_simple( dfp, h.m_bitpixsize);
        dfp.close();
        QString fullPath = path( h);
        h.m_valid = QFileInfo( fullPath).isDir ();
        return h;
    }
    catch(...) {
        dbg(1) << "getFileHandle exception\n";
    }

    // return an invalid handle
    return FileHandle();
}


/// Creates/retrieves a file handle from the cache for a given file.
/// If it does not exist, an entry is created for the file.
/// If the entry cannot be created, an invalid handle is returned.
FileHandle
Controller::
createFileHandle(
        const FitsFileLocation & floc,
        qint64 width, qint64 height, qint64 nframes, int bitpixsize)
{
    return impl_-> createFileHandle ( floc, width, height, nframes, bitpixsize);
}
FileHandle
Controller::Implementation::
createFileHandle(const FitsFileLocation &floc,
                 qint64 width, qint64 height, qint64 nframes, int bitpixsize)
{
    try {
        if( ! isWorking()) return FileHandle();

        FileHandle h = getFileHandle ( floc);
        if( h.valid ()) return h;

        QString dirName = path(h);
        QString tmpDirName = dirName + NFSSafe::getUniqueString ();
        QDir::current ().mkpath ( tmpDirName);
        NFSSafe::makeEmptyFile ( tmpDirName + "/frames.index", nframes);
        NFSSafe::makeEmptyFile ( tmpDirName + "/frames.partial", HistogramInfo::BinSize * nframes);
        NFSSafe::makeEmptyFile ( tmpDirName + "/zprofiles.index", width * height);
        NFSSafe::makeEmptyFile ( tmpDirName + "/zprofiles.partial", width * height * nframes * bitpixsize);
        QFile fp( tmpDirName + "/dimensions");
        fp.open( QFile::ReadWrite);
        write_simple( fp, width);
        write_simple( fp, height);
        write_simple( fp, nframes);
        write_simple( fp, bitpixsize);
        fp.close();


        // now put the temporary directory in place
        if( ! QFile::rename ( tmpDirName, dirName)) {
            // clean up, someone beat us to it
            QFile::remove ( tmpDirName + "/frames.index" );
            QFile::remove ( tmpDirName + "/frames.partial" );
            QFile::remove ( tmpDirName + "/zprofiles.index" );
            QFile::remove ( tmpDirName + "/zprofiles.partial" );
            QFile::remove ( tmpDirName + "/dimensions" );
            QDir::current ().rmdir ( tmpDirName);
            return getFileHandle ( floc);
        }

        return getFileHandle ( floc);

    } catch ( ... ) {
        dbg(1) << "Exception in createFileHandle()\n";
    }

    // return an invalid file handle
    return FileHandle();

}


Controller::InitParameters
Controller::InitParameters::fromIniFile (
        const QString & fname,
        const QString & section)
{
    QSettings s( fname, QSettings::IniFormat);
    if( ! section.isNull ())
        s.beginGroup ( section);
    QString dir = s.value ( "dir").toString ();
    QString cacheName = s.value ( "cacheName").toString ();

    if( dir.isNull () || cacheName.isNull ())
        fatalError ( "Cannot read cache information from " + fname + " section [" + section + "]");

    InitParameters init;
    if( ! dir.isNull ())
        init.setDir ( dir);
    if( ! cacheName.isNull ())
        init.setCacheName ( cacheName);
    return init;
}



Controller::
Controller(const InitParameters & init)
    : impl_( new Implementation( init))
{
}

Controller::Implementation::
Implementation(const InitParameters & init)
{
    prepDbWasRunBefore = false;
    prepDbCachedResult = false;
    m_initParameters = init;

    m_rootDir = init.m_dir + "/" + init.m_cacheName;

    bool prp = prepDB();
    if( ! prp) {
        dbg(1) << ConsoleColors::warning()
               << "PrepDB() failed"
               << ConsoleColors::resetln();
    }
//    if( ! prepDB ())
//        throw std::runtime_error( ("Could not prep db in " + rootPath ()).toStdString());
}

const Controller::InitParameters &
Controller::
getInitParameters ()
{
    return impl_-> getInitParameters ();
}

const Controller::InitParameters &
Controller::Implementation::
getInitParameters ()
{
    return m_initParameters;
}


// retrieves the version of the cache in the database
int Controller::Implementation::getStoredVersion()
{
    QFile fp( rootPath () + "/version");
    if( fp.open( QFile::ReadOnly)) {
        QString line = fp.readLine (100);
        bool ok; int v;
        v = line.toInt ( & ok);
        if( ok) return v;
        return -1;
    }

    return -1;
}

// cached version of prepdb
// assumes the server/table names don't change
bool Controller::Implementation::prepDB(bool forced)
{
    QMutexLocker locker( & prepDbMutex);

    dbg(1) << "dbPrepped = " << prepDbWasRunBefore << "\n";
    dbg(1) << "cachedDbPrepResult = " << prepDbCachedResult << "\n";

    // if we ran prepDb before, and forced is not set, return the result
    if( prepDbWasRunBefore && ! forced) return prepDbCachedResult;

    // otherwise run the real prepdb and remember the result

    prepDbWasRunBefore = true;
    prepDbCachedResult = prepDBraw ();

    return prepDbCachedResult;

}

// returns whether the DB is operational
bool Controller::Implementation::isWorking() {
    return prepDbCachedResult;
}

// connect to db
// get the current version of the cache
// if the version is the current version, return true
// if the version is too high, return false
// if the version is too low or version does not exist, delete the cache
// create a cache & set the version
//
bool Controller::Implementation::prepDBraw()
{
    try {
        int storedVersion = getStoredVersion ();
        if( storedVersion == CurrentVersion) {
            return true;
        }
        if( storedVersion > CurrentVersion) {
            dbg(1) << "cache is too new, cannot use it\n";
            return false;
        }
        if( storedVersion > -1) {
            /// @todo implement automatic upgrade of the cache?
            dbg(1) << "cache is too old, cannot use it\n";
            return false;
        }

        // version was not found at all, it's possible someone else is prepping
        // the db, so let's call the safe mkdir (in case it's stored on NFS)
        // and only one process will be allowed to prep the db
        if( ! NFSSafe::safemkdir ( rootPath ())) {
            // we were not able to do safe mkdir, that means someone must be trying
            // prepdb, or the place where the root is supposed to be cannot be
            // written to (e.g. permissions, the path is invalid, not mounted, etc)
            // let's try to get the version again in couple of seconds, and if that fails
            // we report error
            dbg(1) << "safemkdir unsuccessful, retrying for 5 seconds\n";
            QTime timer; timer.restart ();
            while( timer.elapsed () < 5000) {
                qsleep ( 0.1);
                storedVersion = getStoredVersion ();
                if( storedVersion > -1) break;
            }
            if( storedVersion != CurrentVersion) {
                dbg(1) << "cache still has wrong version, giving up\n";
                return false;
            } else {
                dbg(1) << "ok, someone else prepped the cache\n";
                return true;
            }
        }

        // at this point we were the ones that were successful at creating the
        // root of the db, so we should prep it

        // write current version to version
        {
            QString versionFname = QString("%1/version").arg( rootPath ());
            QString versionFnameTmp = versionFname + NFSSafe::getUniqueString ();
            QFile vfp( versionFnameTmp);
            if( ! vfp.open( QFile::ReadWrite)) {
                dbg(1) << "Failed to write version to tmpfile " << versionFnameTmp << "\n";
                return false;
            }
            QTextStream vout( & vfp);
            vout << CurrentVersion << "\n";
            vout.flush ();
            if( ! vfp.flush ()) {
                dbg(1) << "Flush failed on tmpfile " << versionFnameTmp << "\n";
                vfp.remove ();
                return false;
            }
            vfp.close ();
            if( ! vfp.rename ( versionFname)) {
                dbg(1) << "Failed to rename " << versionFnameTmp << " to "
                       << versionFname << "\n";
                vfp.remove ();
                return false;
            }
            dbg(1) << "Version written\n";
        }

        return true;
    } catch(...) {
        dbg(1) << "Exception in prepDb\n";
    }

    return false;
}

Controller::~Controller()
{
    delete impl_;
}


FileInfo
Controller::
getFileInfo(
        const FileHandle & handle)
{
    return impl_-> getFileInfo ( handle);
}

FileInfo
Controller::Implementation::
getFileInfo(
        const FileHandle & handle)
{
    try {
        if( ! handle.valid ()) return FileInfo();

        // try to read the info from global.final
        QFile fp( rootPath () + "/" + handle.m_prefix + "/" + handle.m_dirName + "/" + "global.final");
        if( fp.open( QFile::ReadOnly)) {
            FileInfo info;
            read_simple ( fp, info.nFrames );
            read_HistogramInfo ( fp, info.hinfo );
            info.m_valid = true;
            return info;
        } else {
            return FileInfo();
        }
    }
    catch(...) {
        dbg(1) << "getFileInfo: exception";
        return FileInfo();
    }
}

bool
Controller::
setFileInfo(const FileHandle & h, const FileInfo & f)
{
    return impl_-> setFileInfo( h, f);
}

bool
Controller::Implementation::
setFileInfo(const FileHandle & handle, const FileInfo & finfo)
{
    try {
        // try to write the info to global.final, but through a temporary file,
        // with a rename at the end, since we are likely on NFS
        QString fullName = rootPath () + "/" + handle.m_prefix + "/" + handle.m_dirName + "/" + "global.final";
        // if the final file exists, don't do anything
        if( QFileInfo( fullName).exists ()) return true;

        // write the info to a tmp file
        QString tmpName = fullName + "." + NFSSafe::getUniqueString ();

        QFile fp( tmpName);
        if( fp.open( QFile::WriteOnly | QFile::Truncate )) {
            write_simple ( fp, finfo.nFrames );
            write_HistogramInfo ( fp, finfo.hinfo );
            fp.close();
            // try to rename, if not successful, it means someone else beat us to
            // it (most likely), so remove the temporary file instead
            if( ! fp.rename ( fullName))
                fp.remove ();
            // to catch some weird errors, return based on the existance of the
            // final file
            return QFileInfo( fullName).exists ();
        } else {
            dbg(1) << "Failed to write " << tmpName << "\n";
            return false;
        }
    } catch( ...) {
        dbg(1) << "Exception in setFileInfo\n";
    }

    return false;

}


bool
Controller::
setRawZprofile (const FileHandle &h, int x, int y, const ByteBlock &a)
{
    return impl_-> setRawZprofile( h, x, y, a);
}

bool
Controller::Implementation::
setRawZprofile (const FileHandle &handle, int x, int y, const ByteBlock &a)
{
    try {
        if( ! handle.valid ()) return false;
        if( qint64(a.size ()) != handle.depth())
            LTHROW( "handle.depth is not the same as z-profile size");
        if( x < 0 || x >= handle.width () || y < 0 || y >= handle.height ())
            LTHROW( "x,y out of range in set raw zprofile");
        quint64 ind = y * handle.width () + x;
        // if the final zprofiles file is already present, return
        if( handle.m_fullProfileFp-> isOpen ())
            return true;
        if( QFileInfo( path( handle, "zprofiles.final")).exists ())
            return true;
        // we have to save to a partial file
        QString indexFname = path( handle, "zprofiles.index");
        QString profilesFname = path( handle, "zprofiles.partial");

        QFile ifp( indexFname);
        if( ! ifp.open( QFile::ReadWrite)) return false;
        if( ! ifp.seek ( ind)) return false;
        char c;
        if( 1 != ifp.read( & c, 1)) return false;
        if( c != 0) return true; // already set
        QFile ffp( profilesFname);
        if( ! ffp.open( QFile::ReadWrite)) return false;
        if( ! ffp.seek( handle.depth () * ind)) return false;
        write_ByteBlock ( ffp, a);
        ffp.close ();
        // indicate in the index file that this profile is ready
        if( ! ifp.seek( ind)) return false;
        c = 1;
        write_simple<char>( ifp, c);
        ifp.close();

        return true;
    } catch( ...) {
        dbg(1) << "Exception in setRawZprofile\n";
        return false;
    }
}

bool
Controller::
setRawZprofileBatch (FileHandle &h, qint64 pos, const ByteBlock & arr)
{
    return impl_-> setRawZprofileBatch ( h, pos, arr);
}

bool
Controller::Implementation::
setRawZprofileBatch (FileHandle & handle, qint64 ind, const ByteBlock & a)
{
    try {
        dbg(1) << "batch " << ind << " " << a.size () << "\n";
        if( ! handle.valid ()) return false;
        if( (a.size() == 0) || (a.size () % handle.depth ()))
            LTHROW( "batch profile set - incompatible array size");
        if( ind >= handle.width () * handle.height () || ind < 0)
            LTHROW( "batch profile - index out of range");
        if( ind * handle.depth () + qint64(a.size ()) > handle.width () * handle.height () * handle.depth ())
            LTHROW( "batch profile set - array too long");
        // if the final zprofiles file is already present, return
        if( handle.m_fullProfileFp-> isOpen ())
            return true;
        if( QFileInfo( path( handle, "zprofiles.final")).exists ())
            return true;

        dbg(1) << "refreshing profile map before writing batch\n";
        ByteBlock pmap = getZprofileMap ( handle);
        dbg(1) << "profile map refreshed\n";

        // we have to save to a partial file
        QString indexFname = path( handle, "zprofiles.index");
        QString profilesFname = path( handle, "zprofiles.partial");

        QFile ifp( indexFname);
        if( ! ifp.open( QFile::ReadWrite)) return false;
        if( ! ifp.seek ( ind)) return false;

        AFile ffp;
        if( ! ffp.open( profilesFname.toStdString (), O_WRONLY, O_NOATIME))
            return false;
        bool seekNeeded = true;
        for( qint64 i = 0 ; i < qint64(a.size()) / handle.depth () ; i ++ ) {
            qint64 ii = ind + i;
            if( pmap[ii]) {
                seekNeeded = true;
            } else {
                if( seekNeeded) {
                    if( ! ffp.setPos (ii * handle.depth ())) return false;
                    seekNeeded = false;
                }
                if( ffp.write( & a[i * handle.depth ()], handle.depth ()) != handle.depth ())
                    return false;
            }
        }
        ffp.close ();

        dbg(1) << "Batch raw data written\n";
        // indicate in the index file that these profiles are ready
        ByteBlock tmp( a.size () / handle.depth (), char(1));
        write_ByteBlock ( ifp, tmp);
        ifp.close();
        return true;
    } catch( ...) {
        dbg(1) << "Exception in setRawZprofileBatch\n";
        return false;
    }
}





ByteBlock
Controller::
getRawZprofile (const FileHandle & h, int x, int y)
{
    return impl_-> getRawZprofile ( h, x, y);
}

ByteBlock
Controller::Implementation::
getRawZprofile (const FileHandle & handle, int x, int y)
{

    try {
        if( ! handle.valid ()) return ByteBlock();

        if( x < 0 || x >= handle.width () || y < 0 || y >= handle.height ())
            LTHROW( "x,y out of range in set raw zprofile");
        quint64 ind = y * handle.width () + x;
        // if the final zprofiles file is already present, extract the info
        // from there
        if( ! handle.m_fullProfileFp-> isOpen ()) {
            QString profilesFname = path( handle, "zprofiles.final");
            handle.m_fullProfileFp-> open ( profilesFname, O_RDONLY, O_NOATIME);
        }
        if( handle.m_fullProfileFp-> isOpen ()) {
            AFile & ffp = * handle.m_fullProfileFp; // alias
            if( ! ffp.setPos ( handle.depth () * ind)) return ByteBlock();
            ByteBlock a;
            if( handle.depth () != ffp.read( a, handle.depth ()))
                return ByteBlock();
//            QByteArray a = ffp.read( handle.depth ());
//            if( a.length () != handle.depth ()) return QByteArray();
            return a;
        }
        // we have to save to a partial file
        QString indexFname = path( handle, "zprofiles.index");
        QString profilesFname = path( handle, "zprofiles.partial");

        QFile ifp( indexFname);
        if( ! ifp.open( QFile::ReadOnly)) return ByteBlock();
        if( ! ifp.seek ( ind)) return ByteBlock();
        char c;
        if( 1 != ifp.read( & c, 1)) return ByteBlock();
        if( c == 0) return ByteBlock();
        QFile ffp( profilesFname);
        if( ! ffp.open( QFile::ReadOnly)) return ByteBlock();
        if( ! ffp.seek( handle.depth () * ind)) return ByteBlock();
        ByteBlock a( handle.depth ());
        if( handle.depth () != ffp.read( & a[0], handle.depth ()))
            return ByteBlock();
//        QByteArray a = ffp.read( handle.depth ());
//        if( a.length () != handle.depth ()) return QByteArray();
        return a;
    }
    catch(...) {
        dbg(1) << "getzprofile: exception";
        return ByteBlock();
    }
}


bool
Controller::
getRawZprofileExist (const FileHandle & h, int x, int y)
{
    return impl_-> getRawZprofileExist ( h, x, y);
}
bool
Controller::Implementation::
getRawZprofileExist (const FileHandle & handle, int x, int y)
{
    try {
        if( ! handle.valid ()) return false;
        if( x < 0 || x >= handle.width () || y < 0 || y >= handle.height ())
            LTHROW( "x,y out of range in raw zprofile exists");
        quint64 ind = y * handle.width () + x;
        // if the final zprofiles file is already present, return true
        if( handle.m_fullProfileFp-> isOpen ())
            return true;
        if( QFileInfo( path( handle, "zprofiles.final")).exists ())
            return true;
        // if final file does not exist, check the index
        QString indexFname = path( handle, "zprofiles.index");
        QString profilesFname = path( handle, "zprofiles.partial");

        QFile ifp( indexFname);
        if( ! ifp.open( QFile::ReadOnly)) return false;
        if( ! ifp.seek ( ind)) return false;
        char c;
        if( 1 != ifp.read( & c, 1)) return false;
        if( c != 0) return true; // already set
        return false;

        return true;
    }
    catch(...) {
        dbg(1) << "getzprofileexist: exception";
        return false;
    }
}

/// get a map of all zprofiles for a given file
void
Controller::
getZprofileMap(
        const FileHandle & h,
        std::vector< std::vector< bool > > & map)
{
    impl_-> getZprofileMap (h, map);
}
void
Controller::Implementation::
getZprofileMap(
        const FileHandle & handle,
        std::vector< std::vector< bool > > & map)
{

    ByteBlock a = getZprofileMap ( handle);
    if( qint64(a.size ()) != handle.width () * handle.height ()) {
        for( size_t y = 0 ; y < map.size () ; y ++) {
            if( map[y].size() != map[0].size())
                LTHROW( "getzprofile - map not rectangular");
            for( size_t x = 0 ; x < map[y].size() ; x ++ ) {
                map[y][x] = false;
            }
        }
        return;
    } else {
        qint64 ind = 0;
        for( size_t y = 0 ; y < map.size () ; y ++) {
            if( map[y].size() != map[0].size())
                LTHROW( "getzprofile - map not rectangular");
            for( size_t x = 0 ; x < map[y].size() ; x ++ ) {
                map[y][x] = (a[ind ++] != 0);
            }
        }
        return;
    }
}

ByteBlock
Controller::
getZprofileMap( const FileHandle & handle)
{
    return impl_-> getZprofileMap ( handle);
}


ByteBlock
Controller::Implementation::
getZprofileMap( const FileHandle & handle)
{
    try {
        if( ! handle.valid ()) return ByteBlock();

        // if the final zprofiles file is already present, return full map
        if( handle.m_fullProfileFp-> isOpen ())
            return ByteBlock( handle.width () * handle.height (), char(2));
        if( QFileInfo( path( handle, "zprofiles.final")).exists ()) {
            return ByteBlock( handle.width () * handle.height (), char(2));
        }

        // final zprofile does not exist, so we need to check the partial file
        QString indexFname = path( handle, "zprofiles.index");
        QFile ifp( indexFname);
        if( ! ifp.open( QFile::ReadOnly)) return ByteBlock();
        ByteBlock a( handle.width () * handle.height ());
        if( qint64(a.size ()) != ifp.read( & a[0], a.size ()))
            LTHROW( "Could not read full index in getZprofileMap");
//        QByteArray a = ifp.read( handle.width () * handle.height ());
//        if( a.length () != handle.width () * handle.height ())
//            LTHROW( "index file does not match handle");
        return a;
    }
    catch(...) {
        dbg(1) << "getzprofilemap: exception";
        return ByteBlock();
    }
}




/// retrieve information about a frame
FrameInfo
Controller::
getFrameInfo( const FileHandle & h, qint64 frame)
{
    return impl_-> getFrameInfo( h, frame);
}

/// @todo add caching to minimize open/close
FrameInfo
Controller::Implementation::
getFrameInfo( const FileHandle & handle, qint64 frame)
{
    try {
        if( ! handle.valid ()) return FrameInfo();
        // try to read the info from frames.final
        QFile fp( path( handle, "frames.final"));
        if( fp.open( QFile::ReadOnly) && fp.seek( HistogramInfo::BinSize * frame )) {
            FrameInfo info;
            read_HistogramInfo ( fp, info.hinfo );
            info.m_valid = true;
            return info;
        }
        // that failed, so try to read it from the partial file

        // verify index
        QString indexFname = path( handle, "frames.index");
        QFile ifp( indexFname);
        if( ! ifp.open( QFile::ReadOnly)) return FrameInfo();
        if( ! ifp.seek ( frame)) return FrameInfo();
        char c;
        if( 1 != ifp.read( & c, 1)) return FrameInfo();
        if( c == 0) return FrameInfo();
        ifp.close ();

        // index says the record is valid, let's try to read it
        QFile pfp( path(handle, "frames.partial"));
        if( pfp.open( QFile::ReadOnly) && pfp.seek( HistogramInfo::BinSize * frame )) {
            FrameInfo info;
            read_HistogramInfo ( pfp, info.hinfo );
            info.m_valid = true;
            return info;
        }
        return FrameInfo();
    }
    catch(...) {
        dbg(1) << "exception in getframeinfo\n";
        return FrameInfo();

    }
}

std::vector<bool>
Controller::
getFrameMap ( FileHandle & h)
{
    return impl_-> getFrameMap( h);
}

std::vector<bool>
Controller::Implementation::
getFrameMap ( FileHandle & handle)
{
    std::vector<bool> emptyMap, map;
    try {
        if( ! handle.valid ()) return emptyMap;
        // try to read the info from frames.final
        QFile fp( path( handle, "frames.final"));
        if( fp.open( QFile::ReadOnly)) {
            map.resize ( handle.nframes (), true);
            return map;
        }
        // that failed, try to read it from the partial file
        QString indexFname = path( handle, "frames.index");
        QFile ifp( indexFname);
        if( ! ifp.open( QFile::ReadOnly)) return emptyMap;
        ByteBlock b( handle.nframes ());
        map.resize ( handle.nframes ());
        if( handle.nframes () != ifp.read( &b[0], handle.nframes ()))
            return emptyMap;
        for( int i = 0 ; i < handle.nframes () ; i ++)
            map[i] = (b[i] != 0);
        return map;
    }
    catch(...) {
        dbg(1) << "exception in getframemap";
        return emptyMap;

    }
}



/// store frame info
bool
Controller::
setFrameInfo ( const FileHandle & h, qint64 frame, const FrameInfo & info)
{
    return impl_-> setFrameInfo( h, frame, info);
}
bool
Controller::Implementation::
setFrameInfo ( const FileHandle & handle, qint64 frame, const FrameInfo & info)
{
    try {
        if( ! handle.valid ()) return false;
        // if the final frames file is already present, return
        if( QFileInfo( path( handle, "frames.final")).exists ()) return true;
        // we have to save to a partial file
        QString indexFname = path( handle, "frames.index");
        QString framesFname = path( handle, "frames.partial");

        QFile ifp( indexFname);
        if( ! ifp.open( QFile::ReadWrite)) return false;
        if( ! ifp.seek ( frame)) return false;
        char c;
        if( 1 != ifp.read( & c, 1)) return false;
        if( c != 0) return true; // already set

        AFile ffp;
        if( ! ffp.open( framesFname.toStdString (), O_WRONLY, O_NOATIME))
            return false;
        if( ! ffp.setPos ( HistogramInfo::BinSize * frame))
            return false;
        write_HistogramInfo ( ffp, info.hinfo);
//        if( write( ffp.fd (), & info.hinfo.min, 8) != 8) return false;

//        QFile ffp( framesFname);
//        if( ! ffp.open( QFile::ReadWrite)) return false;
//        if( ! ffp.seek( HistogramInfo::BinSize * frame)) return false;
//        write_HistogramInfo ( ffp, info.hinfo);
//        ffp.close ();


        // indicate in the index file that this frame is ready
        if( ! ifp.seek( frame)) return false;
        c = 1;
        write_simple<char>( ifp, c);
        ifp.close();
        return true;
    } catch( ...) {
        dbg(1) << "Exception in setFrameInfo";
        return false;
    }

}

void
Controller::listAllFiles( std::ostream & out)
{
    impl_-> listAllFiles( out);
}
void
Controller::Implementation::listAllFiles( std::ostream & out)
{
    try {
        out << "All files in db:\n";
        out << "Not implemented\n";
    } catch( ...) {
        dbg(1) << "Exception in listAllFiles\n";
    }
}

QString
Controller::Implementation::
path( const FileHandle & handle, const QString & fname)
{
//    if( ! handle.valid ()) LTHROW( "Invalid handle");
    if( handle.m_prefix.isNull () || handle.m_dirName.isNull ())
        LTHROW( "Invalid handle");

    if( fname.isNull ())
        return rootPath () + "/" + handle.m_prefix + "/" + handle.m_dirName;
    else
        return rootPath () + "/" + handle.m_prefix + "/" + handle.m_dirName
                + "/" + fname;
}

QStringList
Controller::
status (FileHandle & h)
{
    return impl_->status (h);
}

QStringList
Controller::Implementation::
status (FileHandle & h)
{
    QString s;
    QTextStream out( & s);

    if( ! h.valid ()) {
        out << "Invalid entry";
    } else {
        out << "Dimensions: " << h.width () << "x" << h.height () << "\n";
        out << "nFrames: " << h.nframes () << " bitpixsize = " << h.bitpixsize () << "\n";
        FileInfo finfo = getFileInfo ( h);
        if( ! finfo.valid ())
            out << "FileInfo = <invalid>\n";
        else {
            out << "FileInfo = valid histogram = " << finfo.hinfo.valid << "\n";
        }
        out << "Frames = ";
        if( QFileInfo( path( h, "frames.final")).exists ())
            out << "finished\n";
        else {
            std::vector<bool> fm = getFrameMap ( h);
            size_t fmCount = std::count( fm.begin (), fm.end (), true);
            out << fmCount * 100 / h.nframes () << "% ("
                << fmCount << "/" << h.nframes () << ")\n";
        }
        out << "Profiles = ";
        if( QFileInfo( path( h, "zprofiles.final")).exists ())
            out << "finished\n";
        else {
            ByteBlock pm = getZprofileMap ( h);
            qint64 pix = h.width () * h.height ();
            qint64 pmCount = std::count( pm.begin (), pm.end (), 0);
            pmCount = pix - pmCount;
            out << pmCount * 100 / pix << "% (" << pmCount << ")" << "\n";
        }
    }

    return s.split ('\n');
}

void
Controller::
optimize (FileHandle & h)
{
    impl_-> optimize ( h);
}

void
Controller::Implementation::
optimize (FileHandle & handle)
{
    if( ! handle.valid ()) return;
    dbg(1) << "Optimize " << handle.m_dirName << "\n";
    if( ! QFileInfo( path( handle, "frames.final")).exists ()) {
        std::vector<bool> fm = getFrameMap ( handle);
        qint64 fmCount = std::count( fm.begin (), fm.end (), true);
        if( fmCount == handle.nframes ()) {
            dbg(1) << "Optimizing frames\n";
            QFile::rename ( path( handle, "frames.partial"),
                            path( handle, "frames.final"));
            QFile::remove ( path( handle, "frames.index"));
        } else {
            dbg(1) << "Cannot optimize frames yet\n";
        }
    }
    if( ! QFileInfo( path( handle, "zprofiles.final")).exists ()) {
        ByteBlock pm = getZprofileMap ( handle);
        qint64 pCount = std::count( pm.begin (), pm.end (), 0);
        if( pCount == 0 ) {
            dbg(1) << "Optimizing profiles\n";
            QFile::rename ( path( handle, "zprofiles.partial"),
                            path( handle, "zprofiles.final"));
            QFile::remove ( path( handle, "zprofiles.index"));
        } else {
            dbg(1) << "Cannot optimize profiles yet\n";
        }
    }

}




};
};

/*

  db format

  Each file info will be in its own directory:

  <root> / <prefix> / <per file cache>

  <Prefix> will be calculated somehow from the filename. This is so
  that not all files end up in the same directory...

  <per file cache> is a directory name formed by mangling the file's path.

  Inside the directory there will be couple of files in different stages of completion.

  dimensions = binary file with dimensions (width/height/nframes/bitpixsize)
  floc = text file with location information
        line1: unique id (including elgg stuff)
        line2: local file path

  global.final = global info about the file

  frames.partial = partial info about the file
  frames.index = which entries in frames.partial are complete
  frames.final = if all frames are done, this will be the final file

  zprofiles.partial = partial info about the profiles
  zprofiles.index = which entries in profiles.partial are complete
  zprofiles.final = if all profiles are done, this will be the final file

  any file named '.final' is final, and can be safely read without any worries
  that someone else might be writing to it

  */

