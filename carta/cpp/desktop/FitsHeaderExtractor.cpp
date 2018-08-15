/**
 * Code that generates a fits header from ImageInterface using casacore.
 **/

#include "FitsHeaderExtractor.h"
#include "plugins/CasaImageLoader/CCImage.h"

#include <casacore/images/Images/ImageFITSConverter.h>
#include <casacore/fits/FITS/fitsio.h>
#include <casacore/fits/FITS/hdu.h>
#include <casacore/fits/FITS/FITSDateUtil.h>
#include <casacore/fits/FITS/FITSKeywordUtil.h>
#include <casacore/fits/FITS/FITSHistoryUtil.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/coordinates/Coordinates/LinearCoordinate.h>
#include <casacore/images/Images/ImageInterface.h>

#include "CartaLib/UtilCASA.h"

void FitsLine::_parse(QString &key, QString &value, QString &comment) {
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

QString FitsLine::getline(QString & key, QString & value)
{
    QString NewKeyStr = key.append(QString(8 - key.length(),' '));
    QString NewHeader = QString("%1= %2 /").arg(NewKeyStr).arg(value);
    return NewHeader.append(QString(80 - NewHeader.length(),' '));
}

FitsHeaderExtractor::FitsHeaderExtractor()
{ }

FitsHeaderExtractor::~FitsHeaderExtractor()
{ }

void
FitsHeaderExtractor::setInput( Carta::Lib::Image::ImageInterface::SharedPtr image )
{
    m_cartaImage = image;
}

QStringList
FitsHeaderExtractor::getHeader()
{
    m_errors.clear();
    if ( ! m_cartaImage ) {
        m_errors << "Input is NULL";
        return QStringList();
    }

    QStringList result;

    // was this created using CasaImageLoader plugin?
    CCImageBase * base = dynamic_cast < CCImageBase * > ( & * m_cartaImage );
    if ( base ) {
        casacore::LatticeBase * latticeBase = base-> getCasaImage();
        if ( latticeBase ) {

            // casacore's fits parser
            result = _CasaFitsConverter( latticeBase );
        }
        else {
            m_errors << "Cannot produce FITS header for this image because"
                     << "I have no idea where this image came from.";
        }
    }

    return result;
} // getHeader

QStringList
FitsHeaderExtractor::getErrors()
{
    return m_errors;
}

// Most of the code here was extracted from ImageFITS2Converter.cc from casaCore-1.7.0 source
// tree. There are probably unused pieces of code/weird comments,etc...
/// \todo clean up the code below
QStringList
FitsHeaderExtractor::_CasaFitsConverter( casacore::LatticeBase * lbase )
{
    // can we get float image interface ?
    casacore::ImageInterface < casacore::Float > * fii = nullptr;
    #ifndef Q_OS_MAC
        fii = dynamic_cast < casacore::ImageInterface < casacore::Float > * > ( lbase );
    #else
        fii = static_cast < casacore::ImageInterface < casacore::Float > * > ( lbase );
    #endif

    if ( ! fii ) {
        m_errors << "Could not convert base to float image";
        return QStringList();
    }

    // alias image to be reference to the image, since the original code used a reference
    // rather than a pointer and I'm too lazy to convert everything to pointers :)
    casacore::ImageInterface < casacore::Float > & image = * fii;

    using namespace casacore;

    QStringList errors;
    bool preferVelocity = true;
    bool opticalVelocity = true;
    float minPix = 1.0;
    float maxPix = - 1.0;
    int BITPIX = - 32;

    // I'm leaving the following in as constansts for the moment,
    // in case we ever decide to do something fancier. They used to be configurable parameters
    // in casacore
    const bool stokesLast = false;
    const bool degenerateLast = false;
    const bool preferWavelength = false;
    const bool airWavelength = false;

    const bool verbose = false;
    const bool primHead = true;
    const bool allowAppend = true;
    const bool history = false;

    String originStr = String();
    String errorString;
    ImageFITSHeaderInfo fhi;

    casa_mutex.lock();
    bool isok = ImageFITSConverter::ImageHeaderToFITS (errorString, fhi, image,
                                                       preferVelocity, opticalVelocity,
                                                       BITPIX, minPix, maxPix,
                                                       degenerateLast, verbose,
                                                       stokesLast, preferWavelength,
                                                       airWavelength,
                                                       primHead, allowAppend,
                                                       originStr, history);
    casa_mutex.unlock();

    if(!isok)
    {
        m_errors << errorString.c_str();
        return QStringList();
    }

    // add END keyword
    fhi.kw.end();

    FitsKeyCardTranslator m_kc;

    // the translator needs a buffer of 2880 bytes...
    QByteArray buff( 2880, 'X' );

    QStringList result;
    fhi.kw.first();
    fhi.kw.next();
    while ( true ) {
        bool done = ! m_kc.build( buff.data(), fhi.kw );
        QString allStr( buff );
        for ( int i = 0 ; i < 2880 ; i += 80 ) {
            QString raw = allStr.mid( i, 80 );
            QString line = _FITSKeyWordParser(raw);
            if( !line.isEmpty() )
            {
                result << line;
            }
            if ( line.startsWith( "END" ) && line.simplified() == "END" ) {
                done = true;
                break;
            }
        }
        if ( done ) {
            break;
        }
    }

    return result;
} // tryCasaCoreFitsConverter

QString FitsHeaderExtractor::_FITSKeyWordParser(QString _raw)
{
    QString lineStr;
    QString key = FitsLine(_raw).key();
    QString val = FitsLine(_raw).value();
    if(!key.isEmpty() &&
            QString::compare(key, "ORIGIN", Qt::CaseInsensitive) &&
            QString::compare(key, "HISTORY", Qt::CaseInsensitive) &&
            QString::compare(key, "COMMENT", Qt::CaseInsensitive))
    {
        // in general case
        lineStr = _raw;

        // Known issue: CASAImage does not follow FIT-WCS standard
        // replace PC%2_%2 to PC%1_%1
        if( key.contains(QRegExp("PC[0-9]{2}_[0-9]{2}")) )
        {
            QString NewKey = QString("PC%1_%2").arg(key.at(3)).arg(key.at(6));
            lineStr = FitsLine::getline(NewKey, val);
        };

    }

    return lineStr;
}
