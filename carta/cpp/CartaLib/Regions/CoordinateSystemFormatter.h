/**
 *
 **/

#pragma once

#include "ICoordSystem.h"

namespace Carta
{
namespace Lib
{
namespace Regions
{
/// the purpose of this class is to format coordinates in an easy to use manner :)
class CoordinateSystemFormatter
{
public:

    /// get the labels for the given coordinate system
    QStringList
    getLabels( const CompositeCoordinateSystem & cs );

    /// return a formatted coordinate for the given coordinate system
    QStringList
    format( const CompositeCoordinateSystem & cs, const PointN & pt );

    /// switch between html and raw text formatting
    void
    setHtml( bool flag );

private:

    QString
    getSkyLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

    QString
    formatSky( const BasicCoordinateSystemInfo & bcs, int subAxis, double val );

    QString
    getSpectralLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

    QString
    formatSpectral( const BasicCoordinateSystemInfo & bcs, int subAxis, double val );

    QString
    getStokesLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

    QString
    formatStokes( const BasicCoordinateSystemInfo & bcs, int subAxis, double val );

    QString
    getPixelLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

    QString
    formatPixel( const BasicCoordinateSystemInfo & bcs, int subAxis, double val );

    bool m_htmlFlag = false;
};

 // getLabels

//QStringList
//CoordinateSystemFormatter::format( const CompositeCoordinateSystem & cs, const PointN & pt ); // getLabels

//QString
//CoordinateSystemFormatter::getSkyLabel( const BasicCoordinateSystemInfo & bcs, int subAxis ); // getSkyLabel

//QString
//CoordinateSystemFormatter::formatSky( const BasicCoordinateSystemInfo & bcs,
//                                      int subAxis,
//                                      double val );

//QString
//CoordinateSystemFormatter::getSpectralLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

//QString
//CoordinateSystemFormatter::formatSpectral( const BasicCoordinateSystemInfo & bcs,
//                                           int subAxis,
//                                           double val );

//QString
//CoordinateSystemFormatter::getStokesLabel( const BasicCoordinateSystemInfo & bcs, int subAxis );

//QString
//CoordinateSystemFormatter::formatStokes( const BasicCoordinateSystemInfo & bcs,
//                                         int subAxis,
//                                         double val );




}
}
}
