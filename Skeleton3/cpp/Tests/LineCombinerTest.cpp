/**
 *
 **/



/**
 *
 **/

#include "catch.h"
#include "../CartaLib/Algorithms/LineCombiner.h"
#include <QString>
#include <QTextStream>
#include <QLineF>
#include <string>
#include <algorithm>

template < typename T >
std::string toStr( const T & val)
{
    std::ostringstream ts;
    ts << val;
    return ts.str();
}

template <typename T1, typename T2>
std::string toStr ( const std::pair< T1, T2> & pair)
{
    std::string result = "(" + toStr( pair.first) + "," + toStr( pair.second) + ")";
    return result;
}

template <typename T>
std::string toStr ( const std::vector< T > & arr)
{
    std::string result = "[";
    for( const auto & a : arr) {
        result += toStr( a) + " ";
    }
    result += "]";
    return result;
}

using namespace Carta::Lib::Algorithms;

TEST_CASE( "Line combiner testing", "[polyline]" ) {

    QRectF rect1( 0, 0, 10, 10);

    SECTION( "empty input") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        REQUIRE( lc.getPolygons().size() == 0);
    }

    SECTION( "single line segment") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        QPointF p1( 0, 0);
        QPointF p2( 1, 1);
        lc.add( p1, p2);
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 2);
    }

    SECTION( "two disconnected line segments") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        lc.add( { 0, 0}, { 1, 1});
        lc.add( { 2.2, 3.3 }, { 5.0, 1.0 });
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 2);
        REQUIRE( res[0].size() == 2);
        REQUIRE( res[1].size() == 2);
    }

    SECTION( "two connected line segments") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        lc.add( { 0, 0}, { 1, 1});
        lc.add( { 1, 1 }, { 5.0, 1.0 });
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 3);
    }

    SECTION( "three connected line segments, l-r") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        lc.add( { 0, 0}, { 1, 1});
        lc.add( { 1, 1 }, { 5.0, 1.0 });
        lc.add( { 5, 1 }, { 2, 2 });
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 4);
    }

    SECTION( "three connected line segments, r-l") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        QPointF A( 0, 0);
        QPointF B( 1, 1);
        QPointF C( 5, 1);
        QPointF D( 2, 2);
        lc.add( B, A);
        lc.add( C, B);
        lc.add( D, C);
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 4);
    }

    SECTION( "triangle") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        QPointF A( 1, 1);
        QPointF B( 2, 2);
        QPointF C( 3, 1);
        lc.add( B, A);
        lc.add( B, C);
        lc.add( A, C);
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 4);
        REQUIRE( res[0].isClosed());
    }

    SECTION( "u shape") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        QPointF A( 1, 2);
        QPointF B( 1, 1);
        QPointF C( 2, 1);
        QPointF D( 2, 2);
        lc.add( B, A);
        lc.add( D, C);
        lc.add( B, C);
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 4);
        REQUIRE( ! res[0].isClosed());
    }

    SECTION( "u shape II") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        QPointF A( 1, 2);
        QPointF B( 1, 1);
        QPointF C( 2, 1);
        QPointF D( 2, 2);
        lc.add( B, A);
        lc.add( C, D);
        lc.add( B, C);
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 4);
        REQUIRE( ! res[0].isClosed());
    }

    SECTION( "u shape III") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        QPointF A( 1, 2);
        QPointF B( 1, 1);
        QPointF C( 2, 1);
        QPointF D( 2, 2);
        lc.add( A, B);
        lc.add( D, C);
        lc.add( B, C);
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 4);
        REQUIRE( ! res[0].isClosed());
    }

    SECTION( "u shape IV") {
        LineCombiner lc( rect1, 10, 10, 0.001);
        QPointF A( 1, 2);
        QPointF B( 1, 1);
        QPointF C( 2, 1);
        QPointF D( 2, 2);
        lc.add( A, B);
        lc.add( C, D);
        lc.add( B, C);
        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == 4);
        REQUIRE( ! res[0].isClosed());
    }

    SECTION( "circle") {
        QPolygonF poly;
        double rad = 10;
        int n = 100000;
        for( double alpha = 0 ; alpha < M_PI * 2 ; alpha += M_PI * 2 / n) {
            poly.append({ sin(alpha) * rad, cos(alpha) * rad });
        }
        std::vector<QLineF> lines;
        for( int i = 0 ; i < poly.size() - 1; ++ i) {
            if( drand48() < 0.5 )
                lines.push_back( QLineF( poly[i], poly[i+1]));
            else
                lines.push_back( QLineF( poly[i+1], poly[i]));
        }
        lines.push_back( QLineF( poly[0], poly.last()));
        std::random_shuffle ( lines.begin(), lines.end());
        LineCombiner lc( poly.boundingRect(), 1000, 1000, 1e-9);
        for( auto & line : lines) {
            lc.add( line.p1(), line.p2());
        }

        std::vector<QPolygonF> res = lc.getPolygons();
        REQUIRE( res.size() == 1);
        REQUIRE( res[0].size() == poly.size() + 1);
        REQUIRE( res[0].isClosed());
    }


}
