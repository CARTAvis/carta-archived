/**
 * Calculate contours in 2d array
 *
 * see http://paulbourke.net/papers/conrec/
 *
 **/

#pragma once

#include <vector>
#include <functional>
#include <QPolygonF>

namespace Carta
{
namespace Lib
{
namespace Algorithms
{
//template < typename SequentialDataAccessor >
class ContourConrec
{
public:

    /// the result of the algorithm is a list of contour sets for each requested
    /// level. Each contour set is in turn a list of poly-lines.
    typedef std::vector < std::vector < QPolygonF > > Result;

    /// the data is accessed via
    typedef std::function < double ( int, int ) > DataAccessor;

    /// initiate algorithm
    ContourConrec();

    /// specify requested levels
    void
    setLevels( const std::vector < double > & levels );

    /// specify input data size
    void
    setInputDataSize( int64_t nRows, int64_t nCols );

    /// compute and return the sorted vertices
    Result
    compute( DataAccessor & acc );

private:

    int m_nRows = 0;
    int m_nCols = 0;
    std::vector < double > m_levels;
};
}
}
}
