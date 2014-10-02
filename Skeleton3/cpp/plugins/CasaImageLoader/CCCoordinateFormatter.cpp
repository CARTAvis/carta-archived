/**
 *
 **/

#include "CCCoordinateFormatter.h"
#include <QDebug>

CoordinateFormatterInterface *
CCCoordinateFormatter::clone() const
{
    qFatal( "not implemented" );
}

int
CCCoordinateFormatter::nAxes()
{
    qFatal( "not implemented" );
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

const AxisInfo &
CCCoordinateFormatter::axisInfo( int ind ) const
{
    Q_UNUSED( ind );
    qFatal( "not implemented" );
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
