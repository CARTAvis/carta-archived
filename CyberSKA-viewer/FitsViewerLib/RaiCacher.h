#ifndef RAICACHER_H
#define RAICACHER_H

#include <QtGlobal>
#include <cmath>
#include <algorithm>
#include <cassert>
#include "common.h"
#include "RaiCache.h"


namespace RaiLib {

///// internal class for Noop interrupt
//struct NoInterrupt {
//    void operator()( void) {;}
//};
//static NoInterrupt & noInterrupt() {
//    static NoInterrupt n;
//    return n;
//}

__attribute__((used))
static void noop()  {;}

template <typename Interruptor>
void getMinMaxInterruptible(
        Interruptor & interruptor,
        std::vector<double> & values,
        double percent,
        double & outMin,
        double & outMax )
{
    if( values.size() == 0) {
        // special case when we have no values... return NaNs
        outMin = outMax = std::numeric_limits<double>::quiet_NaN();
        return;
    }
    // otherwise compute the histogram values using selection algorithm
    // STL is nice enough to implement this for us
    int ind1 = round(((1-percent)/2) * values.size());
    int ind2 = values.size() - ind1 - 1;
    std::nth_element( values.begin(), values.begin() + ind1, values.end());
    outMin = values[ind1];

    interruptor();

    std::nth_element( values.begin(), values.begin() + ind2, values.end());
    outMax = values[ind2];
}

void getMinMax( std::vector<double> & values,
                       double percent,
                       double & outMin, double & outMax );


namespace Cache {
/*
class RaiCacher
{
public:
    /// how much memory to use at most
    RaiCacher( qint64 maxMem = 1L * 1024 * 1024 * 1024);

    /// will generate all cache for the given file
    bool generateCacheFor( Controller & c, const FitsFileLocation & floc);

protected:
    qint64 m_maxMem;
};
*/

class Cacher {
public:

    Cacher( qint64 maxMem = 1L * 1024 * 1024 * 1024)
        : m_maxMem( maxMem)
    {;}

    ~Cacher()
    {;}


    /// alias for uninterruptible cache generation
    bool generateCacheFor( Controller & c, const FitsFileLocation & floc)
    {
        return generateCacheForInterruptible( noop, c, floc);
    }

    /// interruptible cache generation
    template < typename Interruptor >
    bool generateCacheForInterruptible(
            Interruptor & interruptor,
            Controller & c,
            const FitsFileLocation & floc)
    {
        // parse the file
        FitsParser parser;
        if( ! parser.loadFile ( floc)) {
            dbg(1) << "Could not parse file.\n";
            return false;
        }

        interruptor();

        FitsParser::HeaderInfo hdr = parser.getHeaderInfo ();
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
            /// TODO implement a selection algorithm that works on really large arrays
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

                getMinMaxInterruptible( interruptor, values, 0.9500, info.hinfo.min95, info.hinfo.max95);
                interruptor();
                getMinMaxInterruptible( interruptor, values, 0.9800, info.hinfo.min98, info.hinfo.max98);
                interruptor();
                getMinMaxInterruptible( interruptor, values, 0.9900, info.hinfo.min99, info.hinfo.max99);
                interruptor();
                getMinMaxInterruptible( interruptor, values, 0.9950, info.hinfo.min995, info.hinfo.max995);
                interruptor();
                getMinMaxInterruptible( interruptor, values, 0.9990, info.hinfo.min999, info.hinfo.max999);
                interruptor();
                getMinMaxInterruptible( interruptor, values, 0.9999, info.hinfo.min9999, info.hinfo.max9999);
                interruptor();
                getMinMaxInterruptible( interruptor, values, 1.0000, info.hinfo.min, info.hinfo.max);
                info.hinfo.valid = true;
                interruptor();
                dbg(1) << "Global info computed\n";
            }

            // now put this into the cache
            if( ! c.setFileInfo ( handle, info)) {
                dbg(1) << "Failed to insert file info into cache\n";
                return false;
            }

            interruptor();
        }

        interruptor();

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
                interruptor();
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

                interruptor();

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

        dbg(1) << "Generating profiles\n";
        ByteBlock pmap2 = c.getZprofileMap ( handle);
        dbg(1) << "Missing profiles = " << std::count( pmap2.begin (), pmap2.end (), 0)
               << "\n";

        interruptor();

        qint64 nPix = m_maxMem / hdr.bitpixSize / hdr.totalFrames;
        if( nPix == 0) nPix = 1;

        dbg(1) << "nPix = " << nPix << "\n";

        qint64 nDone = 0; qint64 nComputed = 0;

        QTime time; time.restart ();

        qint64 totalPixels = hdr.naxis1 * hdr.naxis2,
                zsize = hdr.totalFrames * hdr.bitpixSize;
        ByteBlock buff;
        while( nDone < totalPixels) {
            interruptor();
            {
                double e = time.elapsed () / 1000.0;
                double d = 1.0 * nDone / totalPixels;
                double d2 = 1.0 * nComputed / totalPixels;
                double eta = (1-d) / d2 * e;
                dbg(1) << "nDone = " << nDone << " -> " << nDone * 100.0 / totalPixels << "%\n";
                dbg(1) << "eta = " << eta << "s, rt = " << e << "s\n";
            }

            quint64 s = std::min( nPix, totalPixels - nDone);

            qint64 inThisBlock = std::count( pmap2.begin () + nDone, pmap2.begin () + nDone + s, 0);
            if( inThisBlock != 0 )
            {

                dbg(1) << "Reading in " << s << " profiles\n";
                // preallocate buffer
                buff.resize ( s * hdr.totalFrames * hdr.bitpixSize);

                int xxs = nDone % hdr.naxis1, yys = nDone / hdr.naxis1;
                for( quint64 z = 0 ; z < quint64(hdr.totalFrames) ; z ++ ) {
                    char * bptr = &(buff[0]) + z * hdr.bitpixSize;
                    int xx = xxs, yy = yys;
                    for( quint64 i = 0 , pi = nDone ; i < s ; i ++, pi ++ ) {
                        if( pmap2[pi] == 0) {
                            const char * ptr = parser.srcRaw ( xx, yy, z);
                            std::copy( ptr, ptr + hdr.bitpixSize, bptr);
                        }
                        bptr += zsize;
                        xx ++; if( xx >= hdr.naxis1) { xx = 0; yy ++; }
                    }
                }

                //            interruptor();
                dbg(1) << "Writing out " << s << " profiles\n";

                if( ! c.setRawZprofileBatch ( handle, xxs + yys * handle.width (), buff))
                    return false;
            }


            nDone += nPix;
            nComputed += inThisBlock;
        }

        c.optimize ( handle);
        dbg(1) << "Cache status:\n"
               << "---" << c.status ( handle).join ("\n--- ") << "\n";

        return true;

    }


protected:
    qint64 m_maxMem;

    Q_DISABLE_COPY ( Cacher)

};

//typedef InterruptibleCacher RaiCacher;

}};





#endif // RAICACHER_H
