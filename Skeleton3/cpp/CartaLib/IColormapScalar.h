/**
 *
 **/

#pragma once

#include "CartaLib.h"
#include <QColor>
#include <QString>
#include <memory>
#include <QPointF>

namespace Carta
{
namespace Lib
{
/**
 * @brief The IColormapScalar class
 *
 * This is an interface that plugins have to implement to add a new colormap for
 * scalar-type pixels.
 */
class IColormapScalar
{
    CLASS_BOILERPLATE( IColormapScalar );

public:

    virtual QString
    name() = 0;

    virtual QRgb
    convert( const double & val ) = 0;

    virtual
    ~IColormapScalar() { }
};

/**
 * @brief Abstract helper class for deriving from IColormapScalar. It just adds a static
 * name to the IColormapScalar.
 */
class ColormapScalarNamed : public IColormapScalar
{
    CLASS_BOILERPLATE( ColormapScalarNamed );

public:

    ColormapScalarNamed( QString name /*= "n/a"*/ )
    {
        m_name = name;
    }

    virtual QString
    name() override
    {
        return m_name;
    }

    void
    setName( QString name )
    {
        m_name = name;
    }

protected:

    QString m_name;
};

/**
 * @brief Allows specification of a 1D function as piece-wise linear, then
 * query the values of the function using opeartor()
 *
 * @todo This should be moved into it's own file.
 */
class PWLinear
{
public:

    PWLinear &
    add( double x, double y )
    {
        points_.push_back( QPointF( x, y ) );
        return * this; // allow chaining
    }

    double
    operator() ( double x )
    {
        if ( ! std::isfinite( x ) || points_.empty() ) {
            return std::numeric_limits < double >::quiet_NaN();
        }

        // test boundary conditions
        if ( x <= points_.first().x() ) {
            return points_.first().y();
        }
        if ( x >= points_.last().x() ) {
            return points_.last().y();
        }

        // find the segment and interpolate within it
        for ( int i = 1 ; i < points_.size() ; i++ ) {
            if ( x <= points_[i].x() ) {
                double a =
                    ( points_[i - 1].y() - points_[i].y() ) / ( points_[i - 1].x() - points_[i].x() );
                double b = points_[i].y() - a * points_[i].x();
                return a * x + b;
            }
        }
        return std::numeric_limits < double >::quiet_NaN();
    } // ()

private:

    QList < QPointF > points_;
};
}
}
