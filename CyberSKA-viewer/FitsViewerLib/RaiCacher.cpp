#include <cmath>
#include <algorithm>
#include <cassert>

#include <QByteArray>

#include "RaiCache.h"
#include "FitsParser.h"
#include "RaiCacher.h"

//static const quint64 MaxSizeToCacheGlobalHist = 2L * 1024 * 1024 * 1024;


namespace RaiLib {

/// returns
void getMinMax( std::vector<double> & values,
                double percent,
                double & outMin, double & outMax )
{
    return getMinMaxInterruptible( noop, values, percent, outMin, outMax);
}


namespace Cache {

/*
RaiCacher::RaiCacher(qint64 maxMem)
    : m_maxMem( maxMem)
{
}


/// will generate all cache for the given file
bool RaiCacher::generateCacheFor( Controller & c, const FitsFileLocation &floc)
{
    // parse the file
    FitsParser parser;
    if( ! parser.loadFile ( floc)) {
        dbg(1) << "Could not parse file.\n";
        return false;
    }
    FitsParser::HeaderInfo hdr = parser.getHeaderInfo ();
    if( ! shouldCacheZprofile ( hdr)) {
        dbg(1) << "Skipping zprofile generation because the file does not need cache\n";
    }

    FileHandle handle = c.createFileHandle (
                floc, hdr.naxis1, hdr.naxis2,
                hdr.totalFrames, hdr.bitpixSize);
    if( ! handle.valid ()) {
        dbg(1) << "handle not valid\n";
        return false;
    }
    dbg(1) << "handle = " << handle.str () << "\n";

    // populate global info about the file
    dbg(1) << "Generating global info\n";
    FileInfo info = c.getFileInfo ( handle);
    if( info.valid ()) {
        dbg(1) << "global info is already cached, skipping\n";
        dbg(1) << info.str () << "\n";
    } else {
        info.nFrames = hdr.totalFrames;
        // if the total size of the values is more than about 2gig, don't cache for now
        /// @todo implement a selection algorithm that works on really large arrays
        qint64 nValues = qint64(info.nFrames) * hdr.naxis1 * hdr.naxis2;
        if( qint64(nValues * sizeof(double)) > m_maxMem)
        {
            dbg(1) << "Skipping global histogram calculation because file is too big\n";
        } else if( c.getFileInfo (handle).hinfo.valid ){
            dbg(1) << "Skipping global info generation, it was already cached\n";
        } else {
            dbg(1) << "Computing global info\n";
            std::vector<double> values;
            values.reserve ( nValues);
            for( int sz = 0 ; sz < hdr.totalFrames ; sz ++ ) {
                parser.cacheFrame ( sz);
                for( int sy = 0 ; sy < hdr.naxis2 ; sy ++ ) {
                    for( int sx = 0 ; sx < hdr.naxis1 ; sx ++ ) {
                        double v = parser.src(sx, sy, sz);
                        if( std::isfinite(v)) {
                            values.push_back( v);
                        }
                    }
                }
            }
            dbg(1) << "Loaded all data into memory\n";

            getMinMax( values, 0.9500, info.hinfo.min95, info.hinfo.max95);
            getMinMax( values, 0.9800, info.hinfo.min98, info.hinfo.max98);
            getMinMax( values, 0.9900, info.hinfo.min99, info.hinfo.max99);
            getMinMax( values, 0.9950, info.hinfo.min995, info.hinfo.max995);
            getMinMax( values, 0.9990, info.hinfo.min999, info.hinfo.max999);
            getMinMax( values, 0.9999, info.hinfo.min9999, info.hinfo.max9999);
            getMinMax( values, 1.0000, info.hinfo.min, info.hinfo.max);
            info.hinfo.valid = true;
        }

        dbg(1) << "Global info computed\n";

        // now put this into the cache
        if( ! c.setFileInfo ( handle, info)) {
            dbg(1) << "Failed to insert file info into cache\n";
            return false;
        }
    }


    dbg(1) << "Generating info per frame\n";
    if(1){
        QTime t; t.restart ();
        std::vector<bool> fmap = c.getFrameMap (handle);
        dbg(1) << "fmap has " << fmap.size () << " entries\n";
        if( fmap.size () == 0)
            fmap.resize ( hdr.totalFrames, false);
        if( qint64(fmap.size ()) != hdr.totalFrames)
            LTHROW("getFrameMap returned inconsistent map");
        qint64 nUnfinishedFrames = std::count( fmap.begin (), fmap.end (), false);
        dbg(1) << "remaining frames = " << nUnfinishedFrames << "\n";
        for( qint64 i = 0 ; i < nUnfinishedFrames ; i ++ ) {
            // randomize the next frame
            qint64 frame = qrand () % hdr.totalFrames;
            while( fmap[frame]) frame = (frame + 1) % hdr.totalFrames;
            fmap[frame] = true;
            if( t.elapsed () > 5000) {
                dbg(1) << "Frame progress: " << i * 100.0 / hdr.totalFrames << "% complete\n";
                t.restart ();
            }
            if( c.getFrameInfo ( handle, frame).valid ()) {
                dbg(1) << "Skipping frame " << frame << "... already cached\n";
                continue;
            }

            FrameInfo frameInfo;
            //        frameInfo.frameNo = frame;
            if( 1) {
//                parser.cacheFrame ( frame);
                frameInfo.hinfo = parser.getFrameInfo ( frame);
            } else {
                frameInfo.hinfo.min = frame;
                frameInfo.hinfo.valid = true;
            }

            if( ! c.setFrameInfo ( handle, frame, frameInfo)) {
                dbg(1) << "Failed to insert frame info into cache " << frame << " / " << hdr.totalFrames << "\n";
                return false;
            }
        }
    }

    dbg(1) << "Scanning profiles\n";
    dbg(1) << "Getting map\n";
    std::vector< std::vector< bool > > pmap;
    pmap.resize ( hdr.naxis2, std::vector<bool>( hdr.naxis1, false));
    c.getZprofileMap ( handle, pmap);

    dbg(1) << "Generating z profiles\n";

    qint64 nPix = m_maxMem / hdr.bitpixSize / hdr.totalFrames;
    if( nPix == 0) nPix = 1;

    dbg(1) << "nPix = " << nPix << "\n";

    qint64 nDone = 0;

    QTime time; time.restart ();

    qint64 totalPixels = hdr.naxis1 * hdr.naxis2,
            zsize = hdr.totalFrames * hdr.bitpixSize;
    ByteBlock buff;
    while( nDone < totalPixels) {
        {
            double e = time.elapsed () / 1000.0;
            double d = 1.0 * nDone / totalPixels;
            double eta = (1-d) / d * e;
            dbg(1) << "nDone = " << nDone << " -> " << nDone * 100.0 / totalPixels << "%\n";
            dbg(1) << "eta = " << eta << "\n";
            dbg(1) << "rt = " << e << "s\n";
        }

        quint64 s = std::min( nPix, totalPixels - nDone);
        dbg(1) << "Reading in " << s << " profiles\n";
        // preallocate buffer to hold the profiles
        buff.resize ( s * hdr.totalFrames * hdr.bitpixSize);

        dbg(1) << "  s = " << s << "\n";
        int xxs = nDone % hdr.naxis1, yys = nDone / hdr.naxis1;
        for( quint64 z = 0 ; z < quint64(hdr.totalFrames) ; z ++ ) {
            char * bptr = &(buff[0]) + z * hdr.bitpixSize;
            int xx = xxs, yy = yys;
            for( quint64 i = 0, pi = nDone ; i < s ; i ++, pi ++ ) {
                if( ! pmap[yy][xx]) {
                    const char * ptr = parser.srcRaw ( xx, yy, z);
                    std::copy( ptr, ptr + hdr.bitpixSize, bptr);
                }
                bptr += zsize;
                xx ++; if( xx >= hdr.naxis1) { xx = 0; yy ++; }
            }
        }

        dbg(1) << "Writing out " << s << " profiles\n";

//        QByteArray ba = QByteArray::fromRawData ( &(buff[0]), s * hdr.bitpixSize);
        if( ! c.setRawZprofileBatch ( handle, xxs + yys * handle.width (), buff))
            return false;

//        int xx = xxs, yy = yys;
//        char * bptr = &(buff[0]);
//        for( quint64 i = 0, pi = nDone ; i < s ; i ++, pi ++ ) {
//            if( ! pmap[yy][xx]) {
//                QByteArray a = QByteArray::fromRawData ( bptr, zsize);
//                if( ! c.setRawZprofile ( handle, xx, yy, a)) {
//                    dbg(1) << "Failed to set profile " << xx << " " << yy << "\n";
//                }
//            }
//            bptr += zsize;
//            xx ++; if( xx >= hdr.naxis1) { xx = 0; yy ++; }

//        }

        nDone += nPix;
    }

    c.optimize ( handle);
    dbg(1) << "Cache status:\n"
           << "---" << c.status ( handle).join ("\n--- ") << "\n";

    return true;

}
*/



};
};
