/**
 * Code that generates a fits header from ImageInterface.
 *
 * It can do this using casacore (on casa images), or using custom code on raw FITS files.
 **/

#include "FitsHeaderExtractor.h"
#include "../CasaImageLoader/CCImage.h"
#include "SimpleFitsParser.h"

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
        casa::LatticeBase * latticeBase = base-> getCasaImage();
        if ( latticeBase ) {
            // see if we can use a simple fits parser first
            if ( latticeBase-> isPersistent() ) {
                result = tryRawFits( latticeBase->name().c_str() );
                if ( ! result.isEmpty() ) {
                    return result;
                }
                qDebug() << "simple fits parser failed";
            }

            // it's not FITS, or simple FITS parser could not read it, try
            // casacore's fits conversion
            result = tryCasaCoreFitsConverter( latticeBase );
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

QStringList
FitsHeaderExtractor::tryRawFits( QString fname )
{
    qDebug() << "Trying simple fits parser on" << fname;

    WcsPlotterPluginNS::SimpleFitsParser fitsParser;
    if ( ! fitsParser.loadFile( fname ) ) {
        return QStringList();
    }

    return fitsParser.getHeaderInfo().headerLines;
}

// Most of the code here was extracted from ImageFITS2Converter.cc from casaCore-1.7.0 source
// tree. There are probably unused pieces of code/weird comments,etc...
/// \todo clean up the code below
QStringList
FitsHeaderExtractor::tryCasaCoreFitsConverter( casa::LatticeBase * lbase )
{
    // can we get float image interface ?
    casa::ImageInterface < casa::Float > * fii = nullptr;
    #ifndef Q_OS_MAC
        fii = dynamic_cast < casa::ImageInterface < casa::Float > * > ( lbase );
    #else
        fii = static_cast < casa::ImageInterface < casa::Float > * > ( lbase );
    #endif

    if ( ! fii ) {
        m_errors << "Could not convert base to float image";
        return QStringList();
    }

    // alias image to be reference to the image, since the original code used a reference
    // rather than a pointer and I'm too lazy to convert everything to pointers :)
    casa::ImageInterface < casa::Float > & image = * fii;

    using namespace casa;

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

    //
    // Get coordinates and test that axis removal has been
    // mercifully absent
    //
    CoordinateSystem cSys = image.coordinates();
    if ( cSys.nWorldAxes() != cSys.nPixelAxes() ) {
    errors << "FITS requires that the number of world and pixel axes be"
    " identical.";
    return QStringList();
    }

    //
    // Make degenerate axes last if requested
    // and make Stokes the very last if requested
    //
    IPosition shape = image.shape();
    IPosition newShape = shape;
    const uInt ndim = shape.nelements();

    IPosition cursorOrder( ndim ); // to be used later in the actual data copying
    for ( uInt i = 0 ; i < ndim ; i++ ) {
        cursorOrder( i ) = i;
    }

    if ( stokesLast || degenerateLast ) {
        Vector < Int > order( ndim );
        Vector < String > cNames = cSys.worldAxisNames();

        //       cout << "1: " << cNames << endl;
        uInt nStokes = 0; // number of stokes axes
        if ( stokesLast ) {
            for ( uInt i = 0 ; i < ndim ; i++ ) {
                // loop over axes
                order( i ) = i;
                newShape( i ) = shape( i );
            }
            for ( uInt i = 0 ; i < ndim ; i++ ) {
                // loop over axes
                if ( cNames( i ) == "Stokes" ) {
                    // swap to back
                    nStokes++;
                    order( ndim - nStokes ) = i;
                    newShape( ndim - nStokes ) = shape( i );
                    order( i ) = ndim - nStokes;
                    newShape( i ) = shape( ndim - nStokes );
                }
            }
        }
        if ( nStokes > 0 ) {
            // apply the stokes reordering
            cSys.transpose( order, order );
        }

        //       cNames = cSys.worldAxisNames();
        //       cout << "2: " << cNames << endl;

        if ( degenerateLast ) {
            // make sure the stokes axes stay where they are now
            for ( uInt i = ndim - nStokes ; i < ndim ; i++ ) {
                order( i ) = i;
            }
            uInt j = 0;
            for ( uInt i = 0 ; i < ndim - nStokes ; i++ ) {
                // loop over axes
                if ( shape( i ) > 1 ) {
                    // axis is not degenerate
                    order( j ) = i; // put it in front, keeping order
                    newShape( j ) = shape( i );
                    j++;
                }
            }
            for ( uInt i = 0 ; i < ndim - nStokes ; i++ ) {
                // loop over axes again
                if ( shape( i ) == 1 ) {
                    // axis is degenerate
                    order( j ) = i;
                    newShape( j ) = shape( i );
                    j++;
                }
            }
            cSys.transpose( order, order ); // apply the degenerate reordering
            //	   cNames = cSys.worldAxisNames();
            //	   cout << "3: " << cNames << endl;
        }

        for ( uInt i = 0 ; i < ndim ; i++ ) {
            cursorOrder( i ) = order( i );
            if ( order( i ) != (Int) i ) { }
        }
    }

    //
    bool applyMask = false;
    if ( image.isMasked() ) {
        applyMask = true;
    }

    //
    // Find scale factors
    //
    Record header;
    double bscale, bzero;
    const short minshort = - 32768;
    bool hasBlanks = true;
    if ( BITPIX == - 32 ) {
        bscale = 1.0;
        bzero = 0.0;
        header.define( "bitpix", BITPIX );
        header.setComment( "bitpix", "Floating point (32 bit)" );

        //
        // We don't yet know if the image has blanks or not, so assume it does.
        //
        hasBlanks = true;
    }
    else {
        errors << "BITPIX must be -32 (floating point) or 16 (short integer)";
        return QStringList();
    }

    // At this point, for 32 floating point, we must apply the given
    // mask.  For 16bit, we may know that there are in fact no blanks
    // in the image, so we can dispense with looking at the mask again.
    if ( applyMask && ! hasBlanks ) {
        applyMask = False;
    }

    //
    Vector < Int > naxis( ndim );
    uInt i;
    for ( i = 0 ; i < ndim ; i++ ) {
        naxis( i ) = newShape( i );
    }
    header.define( "naxis", naxis );
    header.define( "bscale", bscale );
    header.setComment( "bscale", "PHYSICAL = PIXEL*BSCALE + BZERO" );
    header.define( "bzero", bzero );
    if ( BITPIX > 0 && hasBlanks ) {
        header.define( "blank", minshort );
        header.setComment( "blank", "Pixels with this value are blank" );
    }
    if ( BITPIX > 0 ) {
        header.define( "datamin", minPix );
        header.define( "datamax", maxPix );
    }

    //
    ImageInfo ii = image.imageInfo();
    String error;
    if ( ! ii.toFITS( error, header ) ) {
        return QStringList();
    }

    //
    header.define( "COMMENT1", "" ); // inserts spaces

    // I should FITS-ize the units

    header.define( "BUNIT", upcase( image.units().getName() ).chars() );
    header.setComment( "BUNIT", "Brightness (pixel) unit" );

    //
    IPosition shapeCopy = newShape;
    Record saveHeader( header );
    Bool ok = cSys.toFITSHeader( header, shapeCopy, True, 'c', True, // use WCS
                                 preferVelocity, opticalVelocity,
                                 preferWavelength );

    if ( ! ok ) {
        errors << "Could not make a standard FITS header. Setting"
        " a simple linear coordinate system.";

        uInt n = cSys.nWorldAxes();
        Matrix < Double > pc( n, n );
        pc = 0.0;
        pc.diagonal() = 1.0;
        LinearCoordinate linear( cSys.worldAxisNames(),
                                 cSys.worldAxisUnits(),
                                 cSys.referenceValue(),
                                 cSys.increment(),
                                 cSys.linearTransform(),
                                 cSys.referencePixel() );
        CoordinateSystem linCS;
        linCS.addCoordinate( linear );

        // Recover old header before it got mangled by toFITSHeader

        header = saveHeader;
        IPosition shapeCopy = newShape;
        Bool ok = linCS.toFITSHeader( header, shapeCopy, True, 'c', False ); // don't use WCS
        if ( ! ok ) {
            error = "Fallback linear coordinate system fails also.";
            return QStringList();
        }
    }

    // When this if test is True, it means some pixel axes had been removed from
    // the coordinate system and degenerate axes were added.

    if ( naxis.nelements() != shapeCopy.nelements() ) {
        naxis.resize( shapeCopy.nelements() );
        for ( uInt j = 0 ; j < shapeCopy.nelements() ; j++ ) {
            naxis( j ) = shapeCopy( j );
        }
        header.define( "NAXIS", naxis );
    }

    //
    // Add in the fields from miscInfo that we can
    //
    const uInt nmisc = image.miscInfo().nfields();
    for ( i = 0 ; i < nmisc ; i++ ) {
        String tmp0 = image.miscInfo().name( i );
        String miscname( tmp0.at( 0, 8 ) );
        if ( tmp0.length() > 8 ) {
            errors << "Truncating miscinfo field " << tmp0.c_str()
                   << " to " << miscname.c_str();
        }

        //
        if ( miscname != "end" && miscname != "END" ) {
            if ( ! header.isDefined( miscname ) ) {
                DataType misctype = image.miscInfo().dataType( i );
                switch ( misctype )
                {
                case TpBool :
                    header.define( miscname, image.miscInfo().asBool( i ) );
                    break;
                case TpChar :
                case TpUChar :
                case TpShort :
                case TpUShort :
                case TpInt :
                case TpUInt :
                    header.define( miscname, image.miscInfo().asInt( i ) );
                    break;
                case TpFloat :
                    header.define( miscname, image.miscInfo().asfloat( i ) );
                    break;
                case TpDouble :
                    header.define( miscname, image.miscInfo().asdouble( i ) );
                    break;
                case TpComplex :
                    header.define( miscname, image.miscInfo().asComplex( i ) );
                    break;
                case TpDComplex :
                    header.define( miscname, image.miscInfo().asDComplex( i ) );
                    break;
                case TpString :
                    if ( miscname.contains( "date" ) && miscname != "date" ) {
                        // Try to canonicalize dates (i.e. solve Y2K)
                        String outdate;

                        // We only need to convert the date, the timesys we'll just
                        // copy through
                        if ( FITSDateUtil::convertDateString( outdate,
                                                              image.miscInfo().asString( i ) ) ) {
                            // Conversion worked - change the header
                            header.define( miscname, outdate );
                        }
                        else {
                            // conversion failed - just copy the existing date
                            header.define( miscname, image.miscInfo().asString( i ) );
                        }
                    }
                    else {
                        // Just copy non-date strings through
                        header.define( miscname, image.miscInfo().asString( i ) );
                    }
                    break;

                // These should be the cases that we actually see. I don't think
                // asArray* converts types.
                case TpArrayBool :
                    header.define( miscname, image.miscInfo().asArrayBool( i ) );
                    break;
                case TpArrayChar :
                case TpArrayUShort :
                case TpArrayInt :
                case TpArrayUInt :
                case TpArrayInt64 :
                    header.define( miscname, image.miscInfo().toArrayInt( i ) );
                    break;
                case TpArrayFloat :
                    header.define( miscname, image.miscInfo().asArrayfloat( i ) );
                    break;
                case TpArrayDouble :
                    header.define( miscname, image.miscInfo().asArraydouble( i ) );
                    break;
                case TpArrayString :
                    header.define( miscname, image.miscInfo().asArrayString( i ) );
                    break;
                default :
                {
                    std::ostringstream ss;
                    ss << misctype;
                    errors << "Not writing miscInfo field '"
                           << miscname.c_str() << "' - cannot handle type " << ss.str().c_str();
                }
                } // switch
            }
            if ( header.isDefined( miscname ) ) {
                header.setComment( miscname, image.miscInfo().comment( i ) );
            }
        }
    }

    //
    // DATE
    //
    String date, timesys;
    Time nowtime;
    MVTime now( nowtime );
    FITSDateUtil::toFITS( date, timesys, now );
    header.define( "date", date );
    header.setComment( "date", "Date FITS file was written" );
    if ( ! header.isDefined( "timesys" ) && ! header.isDefined( "TIMESYS" ) ) {
        header.define( "timesys", timesys );
        header.setComment( "timesys", "Time system for HDU" );
    }

    //
    // add ORIGIN
    //
    // header.define( "ORIGIN", origin );

    // Set up the FITS header
    FitsKeywordList kw = FITSKeywordUtil::makeKeywordList();
    ok = FITSKeywordUtil::addKeywords( kw, header );
    if ( ! ok ) {
        error = "Error creating initial FITS header";
        return QStringList();
    }

    //
    // add HISTORY entries
    //
    /*
    LoggerHolder & logger = image.logger();

    Vector < String > historyChunk;
    uInt nstrings;
    Bool aipsppFormat;
    uInt firstLine = 0;
    while ( 1 ) {
        firstLine = FITSHistoryUtil::toHISTORY( historyChunk, aipsppFormat,
                                                nstrings, firstLine, logger );
        if ( nstrings == 0 ) {
            break;
        }
        String groupType;
        if ( aipsppFormat ) {
            groupType = "LOGTABLE";
        }
        FITSHistoryUtil::addHistoryGroup( kw, historyChunk, nstrings, groupType );
    }
    */

    // add END keyword
    kw.end();

    FitsKeyCardTranslator m_kc;

    // the translator needs a buffer of 2880 bytes...
    QByteArray buff( 2880, 'X' );

    QStringList result;
    kw.first();
    kw.next();
    while ( true ) {
        bool done = ! m_kc.build( buff.data(), kw );
        qDebug() << "Writing 2880 ----";
        qDebug() << buff;
        QString allStr( buff );
        for ( int i = 0 ; i < 2880 ; i += 80 ) {
            QString line = allStr.mid( i, 80 );
            result << line;
            if ( line.startsWith( "END" ) && line.simplified() == "END" ) {
                done = true;
                break;
            }
        }
        if ( done ) {
            break;
        }
    }
    qDebug() << "casafits:" << result;

    // let's go bananas
    return result;
} // tryCasaCoreFitsConverter
