#ifndef NFSSAFE_H
#define NFSSAFE_H

#include <QtGlobal>

class QString;

namespace NFSSafe {

/// make a temp. directory in the same subdirectory as path
/// try to rename it to the desired name
/// if successful, return true
/// if unsuccessful, remove the temp. directory and return false
bool safemkdir( const QString & path);

/// create an empty file of a given size
bool makeEmptyFile( const QString & fname, quint64 size);

/// try to get a unique string, current pid, current host name
/// with hopes this will be useful as a filename on NFS
QString getUniqueString();


};

#endif // NFSSAFE_H
