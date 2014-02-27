#ifndef AFILE_H
#define AFILE_H

#include <fcntl.h>
#include <string>
#include <vector>
#include <QtGlobal>
#include "common.h"
#include <unistd.h>

namespace RaiLib {

// helper class (to auto close raw file descriptors on scope exit)
struct AFile {
    AFile() : m_fd(-1) {}
    ~AFile() { if( m_fd > -1) ::close( m_fd); }
    int fd() const { return m_fd; }
    bool open(const std::string & fname, int flags, mode_t mode) {
//        std::cout << "Opening file: " << fname << "\n";
        if( m_fd > -1) LTHROW( "Open on already open fd");
        m_fd = ::open( fname.c_str (), flags, mode);
        return m_fd > -1;
    }
    bool open(const QString & fname, int flags, mode_t mode) {
        return open( fname.toStdString (), flags, mode);
    }
    bool close() {
        if( m_fd < 0) LTHROW( "close on already closed fd");
        int res = ::close( m_fd);
        m_fd = -1;
        return res == 0;
    }
    qint64 setPos(qint64 offset) {
        if( m_fd < 0) LTHROW( "lseek on closed fd");
        off_t ot = ::lseek ( m_fd, offset, SEEK_SET );
        if( ot == off_t(-1)) return -1;
        return qint64(ot) == offset;
    }
    qint64 write( const char * data, qint64 size) {
        if( m_fd < 0) LTHROW( "write on closed fd");
        return ::write ( m_fd, data, size);
    }
    bool write( const std::vector<char> & block) {
        if( m_fd < 0) LTHROW( "write on closed fd");
        if( block.size () == 0) return true;
        size_t n = write( & block[0], block.size ());
        return n == block.size ();
    }
    qint64 read( char * data, qint64 size) {
        if( m_fd < 0) LTHROW( "read on closed fd");
        return ::read( m_fd, data, size);
    }
    qint64 read( std::vector<char> & data, qint64 size) {
        if( m_fd < 0) LTHROW( "read on closed fd");
        data.resize ( size);
        return ::read( m_fd, & data[0], size);
    }

    bool isOpen() const { return (m_fd > -1); }

protected:
    int m_fd;
    int & fd() { return m_fd; }

    Q_DISABLE_COPY( AFile)
//    // disable copy
//    AutoFile( const AutoFile &);
//    AutoFile & operator = ( const AutoFile &);
};

}


#endif // AFILE_H
