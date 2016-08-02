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

CompositeCoordinateSystem::CompositeCoordinateSystem( int ndim )
{
    m_cs.resize( ndim, BasicCoordinateSystemInfo::pixel( ndim ) );
    m_subAxis.resize( ndim, 0 );
    for ( int i = 0 ; i < ndim ; i++ ) {
        m_subAxis[i] = i;
    }
}

void
CompositeCoordinateSystem::setAxis( int axis, const BasicCoordinateSystemInfo & bcs, int subAxis )
{
    CARTA_ASSERT( axis >= 0 && axis < ndim() );
    CARTA_ASSERT( subAxis >= 0 && subAxis < bcs.ndim() );
    m_cs[axis] = bcs;
    m_subAxis[axis] = subAxis;
}

int
CompositeCoordinateSystem::ndim() const
{
    return m_cs.size();
}

const BasicCoordinateSystemInfo &
CompositeCoordinateSystem::cs( int axis ) const
{
    CARTA_ASSERT( axis >= 0 && axis < ndim() );
    return m_cs[axis];
}

int
CompositeCoordinateSystem::subAxis( int axis ) const
{
    CARTA_ASSERT( axis >= 0 && axis < ndim() );
    return m_subAxis[axis];
}

BasicCoordinateSystemInfo::BasicCoordinateSystemInfo( int dims, WcsType type, WcsSubType subType )
{
    m_ndim = dims;
    m_wcsType = type;
    m_wcsSubType = subType;
}

ICoordSystemConverter::UniquePtr
makePixelIdentityConverter( int ndim )
{
    auto pixelCS = CompositeCoordinateSystem(ndim);
    auto cvtPtr = new IdentityCoordSystemConverter( pixelCS);
    return ICoordSystemConverter::UniquePtr( cvtPtr);
//    auto rawPtr = new IdentityCoordSystemConverter(
//        CompositeCoordinateSystem( ndim ) );

//    return ICoordSystemConverter::UniquePtr(
//               new IdentityCoordSystemConverter(
//                   CompositeCoordinateSystem( ndim ) ) );
}
}
}
}


#ifdef DONT_COMPILE
DefaultCoordSystemConverter::DefaultCoordSystemConverter( int ndim )
{
    m_srcCS = CompositeCoordinateSystem( ndim );
    m_dstCS = CompositeCoordinateSystem( ndim );
}

bool
DefaultCoordSystemConverter::src2dst( const PointN & src, PointN & dst )
{
    dst = src;
    return true;
}

bool
DefaultCoordSystemConverter::dst2src( const PointN & src, PointN & dst )
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
#endif
