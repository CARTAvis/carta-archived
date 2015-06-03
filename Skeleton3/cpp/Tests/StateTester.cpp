/*
 * Tester.cc
 *
 *  Created on: Sep 29, 2014
 *      Author: jjacobs
 */

#include "catch.h"
#include "../common/State/StateInterface.h"
#include <stdexcept>
#include <QDebug>

using namespace std;

class StateInterfaceTestImpl : public Carta::State::StateInterface {

public:

    StateInterfaceTestImpl () : StateInterface ("/a/b/c", "Tester") {}

    void setStateString (const QString & s){
        stateString_p = s;
    }

private:

    virtual QString fetchStateImpl (){
        qDebug()<<"State fetched: "<< stateString_p;
        return QString (stateString_p);
    }

    virtual void flushStateImpl (const QString & stateString){
        stateString_p = stateString;
        qDebug() << "State flushed: " << stateString_p;
    }

    QString stateString_p;
};

TEST_CASE( "Carta state test", "[testname]" ) {

    StateInterfaceTestImpl tester;
    QString del( Carta::State::StateInterface::DELIMITER);
    QString subZ( "sub" + del + "z");
    SECTION( "Test setting state via JSON") {
        tester.setStateString ("{\"a\":\"abc\",\"i\":123,\"pi\":3.14159, "
                             "\"sub\":{\"s\":7,\"z\": [10,20,30]}}");
        tester.fetchState();

        REQUIRE( tester.getValue<QString>("a") == "abc" );
        REQUIRE( tester.getValue<int>("i") == 123 );
        REQUIRE( tester.getValue<double>("pi") == 3.14159 );
        REQUIRE( tester.getValue<int>( "sub/s") == 7 );

        REQUIRE( tester.getArraySize( subZ ) == 3);
        REQUIRE( tester.getValue<int>( subZ + del+ "0") == 10 );
        REQUIRE( tester.getValue<int>( subZ + del + "1") == 20 );
        REQUIRE( tester.getValue<int>( subZ + del + "2") == 30 );
        tester.flushState ();

        SECTION( "Test setting state to a different value"){
            tester.setValue ("i", 321);
            REQUIRE( tester.getValue<int>("i") == 321 );
            tester.flushState ();
        }

        SECTION( "Change state test.") {
            tester.setStateString ("{\"a\":\"abc\",\"i\":456,\"pi\":3.14159,"
            "                \"sub\":{\"s\":49,\"z\": [10,40,30]}}");
            tester.fetchState();
            REQUIRE( tester.hasChanged( "a") == false );
            REQUIRE( tester.hasChanged( "i") == true );
            REQUIRE( tester.hasChanged( "pi") == false );
            REQUIRE( tester.hasChanged( subZ) == true );
        }

        SECTION("Changing size of array sub/z from 3 to 4"){
            tester.resizeArray (subZ, 4);
            REQUIRE( tester.getArraySize( subZ) == 4 );
            REQUIRE( tester.toString() == "{\"a\":\"abc\",\"i\":123,\"pi\":3.14159,\"sub\":{\"s\":7,\"z\":[{},{},{},{}]}}");
            tester.flushState();
        }

        SECTION("Changing size of array sub/z from 3 to 2"){
            tester.resizeArray ( subZ, 2);
            REQUIRE( tester.getArraySize( subZ ) == 2 );
            REQUIRE( tester.toString() == "{\"a\":\"abc\",\"i\":123,\"pi\":3.14159,\"sub\":{\"s\":7,\"z\":[{},{}]}}");
            tester.flushState();
        }

        SECTION("Changing size of array sub/z from 3 to 2 preserving all"){
            tester.resizeArray (subZ, 2, StateInterfaceTestImpl::PreserveAll);
            REQUIRE( tester.getArraySize( subZ ) == 2 );
            REQUIRE( tester.getValue<int>(subZ + del + "0") == 10 );
            REQUIRE( tester.getValue<int>(subZ + del + "1") == 20 );
            tester.flushState();
        }

        SECTION("Changing size of array sub/z from 3 to 4 preserving all"){
            tester.resizeArray (subZ, 4, StateInterfaceTestImpl::PreserveAll);
            REQUIRE( tester.getArraySize( subZ ) == 4 );
            REQUIRE( tester.getValue<int>( subZ + del + "0") == 10 );
            REQUIRE( tester.getValue<int>( subZ + del + "1") == 20 );
            REQUIRE( tester.getValue<int>( subZ + del + "2") == 30 );
            tester.flushState();
        }

        SECTION("Adding array sub/q with 2 elements"){
            tester.insertArray ("sub" + del + "q", 2);
            REQUIRE( tester.getArraySize( "sub" + del + "q") == 2 );
            tester.flushState();
        }

        SECTION( "Changing field i to a string"){
            tester.setValue<QString> ("i", "xyz");
            REQUIRE( tester.getValue<QString>("i") == "xyz");
            tester.flushState();
        }

        SECTION("Adding field obj"){
            tester.insertObject( "obj");
            tester.insertValue<QString>( "obj" + del + "key1", "Hi");
            tester.insertValue<int>("obj" + del + "key2", 991);
            REQUIRE( tester.getValue<QString>("obj" + del + "key1") == "Hi");
            REQUIRE( tester.getValue<int>("obj" + del + "key2") == 991 );
            tester.flushState();
        }

        SECTION( "Adding field values"){
            tester.insertValue<int> ("j", 667);
            REQUIRE( tester.getValue<int>( "j") == 667 );
            tester.insertValue<QString> ("sub" + del + "w", "www");
            REQUIRE( tester.getValue<QString>( "sub" + del + "w") == "www");
            tester.flushState();
        }
    }

    SECTION( "Piecemeal initialization test"){
        tester.setStateString ("{}");
        tester.fetchState();
        tester.insertValue<QString> ("a","abc");
        tester.insertValue<int> ("i", 123);
        tester.insertValue<double> ("pi", 3.14159 );
        tester.insertObject ("sub");
        tester.insertValue<int> ("sub" + del + "s", 7);
        tester.insertArray (subZ, 3);
        tester.setValue<int> ( subZ + del +"0", 10);
        tester.setValue<int> ( subZ + del + "1", 20);
        tester.setValue<int> ( subZ + del + "2", 30);
        tester.flushState();
        REQUIRE( tester.getValue<QString>( "a" ) == "abc");
        REQUIRE( tester.getValue<int>( "i") == 123 );
        REQUIRE( tester.getValue<double>( "pi" ) == 3.14159 );
        REQUIRE( tester.getValue<int>("sub" + del + "s") == 7 );
        REQUIRE( tester.getValue<int>( subZ + del + "2") == 30 );

        SECTION( "Verify an exception is generated when requesting an invalid state"){
            try {
                tester.getValue<int>( subZ + del + "7");
                REQUIRE( false );
            }
            catch( invalid_argument & e ){
                qDebug() << "Expected exception: "<< e.what();
            }

            try {
                tester.getValue<int>( "sub" + del +"y");
                REQUIRE( false );
            }
            catch( invalid_argument & e ){
                qDebug() << "Expected exception : "<< e.what();
            }

            try {
                tester.getValue<int> ("asdfg");
                REQUIRE(false);
            }
            catch (invalid_argument & e){
                qDebug() << "Expected exception: " << e.what();
            }


            try {
                tester.getValue<int> ("i" + del + "y");
                REQUIRE( false );
            }
            catch (invalid_argument & e){
                qDebug() << "Expected exception: " << e.what();
            }
        }

        SECTION("Verify an exception is generated when trying to insert a duplicate object at a given key"){
            try {
                tester.insertObject ("i");
                REQUIRE(false);
            }
            catch (invalid_argument & e){
                qDebug() << "Expected exception : "<< e.what();
            }

            try {
                tester.insertObject ("i" + del + "w");
                REQUIRE(false);
            }
            catch (invalid_argument & e){
                qDebug() << "Expected exception : "<< e.what();
            }

            try {
                tester.insertArray ("i", 7);
                REQUIRE(false);
            }
            catch (invalid_argument & e){
                qDebug() << "Expected exception : "<< e.what();
            }

            try {
                tester.insertArray ("i" + del + "w", 7);
                REQUIRE(false);
            }
            catch (invalid_argument & e){
                qDebug() << "Expected exception: " << e.what();
            }
        }

    }

    SECTION( "Verify an incorrect initialization generates an exception"){
        try {
            tester.setStateString ("}{");
            tester.fetchState();
            REQUIRE(false);
        }
        catch (domain_error & e){
            qDebug() << "Expected exception: " << e.what();
        }
    }

    SECTION( "Test adding an array of objects"){
        tester.setStateString ("{}");
        tester.insertArray ("emp", 2);
        REQUIRE( tester.getArraySize("emp") == 2 );
        tester.insertValue<QString> ("emp" + del + "0" + del + "name", "me");
        REQUIRE( tester.getValue<QString>("emp" + del + "0" + del + "name") == "me");
        tester.insertValue<int> ("emp" + del + "0" + del + "phone", 1234);
        REQUIRE( tester.getValue<int>( "emp" + del + "0" + del + "phone") == 1234 );
        tester.insertValue<QString> ("emp" + del + "1" + del + "name", "you");
        REQUIRE( tester.getValue<QString>("emp" + del + "1" + del + "name") == "you");
        tester.insertValue<int> ("emp" + del + "1" + del + "phone", 5678);
        REQUIRE( tester.getValue<int>( "emp" + del + "1" + del + "phone") == 5678 );
        tester.flushState();
    }

    SECTION( "Test adding an array of arrays"){
        tester.setStateString ("{}");
        tester.insertArray ("outer", 2);
        REQUIRE( tester.getArraySize("outer") == 2);
        tester.setArray ("outer" + del + "0", 3);
        REQUIRE( tester.getArraySize( "outer" + del + "0") == 3 );
        tester.setArray ("outer"+ del + "1", 4);
        REQUIRE( tester.getArraySize( "outer" + del + "1") == 4 );
        tester.setValue<int> ("outer" + del + "1" + del + "3", 345);
        REQUIRE( tester.getValue<int>( "outer" + del + "1" + del + "3") == 345 );
        tester.flushState();
    }

    SECTION( "Test null"){
        tester.setStateString ("{}");
        tester.insertNull ("n");
        const QString nullStr( "{\"type\":\"Tester\",\"n\":null}");
        REQUIRE( tester.toString() == nullStr);
        tester.setValue<int> ("n", 7);
        REQUIRE( tester.getValue<int>("n") == 7 );
        tester.setNull ("n");
        REQUIRE( tester.toString() == nullStr );
        tester.setObject ("n");
        tester.setNull ("n");
        REQUIRE( tester.toString() == nullStr );
        tester.setArray ("n", 2);
        REQUIRE( tester.getArraySize( "n" ) == 2 );
        tester.flushState();
    }

    SECTION( "Array resize test"){
        tester.setStateString ("{}");
        tester.insertArray ("array", 3);
        REQUIRE( tester.getArraySize( "array") == 3 );
        tester.setValue<int> ("array" + del + "0", 1);
        REQUIRE( tester.getValue<int>("array" + del + "0") == 1 );
        tester.resizeArray("array", 0);
        REQUIRE( tester.getArraySize( "array") == 0 );
        tester.resizeArray ("array", 4);
        REQUIRE( tester.getArraySize( "array") == 4 );
        tester.setValue<QString> ("array" + del + "0", "aString");
        REQUIRE( tester.getValue<QString>("array" + del + "0") == "aString");
        tester.flushState();
    }

    SECTION( "Test inserting an object with a string representing its state"){
        tester.setStateString ("{\"a\":\"abc\",\"i\":456,\"pi\":3.14159,\"sub\":{\"s\":49,\"z\": [10,20,30]}}");
        tester.fetchState();
        tester.flushState();
        tester.insertObject ("sub/xx", "{\"q\":10,\"r\":11, \"s\":[12,13,14]}");
        tester.flushState();
        REQUIRE( tester.getValue<int>( "sub/xx/q") == 10 );
        REQUIRE( tester.getArraySize( "sub/xx/s") == 3);

        QList<QString> members = tester.getMemberNames("");
        int memberCount = members.size();
        REQUIRE( memberCount == 4 );
        REQUIRE( members[0] == "a");
        REQUIRE( members[1] == "i");
        REQUIRE( members[2] == "pi");
        REQUIRE( members[3] == "sub");
        tester.flushState();
    }

    SECTION( "Test clearing an object an then resetting it"){
        tester.setStateString("{}");
        tester.fetchState();
        tester.insertObject( "anim");
        tester.flushState();
        tester.insertObject( "anim/long", "{\"type\":1}");
        REQUIRE( tester.getValue<int>("anim/long/type") == 1 );

        //Clean anim
        tester.setObject( "anim");
        tester.flushState();
        try {
            tester.getValue<int>( "anim/long/type");
            REQUIRE( false );
        }
        catch( std::invalid_argument& err ){
            qDebug() << "Expected exception: "<<err.what();
        }
        tester.insertObject( "anim/long", "{\"type\":\"AnimatorType\",\"frameStep\":1,\"frameRate\":20,\"endBehavior\":\"Wrap\",\"Selection\":{\"type\":\"Selection\",\"frameStart\":0,\"frameEnd\":1,\"frame\":0}}" );
        tester.flushState();
        REQUIRE( tester.getValue<int>( "anim/long/frameStep") == 1);
    }

    SECTION( "Testing insertObject and setObject with Json value" ){
       tester.setStateString ("{\"a\":\"abc\",\"i\":456,\"pi\":3.14159,\"sub\":{\"s\":49,\"z\": [10,20,30]}}");
       tester.fetchState();
       tester.insertObject ("sub/xx", "{\"q\":10,\"r\":11, \"s\":[12,13,14]}");
       tester.flushState();
       tester.setObject ("sub/z/0", "{\"h\":345}");
       tester.flushState();
       REQUIRE( tester.toString() =="{\"a\":\"abc\",\"i\":456,\"pi\":3.14159,\"sub\":{\"s\":49,\"z\":[{\"h\":345},20,30],\"xx\":{\"q\":10,\"r\":11,\"s\":[12,13,14]}}}");
       REQUIRE( tester.toString("sub")=="{\"s\":49,\"z\":[{\"h\":345},20,30],\"xx\":{\"q\":10,\"r\":11,\"s\":[12,13,14]}}" );
       REQUIRE( tester.getValue<int>("sub/z/0/h") == 345 );

       SECTION ( "Test inserting an object with a nested object "){
           tester.insertObject( "sub/xx/nested", "{\"anObject\":{\"key\":\"aValue\"}}");
           tester.flushState();
           REQUIRE( tester.toString( "sub/xx/nested/anObject") == "{\"key\":\"aValue\"}");
           REQUIRE( tester.getValue<QString>("sub/xx/nested/anObject/key") == "aValue");
       }
    }

}
