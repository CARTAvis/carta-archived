/**
 *
 **/

#pragma once

namespace Carta
{
namespace Lib
{
/// Helper class that can convert between two different linear coordinate
/// systems in 1D. The reason for making this a class is to offer a slight
/// optimization when one needs to apply the same linear map or its inverse
/// to multiple values. The optimization is achieved by precomputing and
/// storing some intermediates, so that linear transformation is simply
/// a multiplication and addition.
///
/// Naming conventions:
///   x1,x2 refer to the source coordinate system
///   y1,y2 refer to the destination coordinate system
///
class LinearMap1D
{
public:

    /// default constructor is an identity map
    LinearMap1D()
    {
        m_ax = 1.0;
        m_bx = 0.0;
        m_axinv = 1.0;
    }

    /// creates a linear map so that apply(x1) = y1 and apply(x2) = y2
    LinearMap1D( double x1, double x2, double y1, double y2 )
    {
        m_ax = ( y2 - y1 ) / ( x2 - x1 );
        m_bx = y1 - x1 * m_ax;
        m_axinv = 1.0 / m_ax;
    }

    /// apply inverse map to x
    inline double
    inv( double x ) const { return m_axinv * ( x - m_bx ); }

    /// apply the linear map to x
    inline double
    apply( double x ) const { return x * m_ax + m_bx; }

    /// same as apply(x)
    inline double
    operator() ( double x ) const { return apply( x ); }

    /// construct a linear map with ax,bx directly specified
    static LinearMap1D
    createRaw( double ax, double bx );

    /// return the ax factor
    inline double
    ax() const { return m_ax; }

    /// return the bx factor
    inline double
    bx() const { return m_bx; }

protected:

    double m_ax, m_bx, m_axinv; // precomputed constants
};

/// map x from coordinate system x1..x2 to coordinate system y1..y2
/// this is a convenience function that does exactly what the class above does,
/// but without any optimization
inline
double
linMap( double x, double x1, double x2, double y1, double y2 )
{
    return ( x - x1 ) * ( y2 - y1 ) / ( x2 - x1 ) + y1;
}
}
}
