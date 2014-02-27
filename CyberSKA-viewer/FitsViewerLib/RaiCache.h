#ifndef RAICACHE_H
#define RAICACHE_H

#include <vector>
#include <QString>
//#include "common.h"
#include "FitsParser.h"
#include "HistogramInfo.h"
#include "AFile.h"

/// main namespace for classes and functions related to RaiViewer library
namespace RaiLib {

    typedef std::vector<char> ByteBlock;

/// classes and function related to caching information about files
namespace Cache {

/// Determines whether a file should have z-profile cached (based on some heuristics)
bool shouldCacheZprofile( const FitsParser::HeaderInfo & );

/// global information about a file in the cache
class FileInfo {
public:

    /// default constructor
    FileInfo() {
        m_valid = false;
    }

    /// is the information valid (i.e. was it retrieved from the cache)
    bool valid() const { return m_valid; }

    /// for debugging
    std::string str() const;

    // things we store in the database

    /// histogram information for the entire file
    HistogramInfo hinfo;

    /// number of frames in the file
    /// value less than 1 indicates unknown frames
    /// @todo remove this field
    qint64 nFrames;

protected:
    /// is the information valid (i.e. was it retrieved from the cache)
    bool m_valid;

    friend class Controller;
};

/// Information about a frame of a file
class FrameInfo {
public:
    /// default constructor creates an invalid frame info
    FrameInfo() : m_valid(false) {}

    /// histogram related info about this frame
    HistogramInfo hinfo;

    /// for debugging
    std::string str() const;

    /// is the frame valid (was is successfully retrieved from cache)
    bool valid() const { return m_valid; }

    /// frame number
//    int frameNo;

protected:
    /// is this frame valid (in cache)
    bool m_valid;

    friend class Controller;
};

///
/// \brief this is a simple class to hold a handle about a file in cache
///
class FileHandle {

public:

    /// default constructor constructs an invalid handle
    FileHandle();

    /// constructor with location, also constructs an invalid handle
    FileHandle( const FitsFileLocation & floc);

    /// destructor
    ~FileHandle();

    /// returns whether the handle is a valid one (i.e. there is a corresponding
    /// entry for the file in the database)
    bool valid() const;

    /// returns the stored location
    const FitsFileLocation & floc() const;

    /// for debugging purposes
    QString str() const;

    /// return width
    qint64 width() const;

    /// return height
    qint64 height() const;

    /// return number of frames
    qint64 nframes() const;

    /// return bitpixsize
    int bitpixsize() const;

    /// return size (in bytes) of a single profile
    qint64 depth() const { return nframes () * bitpixsize (); }

protected:

    /// only controller has access to modify the protected values below
    /// e.g. validity & ID
    friend class Controller;

    /// is this handle valid
    bool m_valid;
    /// copy of the location
    FitsFileLocation m_floc;

    /// directory prefix inside the db
    QString m_prefix;
    /// the actual directory
    QString m_dirName;
    /// demensions of the image
    qint64 m_width, m_height, m_nframes;
    int m_bitpixsize;

    QSharedPointer< AFile > m_fullProfileFp;

private:
    /// @todo figure out how to disable copy, while making the syntax nice
//    FileHandle( const FileHandle & ); // disable copy
//    FileHandle & operator = ( const FileHandle &); // disable assignment
};

/// Main cache controller. All access to the cache is done via this class.
class Controller {
public:

    /// structure describing initialization parameters to the controller
    struct InitParameters {
        /// sets the directory where the cache will reside
        InitParameters & setDir( const QString & dir) {
            m_dir = dir;
            return * this;
        }
        /// sets the name of the subdirectory
        InitParameters & setCacheName( const QString & cacheName) {
            m_cacheName = cacheName;
            return * this;
        }
        /// constructor - sets some default values
        InitParameters() {
            m_dir = "/scratch/raivcache";
            m_cacheName = "cache1";
        }
        /// reads the information from an INI file
        static InitParameters fromIniFile(
                const QString & fname,
                const QString & section = QString());

        inline bool operator == ( const InitParameters & p) const {
            return p.m_dir == m_dir && p.m_cacheName == m_cacheName;
        }

        inline bool operator != ( const InitParameters & p) const {
            return ! ( p == * this);
        }


    protected:
        friend class Controller;
        QString m_dir;
        QString m_cacheName;
    };

    /// constructor, takes InitParameters as input
    Controller( const InitParameters & init);

    /// returns a copy of the init parameters used to construct this controller
    const InitParameters & getInitParameters();

    /// Retrieves a file handle from the cache for a given file.
    /// If it does not exist, the returned handle will be invalid.
    FileHandle getFileHandle( const FitsFileLocation & floc);

    /// Retrieves a file handle from the cache for a given file.
    /// If it does not exist, an entry is created for the file.
    /// If the entry cannot be created, an invalid handle is returned.
    /// width & height are image dimensions, nframes are the number of
    /// frames, and bitpixsize are bytes needed per frame for storing the
    /// zprofile
    FileHandle createFileHandle(
            const FitsFileLocation & floc,
            qint64 width, qint64 height, qint64 nframes, int bitpixsize);

    /// retrieve information about a file
    /// if information does not exist, or handle is invalid, return
    /// invalid FileInfo
    FileInfo getFileInfo( const FileHandle &);

    /// store information about a file in the database if it does not exist
    bool setFileInfo( const FileHandle &, const FileInfo &);

    /// retrieve information about a frame
    FrameInfo getFrameInfo( const FileHandle &, qint64 frame);

    /// store frame info
    bool setFrameInfo ( const FileHandle &, qint64 frame, const FrameInfo &);

    /// get a map of frames (completed frames)
    std::vector<bool> getFrameMap( FileHandle &);

    /// Test to see if zprofile exists
    bool getRawZprofileExist( const FileHandle &, int x, int y);

    /// Retrieve a z-profile.
    /// Empty result indicates there was no profile.
    ByteBlock getRawZprofile( const FileHandle &, int x, int y);

    /// get a map of all zprofiles for a given file, with a pre-allocated boolean map
    void getZprofileMap( const FileHandle &, std::vector< std::vector< bool > > & map);
    ByteBlock getZprofileMap( const FileHandle &);

    /// Store a z-profile.
    bool setRawZprofile( const FileHandle &, int x, int y, const ByteBlock &);

    /// store a batch of z-profiles
    bool setRawZprofileBatch( FileHandle & h, qint64 pos, const ByteBlock & );

    /// get all frames (possibly faster than get 1 frame at a time)
    /// @todo not implemented yet
    std::vector<FrameInfo> getAllFrames( const FileHandle &);

    /// attempt to optimize the cache entry (i.e. if all information is present
    /// in the partial files, it is moved to final files, to speed up read access)
    void optimize( FileHandle &);

    /// get some information about the status of the file
    QStringList status( FileHandle &);

    /// destructor
    ~Controller();

    /// list all files (debugging)
    void listAllFiles( std::ostream & out );

protected:

    /// PIMPL
    class Implementation;
    Implementation * impl_;

    // disable copy/assignment
    Q_DISABLE_COPY( Controller)

};


}; // namespace Cache

}; // namespace RaiLib



#endif // RAICACHE_H
