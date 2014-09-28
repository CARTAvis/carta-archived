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
    Q_UNUSED( pix );
    qFatal( "not implemented" );
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

CoordinateFormatterInterface::Chain
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
