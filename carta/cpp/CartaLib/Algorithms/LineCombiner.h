/**
 *
 **/

#pragma once

class QRectF;
class QPointF;
#include <QLinkedList>
#include <QPolygonF>
#include <vector>
namespace Carta
{
namespace Lib
{
namespace Algorithms
{
class LineCombiner
{
public:

    LineCombiner( const QRectF & rect, int rows, int cols, double threshold);
    ~LineCombiner();

//    void
//    setRect( const QRectF & rect )
//    {
//        m_rect = rect;
//    }

//    void
//    setThreshold( double threshold )
//    {
//        m_thresholdSq = threshold * threshold;
//    }

//    void
//    setColsRows( int cols, int rows );

    void
    setSmallestY( double y );

    void
    add( QPointF p1, QPointF p2 );

    std::vector < QPolygonF >
    getPolygons();

private:

    typedef QLinkedList < QPointF > Poly;

    struct IndexPt {
        Poly * poly = nullptr;
        bool flipped = false;

        QPointF &
        pt()
        {
            if ( flipped ) {
                return poly-> last();
            }
            else {
                return poly-> first();
            }
        }

        IndexPt( Poly * p, bool f) { poly = p; flipped = f; }
        bool operator ==( const IndexPt & pt) {
            return pt.poly == poly && pt.flipped == flipped;
        }
    };


    IndexPt *
    _findClosestPt( const QPointF & p );

    int m_nRows, m_nCols;

    double m_thresholdSq = 1e-9;
    void
    pt2rowcol( const QPointF & p, int & row, int & col );

    struct Cell {
        QLinkedList < IndexPt > pts;
    };
    std::vector< Cell * > m_grid;
    QRectF m_rect; // bounding rect

    std::vector<QPolygonF> m_polygons;

    Cell * findCell( const QPointF & pt);

    Cell & cell( int row, int col);

    static QPolygonF poly2polygon( Poly * poly);
};
}
}
}
