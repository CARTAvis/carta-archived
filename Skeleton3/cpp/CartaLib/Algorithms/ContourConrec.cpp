/**
 *
 **/

#include "ContourConrec.h"
#include "IImage.h"

#include <QString>
#include <QDebug>

typedef std::vector < double > VD;

/*
 * The code below is modified version of Paul Bourke's algorithm:
 *
 * http://paulbourke.net/papers/conrec/
 *
 */

/*
   Derivation from the fortran version of CONREC by Paul Bourke
   data            ! matrix of data to contour
   ilb,iub         ! bounds for first coordinate (column), inclusive
   ilb,iub,jlb,jub ! bounds for second coordinate (row), inclusive
   xCoords         ! column coordinates (first index)
   yCoords         ! row coordinates (second index)
   nc              ! number of contour levels
   z               ! contour levels in increasing order
*/
static Carta::Lib::Algorithms::ContourConrec::Result
conrec(

//    Carta::Lib::Algorithms::ContourConrec::DataAccessor acc,
    NdArray::RawViewInterface * view,

//        double * * data,
    int ilb,
    int iub,
    int jlb,
    int jub,
    const VD & xCoords,
    const VD & yCoords,
    int nc,
    double * z
    )
{
    NdArray::Double doubleView( view, false );
    auto acc = [& doubleView] ( int col, int row ) {
        return doubleView.get( { col, row }
                               );
    };

    Carta::Lib::Algorithms::ContourConrec::Result result;
    if ( nc < 1 ) {
        return result;
    }
    result.resize( nc );

#define xsect( p1, p2 ) ( h[p2] * xh[p1] - h[p1] * xh[p2] ) / ( h[p2] - h[p1] )
#define ysect( p1, p2 ) ( h[p2] * yh[p1] - h[p1] * yh[p2] ) / ( h[p2] - h[p1] )

    int m1, m2, m3, case_value;
    double dmin, dmax, x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    int i, j, k, m;
    double h[5];
    int sh[5];
    double xh[5], yh[5];
    int im[4] = {
        0, 1, 1, 0
    }, jm[4] = {
        0, 0, 1, 1
    };
    int castab[3][3][3] = {
        { { 0, 0, 8 }, { 0, 2, 5 }, { 7, 6, 9 } },
        { { 0, 3, 4 }, { 1, 3, 1 }, { 4, 3, 0 } },
        { { 9, 6, 7 }, { 5, 2, 0 }, { 8, 0, 0 } }
    };
    double temp1, temp2;

    // original code went from bottom to top, not sure why
    //    for ( j = ( jub - 1 ) ; j >= jlb ; j-- ) {
    for ( j = jlb ; j < jub ; j++ ) {
        qDebug() << "xyz contouring row" << j;
        for ( i = ilb ; i < iub ; i++ ) {
            temp1 = std::min( acc( i, j ), acc( i, j + 1 ) );
            temp2 = std::min( acc( i + 1, j ), acc( i + 1, j + 1 ) );
            dmin = std::min( temp1, temp2 );
            temp1 = std::max( acc( i, j ), acc( i, j + 1 ) );
            temp2 = std::max( acc( i + 1, j ), acc( i + 1, j + 1 ) );
            dmax = std::max( temp1, temp2 );
            if ( dmax < z[0] || dmin > z[nc - 1] ) {
                continue;
            }
            for ( k = 0 ; k < nc ; k++ ) {
                if ( z[k] < dmin || z[k] > dmax ) {
                    continue;
                }
                for ( m = 4 ; m >= 0 ; m-- ) {
                    if ( m > 0 ) {
                        h[m] = acc( i + im[m - 1], j + jm[m - 1] ) - z[k];
                        xh[m] = xCoords[i + im[m - 1]];
                        yh[m] = yCoords[j + jm[m - 1]];
                    }
                    else {
                        h[0] = 0.25 * ( h[1] + h[2] + h[3] + h[4] );
                        xh[0] = 0.50 * ( xCoords[i] + xCoords[i + 1] );
                        yh[0] = 0.50 * ( yCoords[j] + yCoords[j + 1] );
                    }
                    if ( h[m] > 0.0 ) {
                        sh[m] = 1;
                    }
                    else if ( h[m] < 0.0 ) {
                        sh[m] = - 1;
                    }
                    else {
                        sh[m] = 0;
                    }
                }

                /*
                   Note: at this stage the relative heights of the corners and the
                   centre are in the h array, and the corresponding coordinates are
                   in the xh and yh arrays. The centre of the box is indexed by 0
                   and the 4 corners by 1 to 4 as shown below.
                   Each triangle is then indexed by the parameter m, and the 3
                   vertices of each triangle are indexed by parameters m1,m2,and m3.
                   It is assumed that the centre of the box is always vertex 2
                   though this isimportant only when all 3 vertices lie exactly on
                   the same contour level, in which case only the side of the box
                   is drawn.
                      vertex 4 +-------------------+ vertex 3
                               | \               / |
                               |   \    m-3    /   |
                               |     \       /     |
                               |       \   /       |
                               |  m=2    X   m=2   |       the centre is vertex 0
                               |       /   \       |
                               |     /       \     |
                               |   /    m=1    \   |
                               | /               \ |
                      vertex 1 +-------------------+ vertex 2
                */
                /* Scan each triangle in the box */
                for ( m = 1 ; m <= 4 ; m++ ) {
                    m1 = m;
                    m2 = 0;
                    if ( m != 4 ) {
                        m3 = m + 1;
                    }
                    else {
                        m3 = 1;
                    }
                    if ( ( case_value = castab[sh[m1] + 1][sh[m2] + 1][sh[m3] + 1] ) == 0 ) {
                        continue;
                    }
                    switch ( case_value )
                    {
                    case 1 : /* Line between vertices 1 and 2 */
                        x1 = xh[m1];
                        y1 = yh[m1];
                        x2 = xh[m2];
                        y2 = yh[m2];
                        break;
                    case 2 : /* Line between vertices 2 and 3 */
                        x1 = xh[m2];
                        y1 = yh[m2];
                        x2 = xh[m3];
                        y2 = yh[m3];
                        break;
                    case 3 : /* Line between vertices 3 and 1 */
                        x1 = xh[m3];
                        y1 = yh[m3];
                        x2 = xh[m1];
                        y2 = yh[m1];
                        break;
                    case 4 : /* Line between vertex 1 and side 2-3 */
                        x1 = xh[m1];
                        y1 = yh[m1];
                        x2 = xsect( m2, m3 );
                        y2 = ysect( m2, m3 );
                        break;
                    case 5 : /* Line between vertex 2 and side 3-1 */
                        x1 = xh[m2];
                        y1 = yh[m2];
                        x2 = xsect( m3, m1 );
                        y2 = ysect( m3, m1 );
                        break;
                    case 6 : /* Line between vertex 3 and side 1-2 */
                        x1 = xh[m3];
                        y1 = yh[m3];
                        x2 = xsect( m1, m2 );
                        y2 = ysect( m1, m2 );
                        break;
                    case 7 : /* Line between sides 1-2 and 2-3 */
                        x1 = xsect( m1, m2 );
                        y1 = ysect( m1, m2 );
                        x2 = xsect( m2, m3 );
                        y2 = ysect( m2, m3 );
                        break;
                    case 8 : /* Line between sides 2-3 and 3-1 */
                        x1 = xsect( m2, m3 );
                        y1 = ysect( m2, m3 );
                        x2 = xsect( m3, m1 );
                        y2 = ysect( m3, m1 );
                        break;
                    case 9 : /* Line between sides 3-1 and 1-2 */
                        x1 = xsect( m3, m1 );
                        y1 = ysect( m3, m1 );
                        x2 = xsect( m1, m2 );
                        y2 = ysect( m1, m2 );
                        break;
                    default :
                        break;
                    } // switch

                    // add the line segment to the result
                    // ConrecLine( x1, y1, x2, y2, k );
                    if ( std::isfinite( x1 ) && std::isfinite( y1 ) && std::isfinite( x2 ) &&
                         std::isfinite( y2 ) ) {
                        QPolygonF poly;
                        poly.append( QPointF( x1, y1 ) );
                        poly.append( QPointF( x2, y2 ) );
                        result[k].push_back( poly );
                    }
                } /* m */
            } /* k - contour */
        } /* i */
    } /* j */
    return result;
} // conrec

static Carta::Lib::Algorithms::ContourConrec::Result
conrecFaster(
    NdArray::RawViewInterface * view,
    int ilb,
    int iub,
    int jlb,
    int jub,
    const VD & xCoords,
    const VD & yCoords,
    int nc,
    double * z
    )
{
    // we will only need two rows in memory at any given time
    int nRows = jub - jlb + 1;
    int nCols = iub - ilb + 1;
    double * rows[2] {
        nullptr, nullptr
    };
    std::vector < double > row1( nCols ), row2( nCols );
    rows[0] = & row1[0];
    rows[1] = & row2[0];
    int nextRowToReadIn = 0;

    auto updateRows = [&] () -> void {
        CARTA_ASSERT( nextRowToReadIn < view-> dims()[1] );

        // make a row view into the view
        SliceND rowSlice;
        rowSlice.next().start( nextRowToReadIn ).end( nextRowToReadIn + 1 );
        auto rawRowView = view-> getView( rowSlice );
        nextRowToReadIn++;

        // make a double view of this raw row view
        NdArray::Double dview( rawRowView, false );

        // shift the row up
        // note: we could avoid this extra memory copy if we swapped row[] pointers instead,
        // and alternately read in the data into row1,row2...
        row1 = row2;

        // read in the data into row2
        int i = 0;
        dview.forEach([&] ( const double & val ) {
                          row2[i++] = val;
                      }
                      );
        qDebug() << "final i=" << i;
        CARTA_ASSERT( i == nCols );
    };
    updateRows();

//    NdArray::Double doubleView( view, false );
//    auto acc = [& doubleView] ( int col, int row ) {
//        return doubleView.get( { col, row }
//                               );
//    };

    auto acc = [&] ( int col, int row ) {
        row -= nextRowToReadIn - 2;
        return rows[row][col];
    };

    Carta::Lib::Algorithms::ContourConrec::Result result;
    if ( nc < 1 ) {
        return result;
    }
    result.resize( nc );

#define xsect( p1, p2 ) ( h[p2] * xh[p1] - h[p1] * xh[p2] ) / ( h[p2] - h[p1] )
#define ysect( p1, p2 ) ( h[p2] * yh[p1] - h[p1] * yh[p2] ) / ( h[p2] - h[p1] )

    int m1, m2, m3, case_value;
    double dmin, dmax, x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    int i, j, k, m;
    double h[5];
    int sh[5];
    double xh[5], yh[5];
    int im[4] = {
        0, 1, 1, 0
    }, jm[4] = {
        0, 0, 1, 1
    };
    int castab[3][3][3] = {
        { { 0, 0, 8 }, { 0, 2, 5 }, { 7, 6, 9 } },
        { { 0, 3, 4 }, { 1, 3, 1 }, { 4, 3, 0 } },
        { { 9, 6, 7 }, { 5, 2, 0 }, { 8, 0, 0 } }
    };
    double temp1, temp2;

    // original code went from bottom to top, not sure why
    //    for ( j = ( jub - 1 ) ; j >= jlb ; j-- ) {
    for ( j = jlb ; j < jub ; j++ ) {
        updateRows();
        qDebug() << "xyz contouring row" << j;
        for ( i = ilb ; i < iub ; i++ ) {
            temp1 = std::min( acc( i, j ), acc( i, j + 1 ) );
            temp2 = std::min( acc( i + 1, j ), acc( i + 1, j + 1 ) );
            dmin = std::min( temp1, temp2 );
            temp1 = std::max( acc( i, j ), acc( i, j + 1 ) );
            temp2 = std::max( acc( i + 1, j ), acc( i + 1, j + 1 ) );
            dmax = std::max( temp1, temp2 );
            if ( dmax < z[0] || dmin > z[nc - 1] ) {
                continue;
            }
            for ( k = 0 ; k < nc ; k++ ) {
                if ( z[k] < dmin || z[k] > dmax ) {
                    continue;
                }
                for ( m = 4 ; m >= 0 ; m-- ) {
                    if ( m > 0 ) {
                        h[m] = acc( i + im[m - 1], j + jm[m - 1] ) - z[k];
                        xh[m] = xCoords[i + im[m - 1]];
                        yh[m] = yCoords[j + jm[m - 1]];
                    }
                    else {
                        h[0] = 0.25 * ( h[1] + h[2] + h[3] + h[4] );
                        xh[0] = 0.50 * ( xCoords[i] + xCoords[i + 1] );
                        yh[0] = 0.50 * ( yCoords[j] + yCoords[j + 1] );
                    }
                    if ( h[m] > 0.0 ) {
                        sh[m] = 1;
                    }
                    else if ( h[m] < 0.0 ) {
                        sh[m] = - 1;
                    }
                    else {
                        sh[m] = 0;
                    }
                }

                /*
                   Note: at this stage the relative heights of the corners and the
                   centre are in the h array, and the corresponding coordinates are
                   in the xh and yh arrays. The centre of the box is indexed by 0
                   and the 4 corners by 1 to 4 as shown below.
                   Each triangle is then indexed by the parameter m, and the 3
                   vertices of each triangle are indexed by parameters m1,m2,and m3.
                   It is assumed that the centre of the box is always vertex 2
                   though this isimportant only when all 3 vertices lie exactly on
                   the same contour level, in which case only the side of the box
                   is drawn.
                      vertex 4 +-------------------+ vertex 3
                               | \               / |
                               |   \    m-3    /   |
                               |     \       /     |
                               |       \   /       |
                               |  m=2    X   m=2   |       the centre is vertex 0
                               |       /   \       |
                               |     /       \     |
                               |   /    m=1    \   |
                               | /               \ |
                      vertex 1 +-------------------+ vertex 2
                */
                /* Scan each triangle in the box */
                for ( m = 1 ; m <= 4 ; m++ ) {
                    m1 = m;
                    m2 = 0;
                    if ( m != 4 ) {
                        m3 = m + 1;
                    }
                    else {
                        m3 = 1;
                    }
                    if ( ( case_value = castab[sh[m1] + 1][sh[m2] + 1][sh[m3] + 1] ) == 0 ) {
                        continue;
                    }
                    switch ( case_value )
                    {
                    case 1 : /* Line between vertices 1 and 2 */
                        x1 = xh[m1];
                        y1 = yh[m1];
                        x2 = xh[m2];
                        y2 = yh[m2];
                        break;
                    case 2 : /* Line between vertices 2 and 3 */
                        x1 = xh[m2];
                        y1 = yh[m2];
                        x2 = xh[m3];
                        y2 = yh[m3];
                        break;
                    case 3 : /* Line between vertices 3 and 1 */
                        x1 = xh[m3];
                        y1 = yh[m3];
                        x2 = xh[m1];
                        y2 = yh[m1];
                        break;
                    case 4 : /* Line between vertex 1 and side 2-3 */
                        x1 = xh[m1];
                        y1 = yh[m1];
                        x2 = xsect( m2, m3 );
                        y2 = ysect( m2, m3 );
                        break;
                    case 5 : /* Line between vertex 2 and side 3-1 */
                        x1 = xh[m2];
                        y1 = yh[m2];
                        x2 = xsect( m3, m1 );
                        y2 = ysect( m3, m1 );
                        break;
                    case 6 : /* Line between vertex 3 and side 1-2 */
                        x1 = xh[m3];
                        y1 = yh[m3];
                        x2 = xsect( m1, m2 );
                        y2 = ysect( m1, m2 );
                        break;
                    case 7 : /* Line between sides 1-2 and 2-3 */
                        x1 = xsect( m1, m2 );
                        y1 = ysect( m1, m2 );
                        x2 = xsect( m2, m3 );
                        y2 = ysect( m2, m3 );
                        break;
                    case 8 : /* Line between sides 2-3 and 3-1 */
                        x1 = xsect( m2, m3 );
                        y1 = ysect( m2, m3 );
                        x2 = xsect( m3, m1 );
                        y2 = ysect( m3, m1 );
                        break;
                    case 9 : /* Line between sides 3-1 and 1-2 */
                        x1 = xsect( m3, m1 );
                        y1 = ysect( m3, m1 );
                        x2 = xsect( m1, m2 );
                        y2 = ysect( m1, m2 );
                        break;
                    default :
                        break;
                    } // switch

                    // add the line segment to the result
                    // ConrecLine( x1, y1, x2, y2, k );
                    if ( std::isfinite( x1 ) && std::isfinite( y1 ) && std::isfinite( x2 ) &&
                         std::isfinite( y2 ) ) {
                        QPolygonF poly;
                        poly.append( QPointF( x1, y1 ) );
                        poly.append( QPointF( x2, y2 ) );
                        result[k].push_back( poly );
                    }
                } /* m */
            } /* k - contour */
        } /* i */
    } /* j */
    return result;
} // conrecFaster

namespace Carta
{
namespace Lib
{
namespace Algorithms
{
ContourConrec::ContourConrec()
{ }

void
ContourConrec::setLevels( const std::vector < double > & levels )
{
    m_levels = levels;
}

//void
//ContourConrec::setInputDataSize( int64_t nRows, int64_t nCols )
//{
//    m_nRows = nRows;
//    m_nCols = nCols;
//}

/*

ContourConrec::Result
ContourConrec::compute( ContourConrec::DataAccessor & acc )
{
    {
        QString s;
        for ( auto lv : m_levels ) {
            s += QString::number( lv ) + " ";
        }
        qDebug() << "xyz computing contours" << s;
    }

    // prepare the 2d array ala c-style so we can call the original algorithm
    // the original assumes data in column/row order.... probably because it was
    // converted from fortran?

    // first make a contiguous copy of the data in some buffer
    VD buffer( m_nRows * m_nCols );
    size_t buffInd = 0;
    for ( int col = 0 ; col < m_nCols ; ++col ) {
        for ( int row = 0 ; row < m_nRows ; ++row ) {
            buffer[buffInd++] = acc( row, col );
        }
    }

    // now make a '2d' array that we'll be passing to the c algorithm
    std::vector < double * > data( m_nCols );

    for ( int col = 0 ; col < m_nCols ; ++col ) {
        data[col] = & buffer[col * m_nRows];
    }

    // make x coordinates
    VD xcoords( m_nCols );
    for ( int col = 0 ; col < m_nCols ; ++col ) {
        xcoords[col] = col;
    }

    // make y coordinates
    VD ycoords( m_nRows );
    for ( int row = 0 ; row < m_nRows ; ++row ) {
        ycoords[row] = row;
    }

    Carta::Lib::Algorithms::ContourConrec::DataAccessor dacc =
    [& acc] ( int col, int row ) -> double
    {
        return acc( row, col );
    };

    Result result =
        conrec(

//                & data[0],
            dacc,
            0,
            m_nCols - 1,
            0,
            m_nRows - 1,

//                & xcoords[0],
//                & ycoords[0],
            xcoords,
            ycoords,
            m_levels.size(),
            & m_levels[0] );

    return result;

    return Result();
}

*/

ContourConrec::Result
ContourConrec::compute2( NdArray::RawViewInterface * view )
{
//    NdArray::Double doubleView( view, false );
//    setInputDataSize( view-> dims()[1], view-> dims()[0]);
//    DataAccessor da =
//            [ & doubleView]( int row, int col) { return doubleView.get( { col, row}); };
//    return compute( da);

//    setInputDataSize( view-> dims()[1], view-> dims()[0]);
    auto m_nRows = view-> dims()[1];
    auto m_nCols = view-> dims()[0];

    // make x coordinates
    VD xcoords( m_nCols );
    for ( int col = 0 ; col < m_nCols ; ++col ) {
        xcoords[col] = col;
    }

    // make y coordinates
    VD ycoords( m_nRows );
    for ( int row = 0 ; row < m_nRows ; ++row ) {
        ycoords[row] = row;
    }

    Result result =
        conrecFaster(
            view,
            0,
            m_nCols - 1,
            0,
            m_nRows - 1,

//                & xcoords[0],
//                & ycoords[0],
            xcoords,
            ycoords,
            m_levels.size(),
            & m_levels[0] );

    return result;
} // compute
}
}
}
