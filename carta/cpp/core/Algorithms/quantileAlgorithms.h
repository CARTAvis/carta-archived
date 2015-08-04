/**
 * Miscelaneous algorithms related to computing quantiles
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include <QDebug>
#include <limits>
#include <algorithm>
#include <vector>
#include <cmath>

namespace Carta
{
namespace Core
{
namespace Algorithms
{
/// compute requested quantiles
/// \param view the input dataset
/// \param quant which quantiles to compute
/// \return the computed quantiles. If all inputs are nans, the result will also be nans.
///
/// Example: [0.1] will compute a value such that 10% of all values are smaller than the returned
/// value.
///
/// \note this is a dumb algorithm using quickselect. It really only works on datasets that
/// are small enough to store in memory. For really big datasets we need a lot more sophisticated
/// algorithm.
///
/// \note NANs are treated as if they did not exist
///
/// \note for best performance, the supplied list of quantiles should be sorted small->large
template < typename Scalar >
static
typename std::vector < Scalar >
quantiles2pixels(
    NdArray::TypedView < Scalar > & view,
    std::vector < double > quant
    )
{
    qDebug() << "computeClips" << view.dims();

    // basic preconditions
    if ( CARTA_RUNTIME_CHECKS ) {
        for ( auto q : quant ) {
            CARTA_ASSERT( 0.0 <= q && q <= 1.0 );
        }
    }

    // read in all values from the view into memory so that we can do quickselect on it
    std::vector < Scalar > allValues;
    view.forEach(
        [& allValues] ( const Scalar & val ) {
            if ( ! std::isnan( val ) ) {
                allValues.push_back( val );
            }
        }
        );

    // indicate bad clip if no finite numbers were found
    if ( allValues.size() == 0 ) {
        return std::vector < Scalar > ( std::numeric_limits < Scalar >::quiet_NaN(), quant.size() );
    }

    // for every input quantile, do quickselect and store the result
    std::vector < Scalar > result;
    for ( double q : quant ) {
        size_t x1 = Carta::Lib::clamp<size_t>( allValues.size() * q, 0, allValues.size()-1);
        CARTA_ASSERT( 0 <= x1 && x1 < allValues.size() );
        std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end() );
        result.push_back( allValues[x1] );
    }
    CARTA_ASSERT( result.size() == quant.size());

    // some extra debugging help:
    if( CARTA_RUNTIME_CHECKS) {
        qDebug() << "quantile quality check:";
        for( size_t i = 0 ; i < quant.size() ; ++ i) {
            double q = quant[i];
            double v = result[i];
            size_t cnt = 0;
            for( auto inp : allValues) {
                if( inp <= v) cnt ++;
            }
            double qq = double(cnt)/allValues.size();
            qDebug() << "  " << q << "->" << v << qq << fabs(q-qq)
                     << ((fabs(q-qq) > 0.01) ? "!!!" : "");
        }
        qDebug() << "-----------------------------";
    }

    return result;
} // computeClips

/// algorithm for finding quantile from pixel value
template < typename Scalar >
static
double pixel2quantile ( NdArray::TypedView < Scalar > & view, Scalar pixel)
{
    u_int64_t totalCount = 0;
    u_int64_t countBelow = 0;
    view.forEach([&](const Scalar & val) {
        if( Q_UNLIKELY( std::isnan(val))) return;
        totalCount ++;
        if( val <= pixel) countBelow++;
    });
    return double(countBelow) / totalCount;
}

}
}
}
