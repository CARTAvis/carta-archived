/**
 *
 **/

#include "CCCoordinateFormatter.h"
#include <QDebug>

CCCoordinateFormatter::CCCoordinateFormatter( std::shared_ptr < casa::CoordinateSystem > casaCS )
{
    // remember the pointer to casa coordinate systems
    m_casaCS = casaCS;

    // prepare the axis info
//    m_axisInfos.resize( nAxes());

    qDebug() << "CCC nAxes=" << nAxes();
    for ( auto & u :  m_casaCS->worldAxisUnits() ) {
        qDebug() << "all units:" << u.c_str();
    }
    for ( auto & u :  m_casaCS->worldAxisNames() ) {
        qDebug() << "all names:" << u.c_str();
    }
    for ( size_t i = 0 ; i < m_casaCS-> nPixelAxes() ; i++ ) {
        // find the pixel axes in casacore's coordinate system
        // coord will be the index of the 'coordinate'
        // and coord2 will be an index within that index...
        // warning: casa's coordinates and axes are two completely different things!
        // e.g. a standard 4D fits file with frequency and stokes has 3 coordinates, but
        // 4 axes...
        int coord, coord2;
        m_casaCS-> findPixelAxis( coord, coord2, i );

        qDebug() << i << "-->" << coord << "," << coord2;
        qDebug() << "   " <<
        casa::Coordinate::typeToString( m_casaCS-> coordinate( coord ).type() ).c_str();

        Carta::Lib::AxisInfo aInfo;
        if ( coord >= 0 ) {
            const auto & cc = m_casaCS-> coordinate( coord );
            if ( cc.type() == casa::Coordinate::DIRECTION ) {
                aInfo.setKnownType( coord2 == 0 ?
                                    aInfo.KnownType::DIRECTION_LON :
                                    aInfo.KnownType::DIRECTION_LAT );
            }
            else if ( cc.type() == casa::Coordinate::SPECTRAL ) {
                aInfo.setKnownType( aInfo.KnownType::SPECTRAL );
            }
            else if ( cc.type() == casa::Coordinate::STOKES ) {
                aInfo.setKnownType( aInfo.KnownType::STOKES );
            }
            else if ( cc.type() == casa::Coordinate::TABULAR ) {
                aInfo.setKnownType( aInfo.KnownType::TABULAR );

            }
//            else if ( cc.type() == casa::Coordinate::QUALITY ) {
//                aInfo.setKnownType( aInfo.KnownType::QUALITY);
//            }
            else {
                aInfo.setKnownType( aInfo.KnownType::OTHER );
            }
            CARTA_ASSERT( cc.worldAxisNames().size() > 0);
            CARTA_ASSERT( cc.worldAxisNames() ( coord2 ).c_str());
            QString rawAxisLabel = cc.worldAxisNames() ( coord2 ).c_str();
            QString shortLabel = rawAxisLabel;
            aInfo.setLongLabel( Carta::Lib::HtmlString::fromPlain(rawAxisLabel));
            aInfo.setShortLabel( Carta::Lib::HtmlString::fromPlain(shortLabel));
            aInfo.setUnit( cc.worldAxisUnits() ( coord2 ).c_str() );
        }
        m_axisInfos.push_back( aInfo );
    }

    qDebug() << "Parsed axis infos:";
    for( auto & ai : m_axisInfos) {
        qDebug() << "  lp:" << ai.longLabel().plain()
                 << "lh:" << ai.longLabel().html()
        << "sp:" << ai.shortLabel().html()
        << "sh:" << ai.shortLabel().html()
        << "u:" << ai.unit();
    }
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
    return m_casaCS-> nPixelAxes();
}

QStringList
CCCoordinateFormatter::formatFromPixelCoordinate(
    const CoordinateFormatterInterface::VD & pix )
{
    QStringList list;
    casa::Vector < casa::Double > world;
    casa::Vector < casa::Double > pixel = pix;
    m_casaCS-> toWorld( world, pix );
    for ( unsigned int i = 0 ; i < m_casaCS-> nCoordinates() ; i++ ) {
        casa::String units;
        casa::String s = m_casaCS-> format( units, casa::Coordinate::DEFAULT, world[i], i );
        list.append( QString( "%1%2" ).arg( s.c_str() ).arg( units.c_str() ) );
    }
    return list;
}

QString
CCCoordinateFormatter::calculateFormatDistance(
    const CoordinateFormatterInterface::VD & p1,
    const CoordinateFormatterInterface::VD & p2 )
{
    Q_UNUSED( p1 );
    Q_UNUSED( p2 );

    qFatal( "not implemented" );
}

int
CCCoordinateFormatter::axisPrecision( int axis )
{
    Q_UNUSED( axis );
    qFatal( "not implemented" );
}

CCCoordinateFormatter::Me &
CCCoordinateFormatter::setAxisPrecision(
    int precision, int axis )
{
    Q_UNUSED( precision );
    Q_UNUSED( axis );

    qFatal( "not implemented" );
}

bool
CCCoordinateFormatter::toWorld(
    const CoordinateFormatterInterface::VD & pixel,
    CoordinateFormatterInterface::VD & world ) const
{
    Q_UNUSED( pixel );
    Q_UNUSED( world );

    qFatal( "not implemented" );
}

bool
CCCoordinateFormatter::toPixel(
    const CoordinateFormatterInterface::VD & world,
    CoordinateFormatterInterface::VD & pixel ) const
{
    Q_UNUSED( pixel );
    Q_UNUSED( world );
    qFatal( "not implemented" );
}

void
CCCoordinateFormatter::setTextOutputFormat( CoordinateFormatterInterface::TextFormat fmt )
{
    Q_UNUSED( fmt );
    qFatal( "not implemented" );
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
    qFatal( "not implemented" );
}

CCCoordinateFormatter::Me &
CCCoordinateFormatter::setSkyCS( const KnownSkyCS & scs )
{
    Q_UNUSED( scs );
    qFatal( "not implemented" );
}

SkyFormat
CCCoordinateFormatter::skyFormat()
{
    qFatal( "not implemented" );
}

CCCoordinateFormatter::Me &
CCCoordinateFormatter::setSkyFormat( SkyFormat format )
{
    Q_UNUSED( format );
    qFatal( "not implemented" );
}
