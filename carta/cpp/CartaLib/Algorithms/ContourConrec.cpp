/**
 *
 **/

#include <cmath>
#include "ContourConrec.h"
#include "IImage.h"
#include "LineCombiner.h"
#include <cmath>
#include <QString>
#include <QDebug>

typedef std::vector < double > VD;

/*
 * The code below is modified version of Paul Bourke's algorithm:
 *
 * http://paulbourke.net/papers/conrec/
 *
 * The original is included at the bottom for reference, in case the web page
 * disappears.
 */
/*
   Derivation from the fortran version of CONREC by Paul Bourke
   view            ! view of the data
   ilb,iub         ! bounds for first coordinate (column), inclusive
   jlb,jub         ! bounds for second coordinate (row), inclusive
   xCoords         ! column coordinates (first index)
   yCoords         ! row coordinates (second index)
   nc              ! number of contour levels
   z               ! contour levels in increasing order
*/

static Carta::Lib::Algorithms::ContourConrec::Result
conrecFaster(
    Carta::Lib::NdArray::RawViewInterface * view,
    int ilb,
    int iub,
    int jlb,
    int jub,
    const VD & xCoords,
    const VD & yCoords,
    int nc,
    std::vector<double> z
    )
{
    // we will only need two rows in memory at any given time
//    int nRows = jub - jlb + 1;
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
        Carta::Lib::NdArray::Double dview( rawRowView, true );

        // shift the row up
        // note: we could avoid this memory copy if we swapped row[] pointers instead,
        // and alternately read in the data into row1,row2..., for a miniscule performance
        // gain and lot more complicated algorithm
        row1 = row2;

        // read in the data into row2
        int i = 0;
        dview.forEach([&] ( const double & val ) {
                          row2[i++] = val;
                      }
                      );
        CARTA_ASSERT( i == nCols );
    };
    updateRows(); // update the row number

//    NdArray::Double doubleView( view, false );
//    auto acc = [& doubleView] ( int col, int row ) {
//        return doubleView.get( { col, row }
//                               );
//    };

    // to keep the data accessor easy, we use this lambda, and hope the compiler
    // optimizes it into an inline expression... :)
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
        for ( i = ilb ; i < iub ; i++ ) {
            temp1 = std::min( acc( i, j ), acc( i, j + 1 ) );
            temp2 = std::min( acc( i + 1, j ), acc( i + 1, j + 1 ) );
            dmin = std::min( temp1, temp2 );
            // early abort if one of the values is not finite
            if ( ! std::isfinite( dmin ) ) {
                continue;
            }
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
                   though this is important only when all 3 vertices lie exactly on
                   the same contour level, in which case only the side of the box
                   is drawn.
                      vertex 4 +-------------------+ vertex 3
                               | \               / |
                               |   \    m=3    /   |
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

#undef xsect
#undef ysect
} // conrecFaster


    static Carta::Lib::Algorithms::ContourConrec::Result
    conrecFaster_Blur3(
        Carta::Lib::NdArray::RawViewInterface * view,
        int ilb,
        int iub,
        int jlb,
        int jub,
        const VD & xCoords,
        const VD & yCoords,
        int nc,
        std::vector<double> z,
        double kernel[9])
    {
        int nCols = iub - ilb + 1;
        double * rows[4] {
            nullptr, nullptr, nullptr, nullptr
        };
        std::vector <double> row1(nCols), row2(nCols), row3(nCols), row4(nCols);
        rows[0] = & row1[0];
        rows[1] = & row2[0];
        rows[2] = & row3[0];
        rows[3] = & row4[0];
        int nextRowToReadIn = 0;

        auto updateRows = [&] () -> void {
            CARTA_ASSERT( nextRowToReadIn < view-> dims()[1] );

            // make a row view into the view
            SliceND rowSlice;
            rowSlice.next().start( nextRowToReadIn ).end( nextRowToReadIn + 1 );
            auto rawRowView = view-> getView( rowSlice );
            nextRowToReadIn++;

            // make a double view of this raw row view
            Carta::Lib::NdArray::Double dview( rawRowView, true );

            // shift the row up
            row1 = row2;
            row2 = row3;
            row3 = row4;

            // read in the data into the last row
            int i = 0;
            dview.forEach([&] ( const double & val ) {
                              row4[i++] = val;
                          }
                          );
            CARTA_ASSERT( i == nCols );
        };
        updateRows(); // update the row number: n + 1
        updateRows(); // update the row number: n + 2
        updateRows(); // update the row number: n + 3

        // to keep the data accessor easy, we use this lambda, and hope the compiler
        // optimizes it into an inline expression... :)
        auto acc = [&] ( int col, int row ) {
            row -= nextRowToReadIn - 4;
            return rows[row-1][col-1]*kernel[0] + rows[row-1][col+0]*kernel[1] + rows[row-1][col+1]*kernel[2] +
                   rows[row+0][col-1]*kernel[3] + rows[row+0][col+0]*kernel[4] + rows[row+0][col+1]*kernel[5] +
                   rows[row+1][col-1]*kernel[6] + rows[row+1][col+0]*kernel[7] + rows[row+1][col+1]*kernel[8];
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

        for ( j = jlb+1 ; j < jub-1 ; j++ ) {
            updateRows(); // update the row number: n + 4
            for ( i = ilb+1 ; i < iub-1 ; i++ ) {
                temp1 = std::min( acc( i, j ), acc( i, j + 1 ) );
                temp2 = std::min( acc( i + 1, j ), acc( i + 1, j + 1 ) );
                dmin = std::min( temp1, temp2 );
                // early abort if one of the values is not finite
                if ( ! std::isfinite( dmin ) ) {
                    continue;
                }
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
                       though this is important only when all 3 vertices lie exactly on
                       the same contour level, in which case only the side of the box
                       is drawn.
                          vertex 4 +-------------------+ vertex 3
                                   | \               / |
                                   |   \    m=3    /   |
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

    #undef xsect
    #undef ysect
    } // conrecFaster_Blur3


    static Carta::Lib::Algorithms::ContourConrec::Result
    conrecFaster_Blur5(
        Carta::Lib::NdArray::RawViewInterface * view,
        int ilb,
        int iub,
        int jlb,
        int jub,
        const VD & xCoords,
        const VD & yCoords,
        int nc,
        std::vector<double> z,
        double kernel[25])
    {
        int nCols = iub - ilb + 1;
        double * rows[6] {
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
        };
        std::vector <double> row1(nCols), row2(nCols), row3(nCols), row4(nCols), row5(nCols), row6(nCols);
        rows[0] = & row1[0];
        rows[1] = & row2[0];
        rows[2] = & row3[0];
        rows[3] = & row4[0];
        rows[4] = & row5[0];
        rows[5] = & row6[0];
        int nextRowToReadIn = 0;

        auto updateRows = [&] () -> void {
            CARTA_ASSERT( nextRowToReadIn < view-> dims()[1] );

            // make a row view into the view
            SliceND rowSlice;
            rowSlice.next().start( nextRowToReadIn ).end( nextRowToReadIn + 1 );
            auto rawRowView = view-> getView( rowSlice );
            nextRowToReadIn++;

            // make a double view of this raw row view
            Carta::Lib::NdArray::Double dview( rawRowView, true );

            // shift the row up
            row1 = row2;
            row2 = row3;
            row3 = row4;
            row4 = row5;
            row5 = row6;

            // read in the data into the last row
            int i = 0;
            dview.forEach([&] ( const double & val ) {
                              row6[i++] = val;
                          }
                          );
            CARTA_ASSERT( i == nCols );
        };
        updateRows(); // update the row number: n + 1
        updateRows(); // update the row number: n + 2
        updateRows(); // update the row number: n + 3
        updateRows(); // update the row number: n + 4
        updateRows(); // update the row number: n + 5

        // to keep the data accessor easy, we use this lambda, and hope the compiler
        // optimizes it into an inline expression... :)
        //int tmpRow;
        auto acc = [&] ( int col, int row ) {
            //tmpRow = row;
            row -= nextRowToReadIn - 6;
            return rows[row-2][col-2]*kernel[0] + rows[row-2][col-1]*kernel[1] + rows[row-2][col+0]*kernel[2] + rows[row-2][col+1]*kernel[3] + rows[row-2][col+2]*kernel[4] +
                   rows[row-1][col-2]*kernel[5] + rows[row-1][col-1]*kernel[6] + rows[row-1][col+0]*kernel[7] + rows[row-1][col+1]*kernel[8] + rows[row-1][col+2]*kernel[9] +
                   rows[row+0][col-2]*kernel[10] + rows[row+0][col-1]*kernel[11] + rows[row+0][col+0]*kernel[12] + rows[row+0][col+1]*kernel[13] + rows[row+0][col+2]*kernel[14] +
                   rows[row+1][col-2]*kernel[15] + rows[row+1][col-1]*kernel[16] + rows[row+1][col+0]*kernel[17] + rows[row+1][col+1]*kernel[18] + rows[row+1][col+2]*kernel[19] +
                   rows[row+2][col-2]*kernel[20] + rows[row+2][col-1]*kernel[21] + rows[row+2][col+0]*kernel[22] + rows[row+2][col+1]*kernel[23] + rows[row+2][col+2]*kernel[24];
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

        for ( j = jlb+2 ; j < jub-2 ; j++ ) {
            updateRows(); // update the row number: n + 6
            for ( i = ilb+2 ; i < iub-2 ; i++ ) {
                temp1 = std::min( acc( i, j ), acc( i, j + 1 ) );
                temp2 = std::min( acc( i + 1, j ), acc( i + 1, j + 1 ) );
                dmin = std::min( temp1, temp2 );
                // early abort if one of the values is not finite
                if ( ! std::isfinite( dmin ) ) {
                    continue;
                }
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
                       though this is important only when all 3 vertices lie exactly on
                       the same contour level, in which case only the side of the box
                       is drawn.
                          vertex 4 +-------------------+ vertex 3
                                   | \               / |
                                   |   \    m=3    /   |
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

    #undef xsect
    #undef ysect
    } // conrecFaster_Blur5


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

ContourConrec::Result
ContourConrec::compute(NdArray::RawViewInterface * view, QString typeName)
{
    // if no input view was set, we are done
    if ( ! view || m_levels.size() == 0 ) {
        Result result( m_levels.size() );
        return result;
    }

    // the c-algorithm conrec() needs the levels in sorted order (to make things little
    // bit faster), but we would like to report the results in the same order that the
    // levels were requested. So we need to sort the levels, call the conrec(), and
    // then we need to 'unsort' the results...
    typedef std::pair < double, size_t > DI;
    std::vector < DI > tmpLevels( m_levels.size() );
    for ( size_t i = 0 ; i < m_levels.size() ; ++i ) {
        tmpLevels[i].first = m_levels[i];
        tmpLevels[i].second = i;
    }
    auto sort1 = [] ( const DI & a, const DI & b ) {
        return a.first < b.first;
    };
    std::sort( tmpLevels.begin(), tmpLevels.end(), sort1 );
    std::vector < double > sortedRawLevels( m_levels.size() );
    for ( size_t i = 0 ; i < m_levels.size() ; ++i ) {
        sortedRawLevels[i] = tmpLevels[i].first;
    }

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

    Result result;
    QString notify = "[contour] apply Conrec Algorithm with ";

    if (typeName == "Line combiner") {

        qDebug() << notify + typeName;

        Result result1 =
            conrecFaster(
                view,
                0,
                m_nCols - 1,
                0,
                m_nRows - 1,
                xcoords,
                ycoords,
                m_levels.size(),
                sortedRawLevels );

        QRectF rect( 0, 0, m_nCols, m_nRows);

        for( size_t i = 0 ; i < m_levels.size() ; ++ i ) {
            Carta::Lib::Algorithms::LineCombiner lc( rect, m_nRows+1, m_nCols+1, 1e-9);
            std::vector < QPolygonF > & v = result1[i];
            for( QPolygonF & poly : v) {
                for( int i = 0 ; i < poly.size() - 1 ; ++ i ) {
                    lc.add( poly[i], poly[i+1]);
                }
            }
            result.push_back( lc.getPolygons());
            qDebug() << "contour level=" << i << "compress" << v.size() << "-->" << result.back().size();
        }

    } else if (typeName == "Gaussian blur 3x3") {

        qDebug() << notify + typeName;

        double kernel[9] = {0.02479795, 0.10787775, 0.02479795,
                            0.10787775, 0.46929721, 0.10787775,
                            0.02479795, 0.10787775, 0.02479795};

        result =
            conrecFaster_Blur3(
                view,
                0,
                m_nCols - 1,
                0,
                m_nRows - 1,
                xcoords,
                ycoords,
                m_levels.size(),
                sortedRawLevels,
                kernel);

    } else if (typeName == "Box blur 3x3") {

        qDebug() << notify + typeName;

        double kernel[9] = {0.111111, 0.111111, 0.111111,
                            0.111111, 0.111112, 0.111111,
                            0.111111, 0.111111, 0.111111};

        result =
            conrecFaster_Blur3(
                view,
                0,
                m_nCols - 1,
                0,
                m_nRows - 1,
                xcoords,
                ycoords,
                m_levels.size(),
                sortedRawLevels,
                kernel);

    } else if (typeName == "Gaussian blur 5x5") {

        qDebug() << notify + typeName;

        double kernel[25] = {0.00178843, 0.01031068, 0.01809162, 0.01031068, 0.00178843,
                             0.01031068, 0.05944323, 0.10430201, 0.05944323, 0.01031068,
                             0.01809162, 0.10430201, 0.18301345, 0.10430201, 0.01809162,
                             0.01031068, 0.05944323, 0.10430201, 0.05944323, 0.01031068,
                             0.00178843, 0.01031068, 0.01809162, 0.01031068, 0.00178843};

        result =
            conrecFaster_Blur5(
                view,
                0,
                m_nCols - 1,
                0,
                m_nRows - 1,
                xcoords,
                ycoords,
                m_levels.size(),
                sortedRawLevels,
                kernel);

    } else if (typeName == "Box blur 5x5") {

        qDebug() << notify + typeName;

        double kernel[25] = {0.04, 0.04, 0.04, 0.04, 0.04,
                             0.04, 0.04, 0.04, 0.04, 0.04,
                             0.04, 0.04, 0.04, 0.04, 0.04,
                             0.04, 0.04, 0.04, 0.04, 0.04,
                             0.04, 0.04, 0.04, 0.04, 0.04};

        result =
            conrecFaster_Blur5(
                view,
                0,
                m_nCols - 1,
                0,
                m_nRows - 1,
                xcoords,
                ycoords,
                m_levels.size(),
                sortedRawLevels,
                kernel);

    } else {

        qDebug() << notify + "No line combiner";

        result =
            conrecFaster(
                view,
                0,
                m_nCols - 1,
                0,
                m_nRows - 1,
                xcoords,
                ycoords,
                m_levels.size(),
                sortedRawLevels );

    }

    // now we 'unsort' the contours based on the requested order
    Result unsortedResult( m_levels.size() );
    for ( size_t i = 0 ; i < m_levels.size() ; ++i ) {
        unsortedResult[tmpLevels[i].second] = result[i];
    }

    return unsortedResult;
} // compute
}
}
}

#ifdef DONT_COMPILE

/*
   This is the original C code, pasted verbatim.

   Derivation from the fortran version of CONREC by Paul Bourke
   d               ! matrix of data to contour
   ilb,iub,jlb,jub ! index bounds of data matrix
   x               ! data matrix column coordinates
   y               ! data matrix row coordinates
   nc              ! number of contour levels
   z               ! contour levels in increasing order
*/
void
Contour( double * * d, int ilb, int iub, int jlb, int jub,
         double * x, double * y, int nc, double * z,
         void ( * ConrecLine )( double, double, double, double, double ) )
{
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

    for ( j = ( jub - 1 ) ; j >= jlb ; j-- ) {
        for ( i = ilb ; i <= iub - 1 ; i++ ) {
            temp1 = MIN( d[i][j], d[i][j + 1] );
            temp2 = MIN( d[i + 1][j], d[i + 1][j + 1] );
            dmin = MIN( temp1, temp2 );
            temp1 = MAX( d[i][j], d[i][j + 1] );
            temp2 = MAX( d[i + 1][j], d[i + 1][j + 1] );
            dmax = MAX( temp1, temp2 );
            if ( dmax < z[0] || dmin > z[nc - 1] ) {
                continue;
            }
            for ( k = 0 ; k < nc ; k++ ) {
                if ( z[k] < dmin || z[k] > dmax ) {
                    continue;
                }
                for ( m = 4 ; m >= 0 ; m-- ) {
                    if ( m > 0 ) {
                        h[m] = d[i + im[m - 1]][j + jm[m - 1]] - z[k];
                        xh[m] = x[i + im[m - 1]];
                        yh[m] = y[j + jm[m - 1]];
                    }
                    else {
                        h[0] = 0.25 * ( h[1] + h[2] + h[3] + h[4] );
                        xh[0] = 0.50 * ( x[i] + x[i + 1] );
                        yh[0] = 0.50 * ( y[j] + y[j + 1] );
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

                    /* Finally draw the line */
                    ConrecLine( x1, y1, x2, y2, z[k] );
                } /* m */
            } /* k - contour */
        } /* i */
    } /* j */
} // Contour

#endif // ifdef DONT_COMPILE
