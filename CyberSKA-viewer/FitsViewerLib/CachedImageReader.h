#ifndef CACHEDIMAGEREADER_H
#define CACHEDIMAGEREADER_H

#include <QSharedPointer>

//#include "FitsParser.h"
class FitsParser;

#include "RaiCache.h"


/// this class combines the functionality of the cache & fits parser
class CachedImageReader
{
public:
    typedef QSharedPointer < CachedImageReader > SharedPtr;

    /// Constructor: initialize cached image reader for a particular file
    CachedImageReader(QSharedPointer<RaiLib::Cache::Controller> cc,
                       const FitsFileLocation & floc);
    /// return a parser pointer
    QSharedPointer< FitsParser > parserPtr();
    /// return a parser reference
    FitsParser & parser();
    /// return a cache controller reference
    RaiLib::Cache::Controller & cc();
    /// is the reader valid
    bool valid() const;
    /// return the location
    const FitsFileLocation & floc() const;
    /// return the cache handle
    const RaiLib::Cache::FileHandle & handle() const;
    /// get the histogram info about a frame,
    /// tries 1) internal memory cache
    ///       2) disk cache
    ///       3) computes it
    const RaiLib::HistogramInfo & getFrameInfo( qint64 frame);

protected:
    QSharedPointer< RaiLib::Cache::Controller > m_cc;
    QSharedPointer< FitsParser > m_parser;
    FitsFileLocation m_floc;
    RaiLib::Cache::FileHandle m_handle;

    /// local (memory) cache of frames
    std::vector< RaiLib::HistogramInfo > m_hists;
};

#endif // CACHEDIMAGEREADER_H
