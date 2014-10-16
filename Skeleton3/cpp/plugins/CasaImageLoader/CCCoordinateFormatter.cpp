/**
 *
 **/

#include "CCCoordinateFormatter.h"
#include <coordinates/Coordinates.h>
#include <measures/Measures/Stokes.h>
#include <QDebug>

class DoubleFormatter
{
    CLASS_BOILERPLATE( DoubleFormatter );

public:
    Me
    showPlus( bool flag )
    {
        m_showPlus = flag;
        return * this;
    }

    Me
    sexagesimal( bool flag, QString separator = ":" )
    {
        m_sexagesimal = flag;
        m_separator   = separator;
        return * this;
    }

    Me
    precision( int p )
    {
        m_precision = p;
        return * this;
    }

    QString
    go( double value )
    {
        QString result;
        if ( value > 0 && m_showPlus ) {
            result += "+";
        }
        if ( value < 0 ) {
            value   = - value;
            result += "-";
        }
        if ( ! m_sexagesimal ) {
            if ( m_precision < 0 ) {
                result += QString::number( value, 'e', - m_precision );
            }
            else {
                result += QString::number( value, 'f', m_precision );
            }
        }
        else {
            // format the 60^2 part and remove it from value
            long dig;
            dig     = long (value) / ( 60 * 60 );
            result += QString::number( dig ) + ':';
            value  -= dig * 60 * 60;

            // format the 60^1 part and remove it from value
            // but make sure to insert a leading 0
            dig = long (value) / 60;
            if ( dig < 10 ) { result += '0'; } result += QString::number( dig ) + ':';
            value -= dig * 60;

            // format the last part 60^0 with the requested precision
            // note: negative precision is converted to positive
            // note: we add 100 and then remove first character (the '1') to get
            //       leading 0 if any...
            result += QString::number( value + 100, 'f', abs( m_precision ) ).remove( 0, 1 );
        }
        return result;
    } // go

protected:
    bool m_showPlus     = false;
    bool m_sexagesimal  = false;
    int m_precision     = 3;
    QString m_separator = ":";
};

static SkyFormatting
getDefaultForSkyCS( const KnownSkyCS & skyCS )
{
    switch ( skyCS )
    {
    case KnownSkyCS::B1950 :
    case KnownSkyCS::J2000 :
    case KnownSkyCS::ICRS :
        return SkyFormatting::Sexagesimal;

    default :
        return SkyFormatting::Degrees;
    }
}

CCCoordinateFormatter::CCCoordinateFormatter( std::shared_ptr < casa::CoordinateSystem > casaCS )
{
    // remember the pointer to casa coordinate systems
    m_casaCS = casaCS;

    // prepare the axis info
    parseCasaCS();
}

CoordinateFormatterInterface *
CCCoordinateFormatter::clone() const
{
    qFatal( "not implemented" );
}

int
CCCoordinateFormatter::nAxes() const
{
    CARTA_ASSERT( m_casaCS );
    return m_casaCS->nPixelAxes();
}

QStringList
CCCoordinateFormatter::formatFromPixelCoordinate( const CoordinateFormatterInterface::VD & pix )
{
    // first convert the pixel coordinates to world coordinates
    casa::Vector < casa::Double > world;
    casa::Vector < casa::Double > pixel = pix;
    m_casaCS->toWorld( world, pix );

    // format each axis
//    QStringList list;
//    for (unsigned int i = 0; i < m_casaCS->nCoordinates(); i++) {
//        casa::String units;
//        casa::String s = m_casaCS->format(units, casa::Coordinate::FIXED, world[i], i);
//        list.append(QString("%1%2").arg(s.c_str()).arg(units.c_str()));
//    }
//    return list;

    QStringList list;
    for ( int i = 0 ; i < nAxes() ; i++ ) {
        list.append( formatWorldValue( i, world[i] ) );
    }
    return list;
} // formatFromPixelCoordinate

QString
CCCoordinateFormatter::calculateFormatDistance( const CoordinateFormatterInterface::VD & p1,
                                                const CoordinateFormatterInterface::VD & p2 )
{
    Q_UNUSED( p1 );
    Q_UNUSED( p2 );

    qFatal( "not implemented" );
}

int
CCCoordinateFormatter::axisPrecision( int axis )
{
    CARTA_ASSERT( axis >= 0 && axis < nAxes());
    return m_precisions[axis];
}

CCCoordinateFormatter::Me &
CCCoordinateFormatter::setAxisPrecision( int precision, int axis )
{
    CARTA_ASSERT( axis >= 0 && axis < nAxes());
    m_precisions[axis] = precision;
    return * this;
}

bool
CCCoordinateFormatter::toWorld( const CoordinateFormatterInterface::VD & pixel,
                                CoordinateFormatterInterface::VD & world ) const
{
    Q_UNUSED( pixel );
    Q_UNUSED( world );

    qFatal( "not implemented" );
}

bool
CCCoordinateFormatter::toPixel( const CoordinateFormatterInterface::VD & world,
                                CoordinateFormatterInterface::VD & pixel ) const
{
    Q_UNUSED( pixel );
    Q_UNUSED( world );
    qFatal( "not implemented" );
}

void
CCCoordinateFormatter::setTextOutputFormat( CoordinateFormatterInterface::TextFormat fmt )
{
    m_textOutputFormat = fmt;
}

const Carta::Lib::AxisInfo &
CCCoordinateFormatter::axisInfo( int ind ) const
{
    CARTA_ASSERT( ind >= 0 && ind < nAxes() );
    return m_axisInfos[ind];
}

CCCoordinateFormatter::Me &
CCCoordinateFormatter::disableAxis( int ind )
{
    Q_UNUSED( ind );
    qFatal( "not implemented" );
}

CCCoordinateFormatter::Me &
CCCoordinateFormatter::enableAxis( int ind )
{
    Q_UNUSED( ind );
    qFatal( "not implemented" );
}

KnownSkyCS
CCCoordinateFormatter::skyCS()
{
    if ( ! m_casaCS->hasDirectionCoordinate() ) {
        return KnownSkyCS::Unknown;
    }
    int which = m_casaCS->directionCoordinateNumber();
    const casa::DirectionCoordinate & dirCoord = m_casaCS->directionCoordinate( which );
    casa::MDirection::Types dirType            = dirCoord.directionType( true );
    switch ( dirType )
    {
    case casa::MDirection::Types::B1950 :
        return KnownSkyCS::B1950;

    case casa::MDirection::Types::J2000 :
        return KnownSkyCS::J2000;

    case casa::MDirection::Types::ICRS :
        return KnownSkyCS::ICRS;

    case casa::MDirection::Types::GALACTIC :
        return KnownSkyCS::Galactic;

    case casa::MDirection::Types::ECLIPTIC :
        return KnownSkyCS::Ecliptic;

    default :
        return KnownSkyCS::Unknown;
    } // switch
} // skyCS

CCCoordinateFormatter::Me &
CCCoordinateFormatter::setSkyCS( const KnownSkyCS & scs )
{
    qDebug() << "setSkyCS" << static_cast < int > ( scs );

    // don't even try to set this to unknown
    if ( scs == KnownSkyCS::Unknown ) {
        return * this;
    }

    // find out where the direction coordinate lives
    int which = m_casaCS->directionCoordinateNumber();

    // make a copy of it
    casa::DirectionCoordinate dirCoordCopy =
        casa::DirectionCoordinate( m_casaCS->directionCoordinate( which ) );

    // change the system in the copy
    casa::MDirection::Types mdir;
    switch ( scs )
    {
    case KnownSkyCS::B1950 :
        mdir = casa::MDirection::B1950;
        break;
    case KnownSkyCS::J2000 :
        mdir = casa::MDirection::J2000;
        break;
    case KnownSkyCS::ICRS :
        mdir = casa::MDirection::ICRS;
        break;
    case KnownSkyCS::Ecliptic :
        mdir = casa::MDirection::ECLIPTIC;
        break;
    case KnownSkyCS::Galactic :
        mdir = casa::MDirection::GALACTIC;
        break;
    default :
        CARTA_ASSERT( false );
        break;
    } // switch
    qDebug() << "Before setReferenceConversion:";
    for ( const auto & name : dirCoordCopy.worldAxisNames() ) {
        qDebug() << name.c_str();
    }
    dirCoordCopy.setReferenceConversion( mdir );
    qDebug() << "After setReferenceConversion:";
    for ( const auto & name : dirCoordCopy.worldAxisNames() ) {
        qDebug() << name.c_str();

        // replace the direction coordinate in the coordinate system
    }
    if ( ! m_casaCS->replaceCoordinate( dirCoordCopy, which ) ) {
        qWarning() << "Could not set wcs because replaceCoordinate() failed";
    }
    else {
        // reparse the coordinate system
        parseCasaCS();
    }
    return * this;
} // setSkyCS

SkyFormatting
CCCoordinateFormatter::skyFormatting()
{
    return m_skyFormatting;
}

CCCoordinateFormatter::Me &
CCCoordinateFormatter::setSkyFormatting( SkyFormatting format )
{
    m_skyFormatting = format;
    if ( m_skyFormatting == SkyFormatting::Default ) {
        m_skyFormatting = getDefaultForSkyCS( skyCS() );
    }
    return * this;
}

// from m_casaCS extract axis infos
void
CCCoordinateFormatter::parseCasaCS()
{
    // default precision is 3
    m_precisions.resize( nAxes(), 3);

    m_axisInfos.clear();

    qDebug() << "CCC nAxes=" << nAxes();
    for ( auto & u : m_casaCS->worldAxisUnits() ) {
        qDebug() << "all units:" << u.c_str();
    }
    for ( auto & u : m_casaCS->worldAxisNames() ) {
        qDebug() << "all names:" << u.c_str();
    }
    for ( int i = 0 ; i < nAxes() ; i++ ) {
        // find the pixel axes in casacore's coordinate system
        // coord will be the index of the 'coordinate'
        // and coord2 will be an index within that index...
        // warning: casa's coordinates and axes are two completely different things!
        // e.g. a standard 4D fits file with frequency and stokes has 3 coordinates, but
        // 4 axes...
        int coord, coord2;
        m_casaCS->findPixelAxis( coord, coord2, i );

        qDebug() << i << "-->" << coord << "," << coord2;
        qDebug() << "   "
                 << casa::Coordinate::typeToString( m_casaCS->coordinate( coord ).type() ).c_str();

        Carta::Lib::AxisInfo aInfo;
        if ( coord >= 0 ) {
            const auto & cc = m_casaCS->coordinate( coord );
            if ( cc.type() == casa::Coordinate::DIRECTION ) {
                aInfo.setKnownType( coord2 == 0 ? aInfo.KnownType::DIRECTION_LON
                                    : aInfo.KnownType::DIRECTION_LAT );
            }
            else if ( cc.type() == casa::Coordinate::SPECTRAL ) {
                aInfo.setKnownType( aInfo.KnownType::SPECTRAL );
            }
            else if ( cc.type() == casa::Coordinate::STOKES ) {
                aInfo.setKnownType( aInfo.KnownType::STOKES );
            }
            else if ( cc.type() == casa::Coordinate::TABULAR ) {
                aInfo.setKnownType( aInfo.KnownType::TABULAR );

                //            else if ( cc.type() == casa::Coordinate::QUALITY ) {
                //                aInfo.setKnownType( aInfo.KnownType::QUALITY);
            }
            //            }
            else {
                aInfo.setKnownType( aInfo.KnownType::OTHER );
            }
            CARTA_ASSERT( cc.worldAxisNames().size() > 0 );
            CARTA_ASSERT( cc.worldAxisNames() ( coord2 ).c_str() );
            QString rawAxisLabel = cc.worldAxisNames() ( coord2 ).c_str();
            QString shortLabel   = rawAxisLabel;
            aInfo.setLongLabel( Carta::Lib::HtmlString::fromPlain( rawAxisLabel ) );
            aInfo.setShortLabel( Carta::Lib::HtmlString::fromPlain( shortLabel ) );
            aInfo.setUnit( cc.worldAxisUnits() ( coord2 ).c_str() );
        }
        m_axisInfos.push_back( aInfo );
    }

    qDebug() << "Parsed axis infos:";
    for ( auto & ai : m_axisInfos ) {
        qDebug() << "  lp:" << ai.longLabel().plain() << "lh:" << ai.longLabel().html()
                 << "sp:" << ai.shortLabel().html() << "sh:" << ai.shortLabel().html()
                 << "u:" << ai.unit();
    }

    // set formatting to default
    setSkyFormatting( SkyFormatting::Default);
} // parseCasaCS

QString
CCCoordinateFormatter::formatWorldValue( int whichAxis, double worldValue )
{
    // get info for this axis
    const Carta::Lib::AxisInfo & ai = axisInfo( whichAxis );

    //
    // decide what to do based on the type of the axis
    //

    // for longigute / latitude we do the same thing, except for a different factor
    // when doing sexagesimal
    if ( ai.knownType() == ai.KnownType::DIRECTION_LON
         || ai.knownType() == ai.KnownType::DIRECTION_LAT ) {
        double sexFactor = (ai.knownType() == ai.KnownType::DIRECTION_LON)
                           ? 24 * 60 * 60 / (2*M_PI)
                           : 180 * 60 * 60 / M_PI;
        // for longitude values, wrap around negative values
        if( ai.knownType() == ai.KnownType::DIRECTION_LON && worldValue < 0) {
            worldValue += 2 * M_PI;
        }
        if ( skyFormatting() == SkyFormatting::Radians ) {
            return DoubleFormatter()
                       .showPlus( false )
                       .sexagesimal( false )
                       .precision( axisPrecision( whichAxis ) )
                       .go( worldValue );
        }
        if ( skyFormatting() == SkyFormatting::Degrees ) {
            return DoubleFormatter()
                       .showPlus( true )
                       .sexagesimal( false )
                       .precision( axisPrecision( whichAxis ) )
                       .go( worldValue * 180 / M_PI )
                   + ( m_textOutputFormat == TextFormat::Html ? "&deg;" : "deg" );
        }
        CARTA_ASSERT( skyFormatting() == SkyFormatting::Sexagesimal );
        return DoubleFormatter()
                   .showPlus( true )
                   .sexagesimal( true, ":" )
                   .precision( axisPrecision( whichAxis ) )
                   .go( worldValue * sexFactor )
//                + QString("(%1)").arg(worldValue*180/M_PI,0,'f',10)
                ;
    }

    // for stokes we convert to a string using casacore's Stokes class
    if( ai.knownType() == ai.KnownType::STOKES) {
        return casa::Stokes::name( static_cast<casa::Stokes::StokesTypes>(round(worldValue))).c_str();
    }

    // for other types we do verbatim formatting
    QString unit = ai.unit();
    if( m_textOutputFormat == TextFormat::Html) {
        unit = unit.toHtmlEscaped();
    }
    return DoubleFormatter()
               .showPlus( false )
               .sexagesimal( false )
               .precision( axisPrecision( whichAxis ) )
               .go( worldValue ) + unit;
} // formatWorldValue
