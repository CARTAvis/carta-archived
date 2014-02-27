#include "NFSSafe.h"

#include <sys/syscall.h>
#include <sys/types.h>

#include <QString>
#include <QHostInfo>
#include <QFileInfo>
#include <QDir>
#include <unistd.h>

#include "common.h"

/// wrapper for some NFS safe operations
/// @todo move NFSSafe to it's own file
namespace NFSSafe {

QString getUniqueString()
{
    return QString( "%1-%2-%3-%4")
            .arg( QHostInfo::localHostName ())
            .arg( getpid())
            .arg( SYS_gettid)
            .arg( qrand ());
}

bool safemkdir( const QString & path)
{
    QFileInfo inf( path);
    QString tmpname = inf.absoluteDir ().path () + "/" + getUniqueString ();
    if( ! QDir::current ().mkdir ( tmpname)) {
        dbg(1) << "Failed to mkdir " << tmpname << "\n";
        throw "too bad";
    }

    dbg(1) << "Successfuly mkdir " << tmpname << "\n";
    // now rename the temporary mkdir
    if( ! QFile::rename ( tmpname, path)) {
        dbg(1) << "Failed to rename " << tmpname << " to " << path << "\n";
        if( ! QDir::current ().rmdir ( tmpname)) {
            dbg(1) << "Failed to clean up - rmdir " << tmpname << "\n";
            return false;
        }
        return false;
    }
    dbg(1) << "safemkdir successful on " << path << "\n";
    return QFileInfo( path).isDir ();
}

bool makeEmptyFile( const QString & fname, quint64 size)
{
    QString tmpfname = fname + "." + getUniqueString ();
    QFile fp( tmpfname);
    if( ! fp.open ( QFile::WriteOnly)) return false;
    if( ! fp.resize ( size)) return false;
    fp.close ();
    if( fp.rename ( fname)) return true;
    fp.remove ();
    return false;
}

};  // namespace NFSSafe

