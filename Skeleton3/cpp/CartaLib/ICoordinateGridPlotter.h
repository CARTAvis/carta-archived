/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include <memory>

class QPainter;

/// Algorithm API for drawing grids of coordinate systems.
class CoordinateGridPlotterInterface
{
    CLASS_BOILERPLATE( CoordinateGridPlotterInterface );

public:
    /// set line width
    virtual CoordinateGridPlotterInterface &
    setLineWidth( double width ) = 0;

    /// change to a different sky CS (if applicable)
    virtual void
    setSkyCS( const KnownSkyCS & cs ) = 0;

    /// change which axes to plot (first one is horizontal, second is vertical)
    virtual void
    setAxes( int axis1, int axis2 ) = 0;

    /// plot the grid with the current settings
    virtual void
    plot( QPainter & img ) = 0;

    /// virtual destructor
    virtual
    ~CoordinateGridPlotterInterface()
    { }
};
