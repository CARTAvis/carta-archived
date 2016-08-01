/**
 *
 **/

#include "ICoordSystem.h"
#include "CoordinateSystemFormatter.h"

namespace Carta
{
namespace Lib
{
namespace Regions
{
DefaultCoordSystemConverter::DefaultCoordSystemConverter( int ndim )
{
    m_srcCS = CompositeCoordinateSystem( ndim );
    m_dstCS = CompositeCoordinateSystem( ndim );
}

bool
DefaultCoordSystemConverter::convert( const PointN & src, PointN & dst )
{
    dst = src;
    return true;
}

const CompositeCoordinateSystem &
DefaultCoordSystemConverter::srcCS()
{
    return m_srcCS;
}

const CompositeCoordinateSystem &
DefaultCoordSystemConverter::dstCS()
{
    return m_dstCS;
}

CompositeCoordinateSystem::CompositeCoordinateSystem(int ndim)
{
    m_cs.resize( ndim, BasicCoordinateSystemInfo::pixel( ndim ) );
    m_subAxis.resize( ndim, 0 );
    for ( int i = 0 ; i < ndim ; i++ ) {
        m_subAxis[i] = i;
    }
}

void CompositeCoordinateSystem::setAxis(int axis, const BasicCoordinateSystemInfo & bcs, int subAxis)
{
    CARTA_ASSERT( axis >= 0 && axis < ndim() );
    CARTA_ASSERT( subAxis >= 0 && subAxis < bcs.ndim() );
    m_cs[axis] = bcs;
    m_subAxis[axis] = subAxis;
}

int CompositeCoordinateSystem::ndim() const { return m_cs.size(); }

const BasicCoordinateSystemInfo &CompositeCoordinateSystem::cs(int axis) const
{
    CARTA_ASSERT( axis >= 0 && axis < ndim() );
    return m_cs[axis];
}

int CompositeCoordinateSystem::subAxis(int axis) const
{
    CARTA_ASSERT( axis >= 0 && axis < ndim() );
    return m_subAxis[axis];
}

BasicCoordinateSystemInfo::BasicCoordinateSystemInfo(int dims, WcsType type, WcsSubType subType)
{
    m_ndim = dims;
    m_wcsType = type;
    m_wcsSubType = subType;
}

}
}
}

using namespace Carta::Lib::Regions;

static int
apiTestFormatting()
{
    qDebug() << "Test1: pixel coordinate system";
    {
        CompositeCoordinateSystem cs( 5 );
        CoordinateSystemFormatter f;

        // get the labels for the axes
        qDebug() << "Labels" << f.getLabels( cs );

        // format a coordinate
        qDebug() << "Coord:" << f.format( cs, { 1.0, 2.0, 3.0, 1.0 }
                                          );
    }
    qDebug() << "Test2: spectral cube system";

    // make 4 axis coordinate system
    CompositeCoordinateSystem cs( 4 );

    // first axis will be galactic longitude
    cs.setAxis( 0, BasicCoordinateSystemInfo::galactic(), 0 );

    // second axis will be galactic lattitude
    cs.setAxis( 1, BasicCoordinateSystemInfo::galactic(), 1 );

    // third axis will be frequency
    cs.setAxis( 2, BasicCoordinateSystemInfo::frequency() );

    // last axis will be stokes
    cs.setAxis( 3, BasicCoordinateSystemInfo::stokes() );

    // make a formatter
    CoordinateSystemFormatter f;

    // get the labels for the axes
    qDebug() << "Labels" << f.getLabels( cs );

    // format a coordinate
    qDebug() << "Coord:" << f.format( cs, { 1.0, 2.0, 3.0, 1.0 }
                                      );

    return 0;
} // apiTestFormatting

static int x = apiTestFormatting();
