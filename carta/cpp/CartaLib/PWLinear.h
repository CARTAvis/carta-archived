/**
 *
 **/

#pragma once

#include "CartaLib.h"
#include "PixelPipeline/IPixelPipeline.h"
#include <QColor>
#include <QString>
#include <memory>
#include <QPointF>

namespace Carta
{
namespace Lib
{
/**
 * @brief Allows specification of a 1D function as piece-wise linear, then
 * query the values of the function using opeartor()
 *
 * @todo This should be moved into it's own file.
 * @todo There is no optimization.
 */
class PWLinear
{
public:

    /// add a point
    /// @warning make sure the newly added point's 'x' is greater than
    /// the previous point's 'x'
    PWLinear &
    add( double x, double y )
    {
        m_points.push_back( QPointF( x, y ) );
        return * this; // allow chaining
    }

    double
    operator() ( double x )
    {
        if ( ! std::isfinite( x ) || m_points.empty() ) {
            return std::numeric_limits < double >::quiet_NaN();
        }

        // test boundary conditions
        if ( x <= m_points.first().x() ) {
            return m_points.first().y();
        }
        if ( x >= m_points.last().x() ) {
            return m_points.last().y();
        }

        // find the segment and linearly interpolate within it
        for ( int i = 1 ; i < m_points.size() ; i++ ) {
            if ( x <= m_points[i].x() ) {
                if ( fabs( m_points[i - 1].x() - m_points[i].x() ) < 1.0e-6 ) {
                    // if the slope of segment is close to infinity, then we choose the previous "y" value
                    return m_points[i - 1].y();
                } else {
                    // otherwise we use the interpolation to get "y" value with respect to the "x"
                    double a =
                        ( m_points[i - 1].y() - m_points[i].y() ) / ( m_points[i - 1].x() - m_points[i].x() );
                    double b = m_points[i].y() - a * m_points[i].x();
                    return a * x + b;
                }
            }
        }
        return std::numeric_limits < double >::quiet_NaN();
    } // ()

private:

    QList < QPointF > m_points;
};
}
}
