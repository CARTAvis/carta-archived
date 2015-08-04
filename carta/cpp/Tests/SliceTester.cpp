/**
 *
 **/

#include "catch.h"
#include "CartaLib/Slice.h"
#include "CartaLib/IImage.h"
#include <QString>
#include <QTextStream>
#include <string>

static
std::string toStr( const Slice1D::ApplyResult & ar)
{
    return QString("[s:%1 c:%2 s:%3]")
            .arg( ar.start).arg( ar.count).arg( ar.step).toStdString();
}

// tester for 1D slices
struct Tester {

    typedef Slice1D::Index Index;

    Tester() {}
    Tester( QString s) {
        m_input = s.simplified();
    }
    std::string name() { return "Testing " + m_input.toStdString();}
    std::string result() {
        if( ! m_internalError.isEmpty()) {
            return "Internal error: " + m_internalError.toStdString();
        } else {
            std::string extra;
            if( ! m_extraInfo.isEmpty()) {
                extra = " //" + m_extraInfo.toStdString();
            }
            return "Result: " + toStr( m_result) + extra;
        }
    }
    bool passed() { return m_passed; }
    void run() {

        QTextStream extra( & m_extraInfo);
        Slice1D slice;

        Index expectedCount, expectedStart, expectedStep;
        Index n;

        QStringList lv1 = m_input.split( ' ', QString::SkipEmptyParts);
        if( lv1.size() != 3) {
            m_internalError = "need 3 parts to input";
            return;
        }
        bool ok;
        // parse out n
        n = lv1[0].toInt( & ok);
        if( ! ok) {
            m_internalError = "bad n";
            return;
        }
        extra << "n=" << n << " ";

        // parse slice (it could be
        QStringList lv2 = lv1[1].split( ':');
        // handle single index
        if( lv2.size() == 1) {
            Index start = lv2[0].toInt( & ok);
            if( ! ok) { m_internalError = "bad single index"; return; }
            extra << "[" << start << "]";
            slice.index( start);
        }
        else if( lv2.size() > 3) {
            m_internalError = "slice needs <=3 integers";
            return;
        }
        else {
            // padd the slice with empty strings
            while( lv2.size() < 3) {
                lv2.append( "");
            }
            if( ! lv2[0].isEmpty()) {
                Index start = lv2[0].toInt( & ok);
                if( ! ok) { m_internalError = "bad start"; return; }
                extra << start;
                slice.start( start);
            }
            extra << ":";

            if( ! lv2[1].isEmpty()) {
                Index end = lv2[1].toInt( & ok);
                if( ! ok) { m_internalError = "bad end"; return; }
                slice.end( end);
                extra << end;
            }
            extra << ":";

            if( ! lv2[2].isEmpty()) {
                Index step = lv2[2].toInt( & ok);
                if( ! ok) { m_internalError = "bad step"; return; }
                slice.step( step);
                extra << step;
            }
        }


        // parse expected values
        QStringList lv3 = lv1[2].split( ',');
        if( lv3.size() != 3) {
            m_internalError = "expected values need 3 integers";
            return;
        }
        expectedStart = lv3[0].toInt( & ok);
        if( ! ok) { m_internalError = "bad expected start"; return; }
        expectedCount = lv3[1].toInt( & ok);
        if( ! ok) { m_internalError = "bad expected end"; return; }
        expectedStep = lv3[2].toInt( & ok);
        if( ! ok) { m_internalError = "bad expected step"; return; }

//        m_result = slice.apply( n);
        m_result = slice.cpythonApply( n);

        // compare vs expected values
        m_passed = m_result.count == expectedCount
                   && m_result.start == expectedStart
                   && m_result.step == expectedStep;
    }

    QString m_input;
    QString m_internalError;
    Slice1D::ApplyResult m_result;
    bool m_passed = false;
    QString m_extraInfo;
};



TEST_CASE( "Slice1d::apply", "[slice1d]" ) {

#define MYTEST(str) \
    { \
        Tester t = Tester( str); \
        SECTION( t.name()) { \
            t.run(); \
            INFO( t.result()); \
            REQUIRE( t.passed()); \
        } \
    }

    MYTEST( "10 ::1  0,10,1" );
    MYTEST( "10 ::-1 9,10,-1" );
    MYTEST( "10 :: 0,10,1" );
    MYTEST( "10 1:2:1 1,1,1" );
    MYTEST( "10 3:5:1 3,2,1");
    MYTEST( "0 10:20:2 0,0,2");
    MYTEST( "0 :: 0,0,1");
    MYTEST( "10 :: 0,10,1");
    MYTEST( "10 -100:200: 0,10,1");
    MYTEST( "10 100:200: 10,0,1");
    MYTEST( "10 -100:-200: 0,0,1");
    MYTEST( "10 200:100: 10,0,1");
    MYTEST( "10 9:0: 9,0,1");


    MYTEST( "10 1:7:2 1,3,2");
    MYTEST( "10 -2:10: 8,2,1");
    MYTEST( "10 -1:: 9,1,1");
    MYTEST( "10 :-3: 0,7,1");
    MYTEST( "10 5:: 5,5,1");
    MYTEST( "10 :5: 0,5,1");
    MYTEST( "10 ::5 0,2,5");
    MYTEST( "10 :: 0,10,1");
    MYTEST( "10 ::1 0,10,1");
    MYTEST( "10 ::2 0,5,2");
    MYTEST( "10 ::3 0,4,3");
    MYTEST( "10 ::4 0,3,4");
    MYTEST( "10 ::6 0,2,6");
    MYTEST( "10 ::7 0,2,7");
    MYTEST( "10 ::9 0,2,9");
    MYTEST( "10 ::10 0,1,10");
    MYTEST( "10 ::11 0,1,11");

    MYTEST( "10 ::0 -1,0,0"); // error

    MYTEST( "10 0 0,-1,1"); // single index good
    MYTEST( "10 9 9,-1,1"); // single index good
    MYTEST( "10 10 -1,-1,1"); // single index out of bounds error
    MYTEST( "10 -11 -1,-1,1"); // single index out of bounds error
    MYTEST( "10 -1 9,-1,1"); // single index from end good
    MYTEST( "10 -10 0,-1,1"); // single index from end good
    MYTEST( "10 -11 -1,-1,1"); // single index from out of bounds

    MYTEST( "10 5:2:-1 5,3,-1");
    MYTEST( "10 500:2:-1 9,7,-1");
    MYTEST( "10 -1:0:-1 9,9,-1");

    SECTION( "Compare apply to cpython") {
        int nTrials = 100000;
        int maxSize = 10;
        int nPassed = 0;
        for( int i = 0 ; i < nTrials ; i ++) {
            int n = qrand() % (maxSize + 1);
            int start = qrand() % (maxSize * 3) - maxSize;
            int end =  qrand() % (maxSize * 3) - maxSize;
            int step = qrand() % (maxSize + 5);
            Slice1D s;
            if( qrand() % 4 ) { s.start( start); }
            if( qrand() % 4 ) { s.end( end);  }
            if( qrand() % 4 ) { s.step( step);  }
            auto res1 = s.cpythonApply( n);
            auto res2 = s.apply( n);
            bool match = true;
            match = match && res1.start == res2.start;
            match = match && res1.count == res2.count;
            match = match && res1.step == res2.step;
            QString msg; QTextStream msgOut( & msg);
            msgOut << ( match ? "pass" : "fail") << ": ";
            msgOut << n << s.toStr() << " --> ";
            msgOut << res1.start << "," << res1.count << "," << res1.step;
            if( ! match) {
                msgOut << " vs. " << res2.start << "," << res2.count << "," << res2.step;
            }
            INFO( msg.toStdString());

            CHECK( match);
            if( match) nPassed ++;
        }
        INFO((QString( "Passed:%1/:%2").arg(nPassed).arg(nTrials).toStdString()));
        REQUIRE( nPassed==nTrials);
    }

    SECTION( "test toStr") {
        CHECK(( Slice1D().toStr() == "[:]"));
        CHECK(( Slice1D(7).toStr() == "[7]"));
        CHECK(( Slice1D().step(-2).toStr() == "[::-2]"));
        CHECK(( Slice1D().start(10).end(-3).toStr() == "[10:-3]"));
        CHECK(( Slice1D().start(10).step(-3).toStr() == "[10::-3]"));
    }
}

// tester for 1D slices
struct TesterND {

    typedef Slice1D::Index Index;

    TesterND() {}
    TesterND( QString s) {
        m_input = s.simplified();
    }
    std::string name() { return "Testing " + m_input.toStdString();}
    std::string result() {
        if( ! m_internalError.isEmpty()) {
            return "Internal error: " + m_internalError.toStdString();
        } else {
            std::string extra;
            if( ! m_extraInfo.isEmpty()) {
                extra = " //" + m_extraInfo.toStdString();
            }
            return "Result: " + toStr( m_result) + extra;
        }
    }
    bool passed() { return m_passed; }
    void run() {

        QTextStream extra( & m_extraInfo);
        Slice1D slice;

        Index expectedCount, expectedStart, expectedStep;
        Index n;

        QStringList lv1 = m_input.split( ' ', QString::SkipEmptyParts);
        if( lv1.size() != 3) {
            m_internalError = "need 3 parts to input";
            return;
        }
        bool ok;
        // parse out n
        n = lv1[0].toInt( & ok);
        if( ! ok) {
            m_internalError = "bad n";
            return;
        }
        extra << "n=" << n << " ";

        // parse slice (it could be
        QStringList lv2 = lv1[1].split( ':');
        // handle single index
        if( lv2.size() == 1) {
            Index start = lv2[0].toInt( & ok);
            if( ! ok) { m_internalError = "bad single index"; return; }
            extra << "[" << start << "]";
            slice.index( start);
        }
        else if( lv2.size() > 3) {
            m_internalError = "slice needs <=3 integers";
            return;
        }
        else {
            // padd the slice with empty strings
            while( lv2.size() < 3) {
                lv2.append( "");
            }
            if( ! lv2[0].isEmpty()) {
                Index start = lv2[0].toInt( & ok);
                if( ! ok) { m_internalError = "bad start"; return; }
                extra << start;
                slice.start( start);
            }
            extra << ":";

            if( ! lv2[1].isEmpty()) {
                Index end = lv2[1].toInt( & ok);
                if( ! ok) { m_internalError = "bad end"; return; }
                slice.end( end);
                extra << end;
            }
            extra << ":";

            if( ! lv2[2].isEmpty()) {
                Index step = lv2[2].toInt( & ok);
                if( ! ok) { m_internalError = "bad step"; return; }
                slice.step( step);
                extra << step;
            }
        }


        // parse expected values
        QStringList lv3 = lv1[2].split( ',');
        if( lv3.size() != 3) {
            m_internalError = "expected values need 3 integers";
            return;
        }
        expectedStart = lv3[0].toInt( & ok);
        if( ! ok) { m_internalError = "bad expected start"; return; }
        expectedCount = lv3[1].toInt( & ok);
        if( ! ok) { m_internalError = "bad expected end"; return; }
        expectedStep = lv3[2].toInt( & ok);
        if( ! ok) { m_internalError = "bad expected step"; return; }

//        m_result = slice.apply( n);
        m_result = slice.cpythonApply( n);

        // compare vs expected values
        m_passed = m_result.count == expectedCount
                   && m_result.start == expectedStart
                   && m_result.step == expectedStep;
    }

    QString m_input;
    QString m_internalError;
    Slice1D::ApplyResult m_result;
    bool m_passed = false;
    QString m_extraInfo;
};

TEST_CASE( "SliceND", "[slicend]") {

    SECTION( "toStr test") {
        REQUIRE(( SliceND().toStr() == "[:]"));
        REQUIRE(( SliceND().next().toStr() == "[:,:]"));
        REQUIRE(( SliceND({ Slice1D(7)}).toStr() == "[7]"));
        REQUIRE(( SliceND().next().toStr() == "[:,:]"));
        REQUIRE(( SliceND().start(3).step(-1).next().next().index(3).toStr() == "[3::-1,:,3]"));
    }

#define MYTESTND(str) \
    { \
        TesterND t = TesterND( str); \
        SECTION( t.name()) { \
            t.run(); \
            INFO( t.result()); \
            REQUIRE( t.passed()); \
        } \
    }

//    MYTESTND( "[10,20,30] [:] [10,20,30]");

    SECTION( "a=[10x20x30] a[2:5:2,:,70]") {
        SliceND nd = SliceND().start(2).end(5).step(2)
                     .next()
                     .next().index(70);
        auto res = nd.apply({ 10, 20, 30});
        INFO( QString( "Res=%1").arg(res.toStr()).toStdString() );
        REQUIRE( res.isError());

    }

    SECTION( "a=[10x20x30] a[2:5:2,:,7]") {
        SliceND nd = SliceND().start(2).end(5).step(2)
                     .next()
                     .next().index(7);
        auto res = nd.apply({ 10, 20, 30});
        INFO( QString( "Res=%1").arg(res.toStr()).toStdString() );
        REQUIRE(( ! res.isError() && ! res.isSingle() ));
    }

    SECTION( "a=[10x20x30] a[1,2,3]") {
        SliceND nd = { 1, 2, 3};
        auto res = nd.apply({ 10, 20, 30});
        INFO( QString( "Res=%1").arg(res.toStr()).toStdString() );
        REQUIRE(( ! res.isError() && res.isSingle() ));
    }

    SECTION( "a=[10x20x30] a[:]") {
        SliceND nd = SliceND();
        auto res = nd.apply({ 10, 20, 30});
        INFO( QString( "Res=%1").arg(res.toStr()).toStdString() );
        REQUIRE(( ! res.isError() && ! res.isSingle() ));
        REQUIRE(( res.dims().size() == 3 ));
    }

    SECTION( "a=[10x20] a[2:5:2,:,7]") {
        SliceND nd = SliceND().start(2).end(5).step(2)
                     .next()
                     .next().index(7);
        auto res = nd.apply({ 10, 20});
        INFO( QString( "Res=%1").arg(res.toStr()).toStdString() );
        REQUIRE(( res.isError() ));
    }


}
