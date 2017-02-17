#include "SimpleFitsParser.h"
#include "CartaLib/CartaLib.h"

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QFileInfo>
#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QColor>
#include <QTime>
#include <QVariant>
#include <QTimer>
#include <QDebug>

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <climits>
#include <cmath>
#include <vector>

namespace WcsPlotterPluginNS {

//using namespace std;

int SimpleFitsParser::HeaderInfo::dims(int ind) const {
    CARTA_ASSERT_X( ind >= 0, "negative index!?!?!?!");

    if( size_t(ind) >= m_dims.size()) {
        return 1;
    }
    else {
        return m_dims[ind];
    }
}

SimpleFitsParser::HeaderInfo::HeaderInfo()
{
    bitpixSize = 4;
    bitpix = -32;
    naxis = 2;
    naxis1 = naxis2 = naxis3 = naxis4 = 1;
    bscale = bzero = crpix1 = crpix2 = crpix3 = crval1 = crval2 = crval3 = 0;
    cdelt1 = cdelt2 = cdelt3 = 0;
    equinox = 0;
    blank = 0; hasBlank = false;
    totalFrames = 0;
    dataOffset = 0;
    scalingRequired = false;
    iquset = -1;
}


// this is used to sort the keys when writing header file
// returns float for easier management down the road... :)
static float keywordPriority( const QString & key)
{
    if( key == "SIMPLE") return 0;
    if( key == "BITPIX") return 1;
    if( key == "NAXIS") return 2;
    if( key == "NAXIS1") return 3;
    if( key == "NAXIS2") return 4;
    if( key == "NAXIS3") return 5;
    if( key == "NAXIS4") return 6;
    if( key == "NAXIS5") return 7;
    if( key == "END") return std::numeric_limits<float>::max();
    return 1000000;
}

QString FitsHeader::space80 = "                                                                                ";

//// convenience function to convert fits string to a raw string (removing intial quotes & replacing all double quotes with single ones)
//static QString fitsString2raw( const QString & s)
//{
//    if( s.length() < 2) throw "fitsString2raw - string less than 2 characters.";
//    QString res = s;
//    // remove the leading and ending quotes
//    res[0] = res[ res.length()-1] = ' ';
//    // replace all double single-quotes with a single quote
//    res.replace( "''", "'");

//    return res;
//}

// remove leading/trailing quotes and spaces from a string
static QString fitsStringTrimmed( const QString & s)
{
    QString res = s;
    res.remove( '\'');
    res.remove( '"');
    res = res.trimmed();
    return res;
}

// wrapper around regular QFile::read() - it makes sure to read in requested size 's' if possible
static bool blockRead( QFile & f, char * ptr, qint64 s)
{
    qint64 remaining = s;
    while( remaining > 0 ) {
        qint64 d = f.read( (char *) ptr, remaining);
        if( d <= 0 ) {
            qCritical() << "Error: blockRead(): could not read another block.\n";
            return false;
        }
        // update remaining & ptr
        ptr += d;
        remaining -= d;
    }
    return true;
}

// wrapper around regular QFile::write() - it makes sure to write in requested size 's' if possible
static bool blockWrite( QFile & f, const char * ptr, qint64 s)
{
    qint64 remaining = s;
    while( remaining > 0 ) {
        qint64 d = f.write( (const char *) ptr, remaining);
        if( d <= 0 ) {
            qCritical() << "Error: blockWrite(): could not write another block\n";
            return false;
        }
        // update remaining & ptr
        ptr += d;
        remaining -= d;
    }
    return true;
}

// fits header parser
FitsHeader FitsHeader::parse( QFile & f)
{
    FitsHeader hdr;

    // read in header one 'line' (or card) at a time, which is 80 bytes
    // until we find one that conains END
    while( 1)
    {
        // read in another header block
        char block[80];
        if( ! blockRead( f, block, 80)) {
            qCritical() << "Error: FitsHeader::parse() could not read card.\n";
            return hdr;
        }

        // clean up the block by converting anything outside of ASCII [32..126]
        // to spaces
        for( size_t i = 0 ; i < sizeof( block) ; i ++ )
            if( block[i] < 32 || block[i] > 126)
                block[i] = ' ';

        // data offset moves
        hdr._dataOffset += sizeof( block);

        // parse the block: one line at a time (there are 36 lines of 80 chars each,
        // but they are not \0 terminated!!!)
        QString rawLine = QByteArray( (char *) block, sizeof( block) );
        // add this line to the header
        hdr._lines.push_back( FitsLine( rawLine));
        // if this is the 'END' line, terminate the parse
        if( rawLine.startsWith( "END     " ))
            break;
    }
    // adjust offset to be a multiple of 2880
    hdr._dataOffset = ((hdr._dataOffset -1)/ 2880 + 1) * 2880;
    // return this header
    hdr._valid = true;
    return hdr;
}

// will write out the header to a file
// after sorting the lines by keyword priority and if keyword priority is the same then by
// the current line position
bool FitsHeader::write(QFile & f)
{
    // sort the lines based on a) keword priority, b) their current order
    //vector< pair< QString, pair< double, int> > > lines;
    typedef std::pair<QString, std::pair<double,int> > SortLine;
    std::vector<SortLine> lines;
    for( size_t i = 0 ; i < _lines.size() ; i ++ )
        lines.push_back( make_pair(_lines[i].raw(), std::make_pair( keywordPriority( _lines[i].key()), i)));
    // c++ does not support anonymous functions, but it does support local structures/classes with
    // static functions... go figure :)
    struct local { static bool cmp( const SortLine & v1, const SortLine & v2 ) {
        // use std::pair built in comparison, it compares first to first, and only if equal
        // it compares second to second
        return( v1.second < v2.second );
    }};
    std::sort( lines.begin(), lines.end(), local::cmp);
    // put all strings into one big array of bytes for wrting
    QByteArray block;
    for( size_t i = 0 ; i < lines.size() ; i ++ )
        block.append( (lines[i].first + space80).left(80)); // paranoia
    // pad with spaces so that the block is a multiple of 2880 bytes
    while( block.size() % 2880 )
        block.append( ' ');
//    cerr << "FitsHeader::write() block size = " << block.size() << " with " << lines.size() << " lines\n";
//    for( size_t i = 0 ; i < lines.size() ; i ++ )
//        cerr << lines[i].first.toStdString() << "\n";
    if( ! blockWrite( f, block.constData(), block.size()))
        return false;
    else
        return true;
}

int FitsHeader::findLine(const QString &key) {
    for( size_t i = 0 ; i < _lines.size() ; i ++ )
        if( _lines[i].key() == key )
            return i;
    return -1;
}

// get a value from the header as int - throwing an exception if this fails!
int FitsHeader::intValue( const QString & key)
{
    QVariant value = getValue( key);
    if( ! value.isValid())
        throw QString("Could not find key %1 in fits file.").arg(key);
    bool ok;
    int result = value.toInt( & ok);
    if( ! ok )
        throw QString("Found %1=%2 in fits file but expected an integer.").arg(key).arg(value.toString());

    // value converted, return it
    return result;
}

// get a value from the header as int - throwing an exception if this fails!
int FitsHeader::intValue( const QString & key, int defaultValue)
{
    QVariant value = getValue( key);
    if( ! value.isValid())
        return defaultValue;
    bool ok;
    int result = value.toInt( & ok);
    if( ! ok )
        throw QString("Found %1=%2 in fits file but expected an integer.").arg(key).arg(value.toString());

    // value converted, return it
    return result;
}


// get a value from the header as double - throwing an exception if this fails!
double FitsHeader::doubleValue( const QString & key)
{
    QVariant value = getValue( key);
    if( ! value.isValid())
        throw QString("Could not find key %1 in fits file.").arg(key);
    bool ok;
    double result = value.toDouble( & ok);
    if( ! ok )
        throw QString("Found %1=%2 in fits file but expected a double.").arg(key).arg(value.toString());

    // value converted, return it
    return result;
}

// get a value from the header as double - substituting default value if needed!
double FitsHeader::doubleValue( const QString & key, double defaultValue)
{
    QVariant value = getValue( key);
    if( ! value.isValid())
        return defaultValue;
    bool ok;
    double result = value.toDouble( & ok);
    if( ! ok )
        throw QString("Found %1=%2 in fits file but expected a double.").arg(key).arg(value.toString());

    // value converted, return it
    return result;
}


// get a value from the header as string - throwing an exception if this fails!
QString FitsHeader::stringValue( const QString & key)
{
    QVariant value = getValue( key);
    if( ! value.isValid())
        throw QString("Could not find key %1 in fits file.").arg(key);
    return value.toString();
}

// get a value from the header as string - throwing an exception if this fails!
QString FitsHeader::stringValue( const QString & key, const QString & defaultValue)
{
    QVariant value = getValue( key);
    if( ! value.isValid())
        return defaultValue;
    return value.toString();
}


// get a value from the header as int
QVariant FitsHeader::getValue( const QString & key, QVariant defaultValue)
{
    // find the line with this key
    int ind = findLine( key);

    // if there is no such line, report error
    if( ind < 0 )
        return defaultValue;

    // return the value as qvariant
    return QVariant( _lines[ind].value());
}

// set an integer value
void FitsHeader::setIntValue(const QString & pkey, int value, const QString & pcomment)
{
    QString key = (pkey + space80).left(8);
    QString comment = (pcomment + space80).left( 47);
    // construct a line based on the parameters
    QString rawLine = QString( "%1= %2 /  %3").arg( key, -8).arg( value, 20).arg( comment);
    rawLine = (rawLine + space80).left(80); // just in case :)
    // find a line with this key so that we can decide if we are adding a new line or
    // replacing an existing one
    int ind = findLine( pkey);
    if( ind < 0 )
        _lines.push_back( rawLine);
    else
        _lines[ind] = rawLine;
}

// set a double value
void FitsHeader::setDoubleValue(const QString & pkey, double value, const QString & pcomment)
{
    QString space80 = "                                                                                ";
    QString key = (pkey + space80).left(8);
    QString comment = (pcomment + space80).left( 47);
    // construct a line based on the parameters
    QString rawLine = QString( "%1= %2 /  %3").arg( key, -8).arg( value, 20, 'G', 10).arg( comment);
    rawLine = (rawLine + space80).left(80); // just in case :)
    // find a line with this key so that we can decide if we are adding a new line or
    // replacing an existing one
    int ind = findLine( pkey);
    if( ind < 0 )
        _lines.push_back( rawLine);
    else
        _lines[ind] = rawLine;
}

// insert a raw line into fits - no syntax checking is done, except making sure it's padded to 80 chars
void FitsHeader::addRaw(const QString & line)
{
    _lines.push_back( (line + space80).left(80));
}

// simple 3D array
template <class T> struct M3D {
    M3D( int dx, int dy, int dz ) {
        _raw.fill( 0, dx * dy * dz * sizeof(T));
        // cerr << "M3D allocated " << _raw.size() << " bytes\n";
        _dx = dx; _dy = dy; _dz = dz;
    }
    char * raw() { return _raw.data(); }
    T & operator()(int x, int y, int z) {
        qint64 ind = z; ind = ind * _dy + y; ind = ind * _dx + x; ind *= sizeof(T);
        return * (T *) (_raw.constData() + ind);
    }
    void reset() { _raw.fill(0); }
//    void reset() { memset( (void *) _raw.constData(), 0, _raw.size()); }

    QByteArray _raw;
    int _dx, _dy, _dz;
};

// simple 2D array
template <class T> struct M2D {
    M2D( int dx, int dy ) {
        _raw.fill( 0, dx * dy * sizeof(T));
        // cerr << "M2D allocated " << _raw.size() << " bytes\n";
        _dx = dx; _dy = dy;
    }
    char * raw() { return _raw.data(); }
    T & operator()(int x, int y) {
        qint64 ind = (y * _dx + x) * sizeof(T);
        if( ind < 0 || ind >= _raw.size()) throw "M2D out of bounds";
        return * (T *) (_raw.constData() + ind);
    }
    void reset() { _raw.fill(0); }

    QByteArray _raw;
    int _dx, _dy;
};

#ifdef DONT_COMPILE
// buffered 3D FITS cube accessor (by index x,y,z), specific to BITPIX = -32 (i.e. floats)
// almost acts as a 3D matrix but the data is stored on the disk
struct M3DFloatFile{
    M3DFloatFile( QFile * fp, qint64 offset, int dx, int dy, int dz) {
        _fp = fp; _offset = offset; _dx = dx; _dy = dy; _dz = dz;
        _buffStart = _buffEnd = -1;
        _buffSize = 4096;
        _buff = new char[ _buffSize];
        if( ! _buff) throw QString( "Could not allocate buffer");
    }
    ~M3DFloatFile() { delete[] _buff; }
    float operator()(int x, int y, int z) {
        qint64 ind = z; ind = ind * _dy + y; ind = ind * _dx + x; ind *= 4; ind += _offset;
        // if buffer does not yet point to what we need, read in stuff into buffer
        if( ind < _buffStart || ind + 3 > _buffEnd ) {
            qint64 req = _buffSize;
            if( ! _fp-> seek( ind)) throw QString( "Failed to seek to extract a float");
            qint64 len = _fp-> read( _buff, req);
            // cerr << "Reading " << ind << " + " << req << " --> " << len << "\n";
            if( len < 4) throw QString( "Failed to read");
            _buffStart = ind;
            _buffEnd = _buffStart + len - 1;
        }
        char * p = _buff + (ind - _buffStart);
        float val; uchar * d = (uchar *) (& val);
        d[0] = p[3]; d[1] = p[2]; d[2] = p[1]; d[3] = p[0];
        return val;
    }

    QFile * _fp; qint64 _offset; int _dx, _dy, _dz;
    char * _buff;
    qint64 _buffStart, _buffEnd, _buffSize;
};
#endif

SimpleFitsParser::SimpleFitsParser()
{
    m_file = nullptr;
    bitPixFile_ = nullptr;
    m_cachedFrameNum = -1;
}

SimpleFitsParser::~SimpleFitsParser()
{
    if( m_file != 0) {
        if( m_file -> isOpen ())
            m_file -> close ();
        delete m_file;
    }

    if( bitPixFile_ != 0) delete bitPixFile_;

}

// loads a fits file
// - opens it up
// - parses it's header & stores this
bool
SimpleFitsParser::loadFile(
    const FitsFileLocation & fLocation)
{
    qDebug() << "SimpleFitsParser::loadFile( " << fLocation << ")\n";
    // if we had a file open previously, get rid of it
    if( m_file != 0) delete m_file;
    m_file = 0;
    if( bitPixFile_ != 0) delete bitPixFile_;
    bitPixFile_ = 0;
    m_cachedFrameNum = -1;
    m_cachedFrameData.clear ();

    // open the new file
    m_fname = fLocation;

    try {
        m_file = new QFile();
        m_file-> setFileName( m_fname);

        if( ! m_file-> open( QFile::ReadOnly))
            throw QString( "Could not open input file %1").arg( m_fname);
        qDebug() << "File " << m_fname << " open.\n";

        // parse the header
        FitsHeader hdr = FitsHeader::parse( * m_file);
        if( ! hdr.isValid())
            throw QString( "Could not parse FITS header");

        // extract some parameters from the fits file and also validate it a bit
        if( hdr.stringValue("SIMPLE") != "T" )
            throw "Input FITS file does not have 'SIMPLE = T'";
        headerInfo_ = HeaderInfo();
        headerInfo_.bitpix = hdr.intValue( "BITPIX");
        headerInfo_.bitpixSize = bitpixToSize( headerInfo_.bitpix);
        headerInfo_.naxis = hdr.intValue( "NAXIS");
        if( headerInfo_.naxis < 2)
            throw QString( "Cannot deal with files that have NAXIS = %1").arg(headerInfo_.naxis);
        headerInfo_.naxis1 = hdr.intValue( "NAXIS1");
        headerInfo_.naxis2 = hdr.intValue( "NAXIS2");
        headerInfo_.naxis3 = hdr.intValue( "NAXIS3", 1);
        if( headerInfo_.naxis == 2) {
            headerInfo_.naxis = 3; headerInfo_.naxis3 = 1; // simulate 3d cube with 1 frame
        }
        headerInfo_.naxis4 = hdr.intValue( "NAXIS4", 1);

        // read & store all NAXISi values
        {
            int ind = 1;
            while(1) {
                QString keyword = QString("NAXIS%1").arg(ind);
                if( hdr.findLine( keyword) < 0) break;
                int val = hdr.intValue( keyword, -1);
                if( val < 1) {
                    throw QString( "Invalid %1=%2")
                            .arg(keyword).
                            arg(hdr.stringValue(keyword, "n/a"));
                }
                headerInfo_.m_dims.push_back(val);
                ind ++;
            }
        }

        QVariant blank = hdr.getValue( "BLANK");
        if( blank.isValid()) {
            headerInfo_.blank = hdr.intValue( "BLANK");
            headerInfo_.hasBlank = true;
            // blank is only supported for BITPIX > 0
            if( headerInfo_.bitpix < 0) {
                qWarning() << QString("Invalid use of BLANK = %1 keyword with BITPIX = %2.").arg( headerInfo_.blank).arg( headerInfo_.bitpix)
                           << "\n";
                headerInfo_.hasBlank = false;
            }
        } else {
            headerInfo_.hasBlank = false;
        }

        // calculate the number of frames in this fits file (this is basically a product
        // of NAXIS3 * NAXIS4 * ... * NAXISn
        int nFrames = 1;
        for( int i = 3 ; i <= hdr.intValue( "NAXIS") ; i ++ )
            nFrames *= hdr.intValue( QString("NAXIS%1").arg(i));
        headerInfo_.totalFrames = nFrames;
//        cerr << "This fits file has " << nFrames << " frames.\n";

        headerInfo_.bzero = hdr.doubleValue( "BZERO", 0);
        headerInfo_.bscale = hdr.doubleValue( "BSCALE", 1);
        headerInfo_.crval1 = hdr.doubleValue( "CRVAL1", 0);
        headerInfo_.crval2 = hdr.doubleValue( "CRVAL2", 0);
        headerInfo_.crval3 = hdr.doubleValue( "CRVAL3", 0);
        headerInfo_.cdelt1 = hdr.doubleValue( "CDELT1", 1);
        headerInfo_.cdelt2 = hdr.doubleValue( "CDELT2", 1);
        headerInfo_.cdelt3 = hdr.doubleValue( "CDELT3", 1);
        headerInfo_.crpix1 = hdr.doubleValue( "CRPIX1", 0);
        headerInfo_.crpix2 = hdr.doubleValue( "CRPIX2", 0);
        headerInfo_.crpix3 = hdr.doubleValue( "CRPIX3", 0);
        headerInfo_.ctype1 = hdr.stringValue( "CTYPE1", "''");
        headerInfo_.ctype2 = hdr.stringValue( "CTYPE2", "''");
        headerInfo_.ctype3 = hdr.stringValue( "CTYPE3", "''");
        headerInfo_.cunit3 = hdr.stringValue( "CUNIT3", "''");
        headerInfo_.bunit = hdr.stringValue( "BUNIT", "''");
        headerInfo_.equinox = hdr.doubleValue( "EQUINOX", 2000.0);
        headerInfo_.bmaj = hdr.stringValue( "BMAJ", "undefined" );
        headerInfo_.bmin = hdr.stringValue( "BMIN", "undefined" );
        headerInfo_.bpa = hdr.stringValue( "BPA", "undefined" );

        // remove quotes
        headerInfo_.bunit = fitsStringTrimmed( headerInfo_.bunit);

        {
            QString iquset = hdr.stringValue( "IQUSET", "File -1");
            QStringList iqa = iquset.split( ' ', QString::SkipEmptyParts);
            if( iqa.length() > 1) {
                QString s = iqa[1];
                bool ok;
                int x = s.toInt( & ok);
                if( ok) {
                    headerInfo_.iquset = x;
                }
            }
        }

        // make sure the data segment following header is big enough for the data
        qint64 inputSize = m_file-> size();
        qint64 s = headerInfo_.naxis1 * headerInfo_.naxis2 * headerInfo_.naxis3 * headerInfo_.bitpixSize;
        headerInfo_.dataOffset = hdr.dataOffset();
        if( headerInfo_.dataOffset + s > inputSize)
            throw QString( "Invalid fits file size. Maybe accidentally truncated?");

        // position the input to the offset
        if( ! m_file-> seek( headerInfo_.dataOffset))
            throw QString( "Could not read the data (seek failed)");

        // set the header lines
        for( size_t i = 0 ; i < hdr.lines ().size () ; i ++ )
        {
            QString key = FitsLine(hdr.lines ()[i].raw()).key();
            QString val = FitsLine(hdr.lines ()[i].raw()).value();
            if(!key.isEmpty() && QString::compare(key, "ORIGIN", Qt::CaseInsensitive) && QString::compare(key, "HISTORY", Qt::CaseInsensitive) && QString::compare(key, "COMMENT", Qt::CaseInsensitive))
            {
                headerInfo_.headerLines.push_back ( hdr.lines ()[i].raw());
            }
        }

        // figure out whether scaling is actually required
        if( headerInfo_.hasBlank || headerInfo_.bzero != 0 || headerInfo_.bscale != 1.0)
            headerInfo_.scalingRequired = true;
        else
            headerInfo_.scalingRequired = false;
    }
    catch( const QString & err) {
        qWarning() << err;
        return false;
    }
    catch( ...) {
        return false;
    }

    bitPixFile_ = new M3DBitpixFile( headerInfo_, m_file, headerInfo_.dataOffset, headerInfo_.naxis1, headerInfo_.naxis2, headerInfo_.totalFrames);

    return true;
}

// load an entire frame into cache
std::vector<double> & SimpleFitsParser::cacheFrame(int z)
{
    if( m_cachedFrameNum == z) return m_cachedFrameData;
    m_cachedFrameNum = z;
    m_cachedFrameData.resize( headerInfo_.naxis1 * headerInfo_.naxis2);
    size_t ind = 0;
    const char * rawPtr = 0;
    for( int y = 0 ; y < headerInfo_.naxis2 ; y ++ ) {
        for( int x = 0 ; x < headerInfo_.naxis1 ; x ++ ) {
            rawPtr = bitPixFile_->getRaw ( x, y, z);
            double val = raw2double( rawPtr);
            m_cachedFrameData[ind] = val;
            ind ++;
        }
    }
    return m_cachedFrameData;
}



void FitsLine::parse(QString &key, QString &value, QString &comment) {
    // key is the first 8 characters (trimmed)
    key = _raw.left(8).trimmed();
    // by default, value & comment are empty
    value = comment = QString();
    // if there is no equal sign present, return the default values for value/comment, which is empty
    if( _raw.mid( 8, 2).trimmed() != "=") return;
    // find the start/end of the value
    //   start = first non-white character
    //   end   = last character of the value (if string, it's the closing quote, otherwise it's the last non-space
    int vStart = 10, vEnd = -1;
    while( _raw[vStart].isSpace()) { vStart ++; if( vStart >= 80) { vStart = -1; break; }}
    if( vStart == -1) // entire line is empty after the '='
        return;
    if( _raw[vStart] != '\'') { // it's an unquoted value
        // non-string value, find the end
        vEnd = _raw.indexOf( '/', vStart + 1); if( vEnd != -1) vEnd --; else vEnd = 79;
        //            vEnd = vStart + 1;
        //            while( ! _raw[vEnd].isSpace()) { if( vEnd >= 80) break; else vEnd ++; }
        //            vEnd --;
    } else { // it's s quoted string
        // temporarily remove all occurrences of double single-quotes and then find the next single quote
        QString tmp = _raw; for(int i=0;i<=vStart;i++){tmp[i]=' ';} tmp.replace( "''", "..");
        vEnd = tmp.indexOf( '\'', vStart + 1);
        if( vEnd == -1) // we have an unterminated string here
            throw QString( "Unterminated string in header for %1").arg(key);
    }
    // now that we know start/end, get the value
    value = _raw.mid( vStart, vEnd - vStart + 1).trimmed();

    // if this was a string value, get rid of the double single-quotes permanently, and remove the surrounding quotes too
    //if( value[0] == '\'') value = value.mid( 1, value.length()-2).replace( "''", "'");

    // is there a comment?
    comment = _raw.mid( vEnd + 1).trimmed();
    if( ! comment.isEmpty()) {
        if( comment[0] != '/')
            throw ("Syntax error in header: " + _raw.trimmed());
        else
            comment.remove(0,1);
    }
}

}
