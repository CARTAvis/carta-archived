/**
 *
 **/

#include "LineCombiner.h"
#include "CartaLib/CartaLib.h"

#include <QLinkedList>

#ifdef qDebug
#undef qDebug
#endif
#define qDebug QT_NO_QDEBUG_MACRO

namespace Carta
{
namespace Lib
{
namespace Algorithms
{
LineCombiner::LineCombiner( const QRectF & rect, int rows, int cols, double threshold )
{
    m_rect = rect;
    m_nRows = rows;
    m_nCols = cols;
    m_thresholdSq = threshold * threshold;

    Cell * rawPtr = new Cell[m_nRows * m_nCols];

    m_grid.resize( m_nRows );
    for ( int row = 0 ; row < m_nRows ; ++row ) {
        m_grid[row] = rawPtr + row * m_nCols;
    }
}

LineCombiner::~LineCombiner()
{
    if ( m_grid.size() > 0 ) {
        delete[] m_grid[0];
    }
}

//void LineCombiner::setColsRows(int cols, int rows)
//{
//    CARTA_ASSERT( m_grid.size() == 0);

//    m_nCols = cols;
//    m_nRows = rows;

//    Cell * rawPtr = new Cell[ m_nRows * m_nCols];

//    m_grid.resize( m_nRows);
//    for( int row = 0 ; row < m_nRows ; ++ row){
//        m_grid[row] = rawPtr + row * m_nCols;
//    }
//}

void
LineCombiner::add( QPointF p1, QPointF p2 )
{
    bool ok;

    qDebug() << "add" << p1 << p2;

    // find closest points within the threshold distance of p1 and p2
    IndexPt * ip1 = _findClosestPt( p1 );
    IndexPt * ip2 = _findClosestPt( p2 );

    // normalize the cases
    if ( ! ip1 ) {
        std::swap( ip1, ip2 );
        std::swap( p1, p2 );
    }

    if ( ip1 ) {
        CARTA_ASSERT( ip1-> poly );
    }
    if ( ip2 ) {
        CARTA_ASSERT( ip2-> poly );
    }

    // case1: this line segment is not near anything else
    if ( ip1 == nullptr && ip2 == nullptr ) {
        qDebug() << "case null null";

        // we insert a new polyline and update spacial index

        // make a new polyline from p1 and p2
        Poly * poly = new Poly;
        poly->append( p1 );
        poly->append( p2 );

        // insert beginning of this polyline into grid
        findCell( p1 )-> pts.append( IndexPt( poly, false ) );

        // insert end of this polyine into grid
        findCell( p2 )-> pts.append( IndexPt( poly, true ) );

        IndexPt ipt1( poly, true );
        CARTA_ASSERT( findCell( ipt1.pt() )->pts.contains( ipt1 ) );

        return;
    }

    // catch a super-special case.... both points point to the same polyline, same end...
    // we'll treat this as if only one of the points pointed to a polyline)
    if( ip2 && ip1->poly == ip2->poly && ip1->flipped == ip2->flipped) {
        qDebug() << "super special";
        ip2 = nullptr;
    }

    // only one point has a match (ip1, ip2 is null)
    if ( ip1 != nullptr && ip2 == nullptr ) {
        qDebug() << "case poly null";

        // make a copy of what ip1 points to, because it'll be destroyed
        IndexPt ip1copy = * ip1;

        // remove ip1 from it's corresponding cell (after this ip1 will point to
        // a destroyed memory!)
        ok = findCell( ip1-> pt() )-> pts.removeOne( * ip1 );
        CARTA_ASSERT( ok );

        // re-point ip1 to the copy
        ip1 = & ip1copy;

        // we extend the polyline that ip1 points to with p2
        if ( ip1-> flipped ) {
            ip1-> poly-> append( p2 );
        }
        else {
            ip1-> poly-> prepend( p2 );
        }

        // and add a new index point (for p2) to the respective cell
        findCell( p2 )-> pts.append( * ip1 );
        return;
    }

    // both points have a match, and it's the same polyline, but different ends...
    if ( ip1-> poly == ip2-> poly ) {
        qDebug() << "case poly poly same";

        CARTA_ASSERT( ip1->flipped == ! ip2->flipped );

        // we need to remove both points from their cells
        Poly * poly = ip1->poly;
        ok = findCell( ip1->pt() )->pts.removeOne( * ip1 );
        CARTA_ASSERT( ok );
        ok = findCell( ip2->pt() )->pts.removeOne( * ip2 );
        CARTA_ASSERT( ok );

        // make it a closed polyline
        poly->append( poly->first() );

        QPolygonF polygon = poly2polygon( poly );
        m_polygons.push_back( polygon );
        delete poly;

        return;
    }

    // last case is: both points have a match to 2 different polylines
    qDebug() << "case poly poly diff";

    // we need to merge these two polylines together
    IndexPt ip1c = * ip1;
    IndexPt ip2c = * ip2;

    // remove first polyline from the spatial index
    ok = findCell( ip1c.poly->front() )-> pts.removeAll( { ip1c.poly, false }
                                                         );
    CARTA_ASSERT( ok );
    ok = findCell( ip1c.poly->back() )-> pts.removeAll( { ip1c.poly, true }
                                                        );
    CARTA_ASSERT( ok );

    // remove second polyline from the spatial index
    ok = findCell( ip2c.poly->front() )-> pts.removeAll( { ip2c.poly, false }
                                                         );
    CARTA_ASSERT( ok );
    ok = findCell( ip2c.poly->back() )-> pts.removeAll( { ip2c.poly, true }
                                                        );
    CARTA_ASSERT( ok );

    // we need to handle 4 cases for merging... in any case, we'll be re-using poly1 and
    // appending/prepending to it all elements from poly2
    // case1: append poly2 to the end of poly1, in forward order
    if ( ip1c.flipped && ! ip2c.flipped ) {
        qDebug() << "subcase1 - append forward";
        for ( auto & pt : * ip2c.poly ) {
            ip1c.poly->append( pt );
        }
    }
    else if ( ip1c.flipped && ip2c.flipped ) {
        qDebug() << "subcase2 - append reverse";
        QLinkedListIterator < QPointF > i( * ip2c.poly );
        i.toBack();
        while ( i.hasPrevious() ) {
            ip1c.poly-> append( i.previous() );
        }
    }
    else if ( ! ip1c.flipped && ! ip2c.flipped ) {
        qDebug() << "subase3 - prepend forward";
        for ( auto & pt : * ip2c.poly ) {
            ip1c.poly->prepend( pt );
        }
    }
    else {
        qDebug() << "subase4 - prepend reverse";
        QLinkedListIterator < QPointF > i( * ip2c.poly );
        i.toBack();
        while ( i.hasPrevious() ) {
            ip1c.poly-> prepend( i.previous() );
        }
    }

    // get rid of poly2
    delete ip2c.poly;

    // re-insert the endpoints of poly1 into spatial index
    ip1c = IndexPt( ip1c.poly, false );
    ip2c = IndexPt( ip1c.poly, true );
    findCell( ip1c.poly->first() )->pts.append( ip1c );
    findCell( ip1c.poly->last() )->pts.append( ip2c );
} // add

std::vector < QPolygonF >
LineCombiner::getPolygons()
{
    // collect all polylines that are still in grid cells
    for ( int row = 0 ; row < m_nRows ; ++row ) {
        for ( int col = 0 ; col < m_nCols ; ++col ) {
            for ( IndexPt & ipt : cell( row, col ).pts ) {
                // only consider non-flipped pts, since there will always be 2 per polyline
                if ( ipt.flipped ) {
                    qDebug() << "Skipping " << ipt.poly;
                    continue;
                }

                // convert the polyline to QPolygonF
                QPolygonF pf = poly2polygon( ipt.poly );

                // delete the polyline
                qDebug() << "Deleting poly" << ipt.poly;

                delete ipt.poly;

                // add the new polygon to our list
                m_polygons.push_back( pf );
            }
            cell( row, col ).pts.clear();
        }
    }
    return m_polygons;
} // add

void
LineCombiner::pt2rowcol( const QPointF & p, int & row, int & col )
{
    double cellx = m_rect.width() / m_nCols;
    double celly = m_rect.height() / m_nRows;
    col = ( p.x() - m_rect.left() ) / cellx;
    row = ( p.y() - m_rect.top() ) / celly;

    if ( col < 0 ) {
        col = 0;
    }
    if ( col >= m_nCols ) {
        col = m_nCols-1;
    }
    if ( row < 0 ) {
        row = 0;
    }
    if ( row >= m_nRows ) {
        row = m_nRows-1;
    }
} // pt2rowcol

LineCombiner::Cell *
LineCombiner::findCell( const QPointF & pt )
{
    int row, col;
    pt2rowcol( pt, row, col );
    return & cell( row, col );
}

LineCombiner::Cell &
LineCombiner::cell( int row, int col )
{
    CARTA_ASSERT( row >= 0 );
    CARTA_ASSERT( col >= 0 );
    CARTA_ASSERT( row < m_nRows );
    CARTA_ASSERT( col < m_nCols );
    CARTA_ASSERT( m_grid.size() > 0 );
    return m_grid[row][col];
}

QPolygonF
LineCombiner::poly2polygon( LineCombiner::Poly * poly )
{
    QPolygonF pf;
    for ( QPointF & pt : * poly ) {
        pf.append( pt );
    }
    return pf;
}

LineCombiner::IndexPt *
LineCombiner::_findClosestPt( const QPointF & p )
{
    // find the row/column of the grid cell containing this point
    int row, col;
    pt2rowcol( p, row, col );

    // we'll be searching 3x3 cells around row/col
    double bestDist = - 1.0;
    IndexPt * result = nullptr;
    for ( int r = row - 1 ; r <= row + 1 ; ++r ) {
        if ( r < 0 || r >= m_nRows ) {
            continue;
        }
        for ( int c = col - 1 ; c <= col ; ++c ) {
            if ( c < 0 || c >= m_nCols ) {
                continue;
            }
            Cell & cell = this-> cell( r, c );
            for ( auto & ipt : cell.pts ) {
                QPointF & pt = ipt.pt();
                double dx = pt.x() - p.x();
                double dy = pt.y() - p.y();
                double dsq = dx * dx + dy * dy;
                if ( dsq < m_thresholdSq && ( dsq < bestDist || bestDist < 0 ) ) {
                    bestDist = dsq;
                    result = & ipt;
                }
            }
        }
    }

    return result;
} // _findClosestPt
}
}
}
