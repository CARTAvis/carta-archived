#ifndef FITSPARSER_H
#define FITSPARSER_H

#include <limits>
#include <algorithm>

#include <QImage>
#include <QString>
#include <QFile>
#include <QColor>
#include <QList>
#include <QStringList>


#include "common.h"
#include "HistogramInfo.h"

/// This class is used to do some basic parsing of FITS files. Only the primary HDU
/// is parsed. The primary purpose is to extract some 'interesting' values from
/// the header, and then to access the individual elements of the image array.
class FitsParser
{
public:

    /// @brief Relevant values extracted from the fits header.
    /// Only 'interesting values' are extracted. These are basically the ones needed to
    /// complete the extraction, and ones that the fits viewer can use
    struct HeaderInfo {
        // raw values
        int bitpix;
        int naxis;
        int naxis1, naxis2, naxis3;
        double bscale, bzero;
        double crpix1, crpix2, crpix3, crval1, crval2, crval3, cdelt1, cdelt2, cdelt3;
        QString ctype1, ctype2, ctype3, cunit3, bunit;
        double equinox;
        int blank; bool hasBlank;
        QString bmaj, bmin, bpa;
        int iquset;

        // derived values
        int bitpixSize; // bitpix converted to bytes
        bool scalingRequired; // if BLANK, BZERO or BITPIX are specified
        int totalFrames; // total number of frames = product(naxis3,...,naxisN)
        qint64 dataOffset; // calculated offset to the first byte of the data

        // all header lines stored raw
        QStringList headerLines;

        HeaderInfo();
    };

    /// constructor - creates a non-valid parser
    FitsParser();

    /// destructor - frees up resources (if any)
    ~FitsParser();

    /// Sets the file location, parses the header and returns true on success
    bool loadFile( const FitsFileLocation & fLocation_);

    /// returns the current location of the fits file
    const FitsFileLocation & getLocation() const { return fLocation_; }

    /// Returns a parsed header info (only interesting information though)
    inline const HeaderInfo & getHeaderInfo() const { return headerInfo_; }

    /// Extracts a value from a pixel (using image coordinates)
    /// BLANK, BZERO and BSCALE are applied to the extracted raw values
    /// and returns the result as double, regardless of bitpix
    /// If the current frame is cached, the cached value is returned.
    double src( int x, int y, int z);

    /// extracts a value from a pixel and returns a pointer to the raw value, adjusted
    /// to the endianness of the local machine, but without applying BLANK,BZERO or BSCALE
    const char * srcRaw( int x, int y, int z);

    /// applies BLANK, BZERO and BSCALE to a raw value (passed in as a pointer)
    /// heder.bitpix is used to interpret what the pointer points to
    double raw2double( const char * ptr);

    /// calculates frame information
    RaiLib::HistogramInfo getFrameInfo( int frame);

    /// caches an entire frame z in memory and returns pointer to the first
    /// element in cache
    std::vector<double> & cacheFrame( int z);

private:
    QFile * file_;
    FitsFileLocation fLocation_;
    HeaderInfo headerInfo_;
    struct M3DBitpixFile;
    M3DBitpixFile * bitPixFile_;

    std::vector<double> m_cachedFrameData;
    int m_cachedFrameNum;

    // disable copy
    FitsParser( const FitsParser &) = delete;
    FitsParser & operator = ( const FitsParser &) = delete;

};

// given a raw bitpix value (as found in FITS files), returns the number of bytes
// which is basically abs(bitpix)/8
inline int bitpixToSize( int bitpix )
{
    if( bitpix == 8 ) return 1;
    if( bitpix == 16 ) return 2;
    if( bitpix == 32 ) return 4;
    if( bitpix == -32 ) return 4;
    if( bitpix == -64 ) return 8;

    throw QString( "Illegal value BITPIX = %1").arg( bitpix);
}

// buffered 3D FITS cube raw data accessor (by index x,y,z)
// acts as a 3D matrix but the data is stored on the disk
struct FitsParser::M3DBitpixFile {

    M3DBitpixFile( const FitsParser::HeaderInfo & fits,
                   QFile * fp, qint64 offset, int dx, int dy, int dz)
    {
        _fp = fp; _offset = offset; _dx = dx; _dy = dy; _dz = dz;
        _dataSize = fits.bitpixSize;
        _buffStart = _buffEnd = -1; // indicate invalid buffer
        _buffSize = 4096;
        _buff = new char[ _buffSize];
        if( ! _buff) throw QString( "Could not allocate buffer");
    }

    ~M3DBitpixFile() {
        delete[] _buff;
    }

    // place to store results from getRaw if endian swap is needed
    char resBuffer[8];

    // returns a pointer where the raw bytes can be extracted from, already
    // converted to the native endian
    // on error an exception is thrown
    inline const char * getRaw(int x, int y, int z) {
        qint64 ind = z;
        ind = ind * _dy + y;
        ind = ind * _dx + x;
        ind *= _dataSize;
        ind += _offset;

//  would the compiler optimize this better if it was written like this?
//        ind = ((z * _dy + y) * _dx + x) * _dataSize + _offset;
//  or like this?
//        ind = z * framesize + y * rowsize + x * datasize + offset;

        // if buffer does not yet point to what we need, read in stuff into buffer
        if( ind < _buffStart || ind + _dataSize -1 > _buffEnd ) {
            qint64 req = _buffSize;
            if( ! _fp-> seek( ind)) {
                throw QString( "Failed to seek to extract data");
            }
            qint64 len = _fp-> read( _buff, req);
            if( len < _dataSize) {
                throw QString( "failed to read the minimum (%1").arg( _dataSize);
            }
            _buffStart = ind;
            _buffEnd = _buffStart + len - 1;
        }
        char * p = _buff + (ind - _buffStart);
        // endian swap
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        // no swap, i.g. on power pc
        return p;
#else
        // big to small endian copy. e.g. on intel
        std::reverse_copy( p, p + _dataSize, resBuffer);
        return resBuffer;
#endif
    }

    QFile * _fp;
    qint64 _offset;
    int _dx, _dy, _dz, _dataSize;
    char * _buff;
    qint64 _buffStart, _buffEnd, _buffSize;
};

inline double
FitsParser::src(int x, int y, int z)
{
    if( bitPixFile_ == 0) throw "FitsParser::src() bitPixFile is null.";
    const char * ptr = 0;
    if( m_cachedFrameNum == z) {
        qint64 ind = y * headerInfo_.naxis1 + x;
        if( ind < 0 || ind >= qint64(m_cachedFrameData.size ()))
            throw "FitsParser::src() index out of bounds";
        return m_cachedFrameData[ ind];
    } else {
        ptr = bitPixFile_->getRaw ( x, y, z);
        return raw2double ( ptr);
    }
}

// extracts a value from a pixel and returns a pointer to the raw value, adjusted
// to the endianness of the local machine, but without applying BLANK,BZERO or BSCALE
inline const char *
FitsParser::srcRaw( int x, int y, int z)
{
    if( bitPixFile_ == 0) throw "FitsParser::srcRaw() bitPixFile is null.";
    return bitPixFile_->getRaw ( x, y, z);
}

// applies BLANK, BZERO and BSCALE to a raw value (passed in as a pointer)
/// heder.bitpix is used to interpret what the pointer points to
inline double FitsParser::raw2double( const char * ptr)
{
    // convert the raw bytes to an initial double
    double original;
    switch( headerInfo_.bitpix) {
    case   8: original = double( * (quint8 *) ptr); break;
    case  16: original = double( * (qint16 *) ptr); break;
    case  32: original = double( * (qint32 *) ptr); break;
    case  64: original = double( * (qint64 *) ptr); break;
    case -32: original = double( * ( float *) ptr); break;
    case -64: original = double( * (double *) ptr); break;
    default: throw QString( "Illegal value BITPIX = %1").arg( headerInfo_.bitpix);
    }

    // if blank, bzero or bscale are not specified (& nontrivial), apply scaling,
    // otherwise return the results directly
    if( ! headerInfo_.scalingRequired) {
        return original;
    }

    // check to see if we should apply BLANK
    if( headerInfo_.hasBlank && original == headerInfo_.blank )
        return std::numeric_limits<double>::quiet_NaN();
    else
        return headerInfo_.bzero + headerInfo_.bscale * original;

}

#endif // FITSPARSER_H
