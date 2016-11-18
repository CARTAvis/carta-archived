/**
 *
 **/

#include "CoordinateSystemFormatter.h"

namespace Carta
{
namespace Lib
{
namespace Regions
{
void
CoordinateSystemFormatter::setHtml( bool flag )
{
    m_htmlFlag = flag;
}

QString
CoordinateSystemFormatter::getSkyLabel( const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::SKY );
    CARTA_ASSERT( subAxis == 0 || subAxis == 1 );
    if ( bcs.wcsSubType() == WcsSubType::GALACTIC ) {
        if ( subAxis == 0 ) {
            return m_htmlFlag ? "l" : "l";
        }
        else {
            return m_htmlFlag ? "l" : "b";
        }
    }
    else if ( bcs.wcsSubType() == WcsSubType::ECLIPTIC ) {
        if ( subAxis == 0 ) {
            return m_htmlFlag ? "&lambda;" : "l";
        }
        else {
            return m_htmlFlag ? "&beta;" : "b";
        }
    }
    else if ( bcs.wcsSubType() == WcsSubType::J2000 ) {
        if ( subAxis == 0 ) {
            return m_htmlFlag ? "&alpha;" : "ra";
        }
        else {
            return m_htmlFlag ? "&delta;" : "dec";
        }
    }
    else {
        CARTA_ASSERT( false );
    }
    return "?sky?";
} // getSkyLabel

QString
CoordinateSystemFormatter::formatSky( const BasicCoordinateSystemInfo & bcs,
                                      int subAxis,
                                      double val )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::SKY );
    CARTA_ASSERT( subAxis == 0 || subAxis == 1 );
    return QString( "(sky:%1)" ).arg( val );
}

QString
CoordinateSystemFormatter::getSpectralLabel( const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::SPECTRAL );
    CARTA_ASSERT( subAxis == 0 );
    if ( bcs.wcsSubType() == WcsSubType::FREQUENCY ) {
        return m_htmlFlag ? "Freq" : "Freq";
    }
    else {
        CARTA_ASSERT( false );
    }
    return "?spectral?";
}

QStringList
CoordinateSystemFormatter::getLabels( const CompositeCoordinateSystem & cs )
{
    QStringList labels;

    for ( int i = 0 ; i < cs.ndim() ; i++ ) {
        const auto & bcs = cs.cs( i );
        int subAxis = cs.subAxis( i );
        if ( bcs.wcsType() == WcsType::SKY ) {
            labels.append( getSkyLabel( bcs, subAxis ) );
        }
        else if ( bcs.wcsType() == WcsType::SPECTRAL ) {
            labels.append( getSpectralLabel( bcs, subAxis ) );
        }
        else if ( bcs.wcsType() == WcsType::STOKES ) {
            labels.append( getStokesLabel( bcs, subAxis ) );
        }
        else if ( bcs.wcsType() == WcsType::PIXEL ) {
            labels.append( getPixelLabel( bcs, subAxis ) );
        }
        else {
            CARTA_ASSERT( false );
            labels.append( "?" );
        }
    }
    return labels;
} // getLabels

QStringList
CoordinateSystemFormatter::format( const CompositeCoordinateSystem & cs, const PointN & pt )
{
    auto ppt = pt;
    if ( int ( ppt.size() ) < cs.ndim() ) {
        ppt.resize( cs.ndim(), 0.0 );
    }
    QStringList result;
    for ( int i = 0 ; i < cs.ndim() ; i++ ) {
        const auto & bcs = cs.cs( i );
        int subAxis = cs.subAxis( i );
        if ( bcs.wcsType() == WcsType::SKY ) {
            result.append( formatSky( bcs, subAxis, ppt[i] ) );
        }
        else if ( bcs.wcsType() == WcsType::SPECTRAL ) {
            result.append( formatSpectral( bcs, subAxis, ppt[i] ) );
        }
        else if ( bcs.wcsType() == WcsType::STOKES ) {
            result.append( formatStokes( bcs, subAxis, ppt[i] ) );
        }
        else if ( bcs.wcsType() == WcsType::PIXEL ) {
            result.append( formatPixel( bcs, subAxis, ppt[i] ) );
        }
        else {
            result.append( QString::number( ppt[i] ) );
        }
    }
    return result;
} // format

QString
CoordinateSystemFormatter::formatSpectral( const BasicCoordinateSystemInfo & bcs,
                                           int subAxis,
                                           double val )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::SPECTRAL );
    CARTA_ASSERT( subAxis == 0 );
    return QString( "(spec:%1)" ).arg( val );
}

QString
CoordinateSystemFormatter::getStokesLabel( const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::STOKES );
    CARTA_ASSERT( subAxis == 0 );
    if ( bcs.wcsSubType() == WcsSubType::STOKES ) {
        return m_htmlFlag ? "Stokes" : "Stokes";
    }
    else {
        CARTA_ASSERT( false );
    }
    return "?stokes?";
}

QString
CoordinateSystemFormatter::formatStokes( const BasicCoordinateSystemInfo & bcs,
                                         int subAxis,
                                         double val )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::STOKES );
    CARTA_ASSERT( subAxis == 0 );
    static const QStringList symbs( { "I", "Q", "U", "V" }
                                    );
    int ival = val;
    if ( ival >= 0 && ival < symbs.size() ) {
        return symbs[ival];
    }
    else {
        return QString( "(%stokes:%1" ).arg( ival );
    }
}

QString
CoordinateSystemFormatter::getPixelLabel( const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::PIXEL );
    CARTA_ASSERT( subAxis >= 0 && subAxis < bcs.ndim() );
    if ( bcs.wcsSubType() == WcsSubType::PIXEL ) {
        if ( subAxis < 3 ) {
            return QString( 'x' + subAxis );
        }
        else {
            return QString( "axis%1" ).arg( subAxis );
        }
    }
    else {
        CARTA_ASSERT( false );
    }
    return QString( "(%1)" ).arg( subAxis );
}

QString
CoordinateSystemFormatter::formatPixel( const BasicCoordinateSystemInfo & bcs,
                                        int subAxis,
                                        double val )
{
    CARTA_ASSERT( bcs.wcsType() == WcsType::PIXEL );
    CARTA_ASSERT( subAxis >= 0 && subAxis < bcs.ndim() );
    return QString( "(pix:%1)" ).arg( val );
}
}
}
}
