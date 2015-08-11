/**
 *
 **/

#include "catch.h"
#include "core/Algorithms/Graphs/TopoSort.h"
#include <QString>
#include <QTextStream>
#include <string>

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

// tester that will test input against output if there are no loops
struct TSortTester
{
    TSortTester( int n) : m_tsort(n) {
        m_n = n;
        m_executed = false;
    }

    void addArrow( int a, int b) {
        m_tsort.addArrow( a, b);
        m_input.push_back( std::make_pair( a, b));
    }

    void test() {
        if( m_executed) return;
        m_output = m_tsort.compute();
        m_executed = true;

        // if we have a loop, we are done
        m_hasLoop = m_output.empty();
        if( m_hasLoop) {
            m_passed = false;
            return;
        }

        // record the mapping from vertex to position in sorted list
        std::vector<int> pos( m_n);
        for( size_t i = 0 ; i < m_output.size() ; ++ i) {
            pos[ m_output[i]] = i;
        }

        // now check for each arrow that pos(a) < pos(b)
        m_passed = true;
        for( const auto & arrow : m_input) {
            if( pos[ arrow.first] >= pos[ arrow.second]) {
                m_passed = false;
                break;
            }
        }

    }

    bool passed() {
        test();
        return m_passed;
    }

    bool hasLoop() {
        test();
        return m_hasLoop;
    }

    std::vector< std::pair< int,int> > input() {
        return m_input;
    }
    std::vector< int > output() {
        test();
        return m_output;
    }

    Carta::Core::Algorithms::Graphs::TopoSort m_tsort;
    int m_n;
    bool m_passed;
    bool m_executed;
    std::vector<int> m_output;
    std::vector< std::pair< int, int> > m_input;
    bool m_hasLoop;
};

TEST_CASE( "Toposort testing", "[toposort]" ) {

    SECTION( "7 vertices, no edges") {
        TSortTester tester(7);
        INFO( "Input: " + toStr( tester.input()));
        INFO( "Output: " + toStr( tester.output()));
        REQUIRE( tester.passed());
    }

    SECTION( "7 vertices, single edge") {
        TSortTester tester(7);
        tester.addArrow( 1, 2);
        INFO( "Input: " + toStr( tester.input()));
        INFO( "Output: " + toStr( tester.output()));
        REQUIRE( tester.passed());
    }


    SECTION( "Wiki example") {
        TSortTester tester(12);
        tester.addArrow( 7, 11);
        tester.addArrow( 5, 11);
        tester.addArrow( 3, 8);
        tester.addArrow( 7, 8);
        tester.addArrow( 3, 10);
        tester.addArrow( 11, 2);
        tester.addArrow( 11, 9);
        tester.addArrow( 11, 10);
        tester.addArrow( 8, 9);

        INFO( "Input: " + toStr( tester.input()));
        INFO( "Output: " + toStr( tester.output()));
        REQUIRE( tester.passed());
    }

    SECTION( "Wiki example with loop") {
        TSortTester tester(12);
        tester.addArrow( 7, 11);
        tester.addArrow( 5, 11);
        tester.addArrow( 3, 8);
        tester.addArrow( 7, 8);
        tester.addArrow( 3, 10);
        tester.addArrow( 11, 2);
        tester.addArrow( 11, 9);
        tester.addArrow( 11, 10);
        tester.addArrow( 8, 9);
        tester.addArrow( 2, 7);

        INFO( "Input: " + toStr( tester.input()));
        INFO( "Output: " + toStr( tester.output()));
        REQUIRE( ! tester.passed());
        REQUIRE( tester.hasLoop());
    }

}
