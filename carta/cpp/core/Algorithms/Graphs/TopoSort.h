/**
 * Topological sort algorithm.
 *
 * It essentially implements the algorithm from the wikipedia:
 *
 * L  Empty list that will contain the sorted elements
 * S  Set of all nodes with no incoming edges
 * while S is non-empty do
 *     remove a node n from S
 *     add n to tail of L
 *     for each node m with an edge e from n to m do
 *         remove edge e from the graph
 *         if m has no other incoming edges then
 *             insert m into S
 * if graph has edges then
 *     return error (graph has at least one cycle)
 * else
 *     return L (a topologically sorted order)
 *
 **/

#pragma once

#include <vector>

namespace Carta
{
namespace Core
{
namespace Algorithms
{
namespace Graphs
{
class TopoSort
{
public:

    typedef std::vector < int > VI;

    /// initiate algorithm with n vertices, numbered from 0..(n-1)
    TopoSort( int n );

    /// add an arrow from vertex a to b
    void
    addArrow( int a, int b );

    /// compute and return the sorted vertices
    VI
    compute();

protected:

    /// counts of incoming arrows for each vertex
    VI m_counts;

    /// for each vertex we keep a list of outgoing arrows
    std::vector < VI > m_arrows;
};
}
}
}
}
