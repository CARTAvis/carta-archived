/**
 *
 **/

#include "TopoSort.h"
#include <QDebug>
#include <stack>

namespace Carta
{
namespace Core
{
Algorithms::Graphs::TopoSort::TopoSort( int n )
{
    Q_ASSERT( n >= 0 );
    m_counts.resize( n, 0 );
    m_arrows.resize( n, VI() );
}

void
Algorithms::Graphs::TopoSort::addArrow( int a, int b )
{
    Q_ASSERT( a >= 0 && b >= 0 );
    Q_ASSERT( size_t( a ) < m_counts.size() && size_t( b ) < m_counts.size() );
    Q_ASSERT( a != b );

    // add the arrow
    m_arrows[a].push_back( b );

    // increment the count
    m_counts[b]++;
}

Algorithms::Graphs::TopoSort::VI
Algorithms::Graphs::TopoSort::compute()
{
    const size_t n = m_counts.size();

    // initialize the list of vertices with currently 0 count
    VI result;
    std::stack < int > zeroes;
    for ( size_t i = 0 ; i < n ; i++ ) {
        if ( m_counts[i] == 0 ) {
            zeroes.push( i );
        }
    }

    // main loop
    while ( 1 ) {
        // if we have nothing in zeroes, we are done with the loop
        if ( zeroes.empty() ) {
            break;
        }

        // get & remove the last entry from the zero list
        auto a = zeroes.top();
        zeroes.pop();

        // for every arrow from a, update the counter of the destination
        for ( auto b : m_arrows[a] ) {
            m_counts[b]--;

            // if the counter reached 0, add b to the zeros
            if ( m_counts[b] == 0 ) {
                zeroes.push( b );
            }
        }

        // add a to the results
        result.push_back( a );

        // we are done with this iteration, go try the next one
    }

    // if we still have unprocessed vertices, we have a cycle
    if ( result.size() != m_counts.size() ) {
        // we have a cycle
        qWarning() << "Cycle!!!";
        return VI();
    }

    return result;
} // compute
}
}
