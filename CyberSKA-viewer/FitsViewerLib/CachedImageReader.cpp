#include "common.h"
#include "CachedImageReader.h"

CachedImageReader::CachedImageReader(QSharedPointer<RaiLib::Cache::Controller> cc,
        const FitsFileLocation &floc)
{
    m_cc = cc;
    m_parser = QSharedPointer<FitsParser> (new FitsParser);
    if( ! parser().loadFile ( floc)) {
        dbg(1) << "Failed to open " << floc.getLocalFname () << "\n";
        m_parser.clear ();
        return;
    }
    const FitsParser::HeaderInfo & hdr = parser().getHeaderInfo ();

    m_handle = cc-> createFileHandle (
                floc, hdr.naxis1, hdr.naxis2, hdr.totalFrames, hdr.bitpixSize);

    m_hists.resize ( hdr.totalFrames);
}

QSharedPointer<FitsParser> CachedImageReader::parserPtr()
{
    return m_parser;
}

FitsParser & CachedImageReader::parser()
{
    if( m_parser.isNull ()) LTHROW( "pointer is null");
    return * m_parser;
}

RaiLib::Cache::Controller &CachedImageReader::cc()
{
    if( m_cc.isNull ()) LTHROW("m_cc is null");
    return * m_cc;
}

bool CachedImageReader::valid() const
{
    return ! m_parser.isNull ();
}

const FitsFileLocation &CachedImageReader::floc() const
{
    return m_floc;
}

const RaiLib::Cache::FileHandle &CachedImageReader::handle() const
{
    return m_handle;
}

const RaiLib::HistogramInfo &
CachedImageReader::getFrameInfo(qint64 frame)
{
    // make sure we are not out of range
    if( frame < 0 || frame >= parser ().getHeaderInfo ().totalFrames)
        LTHROW( "frame out of range");
    // if this frame is already cached internally, return it
    if( m_hists[frame].valid)
        return m_hists[frame];
    // try to get it from file cache
    m_hists[frame] = cc().getFrameInfo ( handle (), frame).hinfo;
    if( m_hists[frame].valid) return m_hists[frame];
    // we must calculate this
    m_hists[frame] = parser ().getFrameInfo ( frame);
    // stick it into file cache
    RaiLib::Cache::FrameInfo frInfo;
    frInfo.hinfo = m_hists[frame];
    cc().setFrameInfo ( handle(), frame, frInfo);
    // and return it
    return m_hists[frame];

}
