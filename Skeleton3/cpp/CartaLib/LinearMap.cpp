/**
 *
 **/

#include "LinearMap.h"

namespace Carta
{
namespace Lib
{
LinearMap1D
LinearMap1D::createRaw( double ax, double bx )
{
    LinearMap1D res;
    res.m_ax = ax;
    res.m_bx = bx;
    res.m_axinv = 1.0 / ax;
    return res;
}
}
}
