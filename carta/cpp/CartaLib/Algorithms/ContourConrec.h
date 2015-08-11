/**
 * Calculate contours in 2d array
 *
 * see http://paulbourke.net/papers/conrec/
 *
 **/

#pragma once

#include "CartaLib/IImage.h"
#include <vector>
#include <functional>
#include <QPolygonF>

namespace Carta
{
namespace Lib
{
namespace Algorithms
{

class ContourConrec
{
public:

    /// the result of the algorithm is a list of contour sets for each requested
    /// level. Each contour set is in turn a list of poly-lines.
    typedef std::vector < std::vector < QPolygonF > > Result;

    /// initiate algorithm
    ContourConrec();

    /// specify levels for which to generate contours
    void
    setLevels( const std::vector < double > & levels );

    /// compute and return the sorted vertices
    Result
    compute( NdArray::RawViewInterface * );

private:

    std::vector < double > m_levels;
};

}
}
}
