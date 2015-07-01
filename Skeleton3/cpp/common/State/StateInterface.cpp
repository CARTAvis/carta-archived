/*
 * StateInterface.cc
 *
 *  Created on: Sep 29, 2014
 *      Author: jjacobs
 */

#include "StateInterface.h"

#include "IConnector.h"
#include "Globals.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sstream>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <stdexcept>

using namespace rapidjson;
using namespace std;

namespace Carta {

namespace State {

class StateInterfaceImpl {

    friend class StateInterface;

private:

    StateInterfaceImpl (const QString & path )
    : path_p (path)
    {
        state_p.SetObject();
    }

    StateInterfaceImpl (const StateInterfaceImpl & other)
    {
        oldState_p.CopyFrom (other.oldState_p, oldState_p.GetAllocator());
        path_p = other.path_p;
        state_p.CopyFrom (other.state_p, state_p.GetAllocator());

    }

    vector <QString> getKeys (const QString &) const;
    template <typename Iterator>
    QString makeKeys (Iterator begin, const Iterator & end) const;
    const Value & getValueAux (const QString & keyString, const Document & state) const;
    Value & getValueAux (const QString & keyString, Document & state) const;
    Value* _getValueAux( const QString& keyString, const Document& state ) const;
    void insertObjectAux (const QString & keyString, Value & valueToInsert);

    Document oldState_p;
    QString path_p;
    Document state_p;

};

class AsUtf8 {

public:

    AsUtf8 (const QString & value) : qstring_p (value.toStdString().c_str())
    {
        qByteArray_p = qstring_p.toUtf8();
    }

    const char * data (){
        return qByteArray_p.data();
    }

    int size (){
        return qByteArray_p.size();
    }

private:

    QString qstring_p;
    QByteArray qByteArray_p;

};

const QString StateInterface::DELIMITER("/");
const QString StateInterface::STATE_DATA( "data");
const QString StateInterface::FLUSH_STATE( "flush");
const QString StateInterface::OBJECT_TYPE( "type");
const QString StateInterface::INDEX = "index";

StateInterface::StateInterface (const QString & path, const QString& type, const QString& initialState )
: impl_p (new StateInterfaceImpl (path) )
{
    insertValue<QString>( OBJECT_TYPE, type );
    insertValue<int>(INDEX, 0 );
    insertValue<bool>(FLUSH_STATE, false );

    if ( initialState.trimmed().size() > 0 ){
        flushStateImpl( initialState );
    }
}

 void StateInterface::refreshState(){
    setValue<bool>(FLUSH_STATE, true );
    flushState();
    setValue<bool>(FLUSH_STATE, false );
}

StateInterface::StateInterface (const StateInterface & other)
{
    impl_p = new StateInterfaceImpl (* other.impl_p);
}

StateInterface &
StateInterface::operator= (const StateInterface & other)
{
    delete impl_p;
    impl_p = new StateInterfaceImpl (* other.impl_p);

    return * this;
}

StateInterface::~StateInterface ()
{
    delete impl_p;
}

void StateInterface::setState( const QString& jsonStr  ){
    _restoreState( jsonStr );
}


void
StateInterface::fetchState ()
{
    impl_p->oldState_p.CopyFrom (impl_p->state_p, impl_p->state_p.GetAllocator());
    QString json = fetchStateImpl ();
    _restoreState( json );
}

void StateInterface::_restoreState( const QString& json ){

    AsUtf8 jsonUtf8 (json);

    impl_p->state_p.Parse (jsonUtf8.data());

    if (impl_p->state_p.HasParseError()){

        QString message = QString ("StateInterface::fetchState: "
                                   "Error parsing JSON represtentation '%1'")
                              .arg (json);
        throw domain_error (message.toStdString());
    }
}

QString
StateInterface::fetchStateImpl ()
{
    // Put in appropriate PW state accessor to read the state
    IConnector * connector = Globals::instance()->connector();
    QString state = connector->getState( impl_p->path_p );
    return state;
}

void
StateInterface::flushState ()
{
    // Convert document to string

    QString json = toString();
    flushStateImpl (json);
}

QString StateInterface::toString() const {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    impl_p->state_p.Accept (writer);
    string json = buffer.GetString();
    return QString( json.c_str() );
}

QString
StateInterface::toString (const QString & keyString) const
{
    // This might be replicating the nullary signatured toString, but the
    // Document object doesn't seem to always play nice as the Value object
    // which it's supposed to derive from.

    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    value.Accept (writer);
    string json = buffer.GetString();
    return QString( json.c_str() );
}


void
StateInterfaceImpl::insertObjectAux (const QString & keyString, Value & valueToInsert)
{
    // Find the containing object by using all but the last key in the string.

    vector<QString> keys = getKeys (keyString);
    QString prefixKeyString = makeKeys (keys.begin(), keys.end() - 1);
    Value & value = getValueAux (prefixKeyString, state_p);
    QString lastKey = keys.back();

    if (prefixKeyString.isEmpty()){
        prefixKeyString = "*ROOT-OBJECT*"; // for error messages
    }

    if (! value.IsObject()){
        QString message = QString ("Cannot add member %1 to non-object %2")
                               .arg (lastKey)
                               .arg (prefixKeyString);
        throw invalid_argument (message.toStdString());
    }

    // Finagle the key value so that it's acceptable to the framework.

    AsUtf8 lastKeyUtf8 (lastKey);
    Value lastKeyValue;
    lastKeyValue.SetString (lastKeyUtf8.data(), lastKeyUtf8.size(), state_p.GetAllocator());

    if (value.HasMember (lastKeyValue)){
        QString message = QString ("Cannot add member %1 since it already exists in object %2")
                               .arg (lastKey)
                               .arg (prefixKeyString);
        throw invalid_argument (message.toStdString());
    }

    // Insert a field with the last component in the key string having the
    // value of the newly created null-filled array.

    value.AddMember (lastKeyValue, valueToInsert, state_p.GetAllocator());
}

void
StateInterface::insertObject (const QString & keyString, const QString & valueInJson)
{
    Document newValue;

    newValue.Parse (valueInJson.toStdString().c_str());

    if (newValue.HasParseError()){

        QString message = QString ("StateInterface::insertObject: "
                                   "Error parsing JSON representation '%1'")
                              .arg (valueInJson);
        throw domain_error (message.toStdString());
    }

    // Apparently it's needed to get a copy of the value out of the
    // just created document to prevent memory allocation problems
    // of some sort???

    Value copiedValue;
    copiedValue.CopyFrom (newValue, impl_p->state_p.GetAllocator());

    impl_p->insertObjectAux (keyString, copiedValue);
}



void
StateInterface::insertArray (const QString & keyString, int size)
{
    // Create a new value with an empty array.

    Value newArray;
    newArray.SetArray();

    // Create a null value and use it to fill in elements in the array.

    for (int i = 0; i < size; i++){
        Value nullObject;
        nullObject.SetObject();
        newArray.PushBack(nullObject, impl_p->state_p.GetAllocator());
    }

    impl_p->insertObjectAux (keyString, newArray);
}

void
StateInterface::insertObject (const QString & keyString)
{
    // Create a new value with an empty object

    Value newValue;
    newValue.SetObject();

    impl_p->insertObjectAux (keyString, newValue);

}

void StateInterface::resizeArray (const QString & keyString,
                             int size,
                             Preservation preservation)
{
    // Get the array value

    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    if (! value.IsArray()){
        QString message = QString ("StateInterface: Cannot resize '%1' since it is not an array")
                             .arg (keyString);
        throw invalid_argument (message.toStdString());
    }

    // Set up the array to hold the required number of elements

    int oldSize = value.Size();

    // value.Reserve (size, impl_p->state_p.GetAllocator());

    // Remove any array elements that are not in the current array size

    if (preservation == PreserveNone){

        value.Clear(); // all of them are gone
    }
    else if (size < oldSize){

        value.Erase (value.Begin() + size, value.End()); // only extra ones are removed

    }

    // Add null values to flesh out the array to the appropriate new size.


    int nToAdd = preservation == PreserveNone ? size
                                              : size - oldSize;

    for (int i = 0; i < nToAdd; i++){
        Value nullObject;
        nullObject.SetObject();
        value.PushBack(nullObject, impl_p->state_p.GetAllocator());
    }
}


void
StateInterface::flushStateImpl (const QString & val )
{
    // Put in appropriate PW state accessor to write out the state
    IConnector * connector = Globals::instance()->connector();
    connector->setState( impl_p->path_p, val );
}

std::vector <QString> StateInterfaceImpl::getKeys (const QString & keyString) const
{
    vector <QString> keys;

    size_t delimiterLength = StateInterface::DELIMITER.size ();

    int end = 0;
    int start = 0;

    while ( (end = keyString.indexOf (StateInterface::DELIMITER, start)) != -1) {

        keys.push_back (keyString.mid (start, end - start));

        start = end + delimiterLength;
    }
    QString lastPart = keyString.mid (start, keyString.size() - start);
    keys.push_back(lastPart);

    return keys;
}

template<typename Iterator>
QString
StateInterfaceImpl::makeKeys (Iterator iterator, const Iterator & end) const
{

    QString result = "";

    while (iterator != end){

        result += * iterator ++;
        if (iterator != end){
            result += StateInterface::DELIMITER;
        }
    }

    return result;
}

void StateInterface::getTypedValue (bool & typedValue, const QString & keyString) const
{
    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);
    typedValue = value.GetBool();
}

void StateInterface::getTypedValue (double & typedValue, const QString & keyString) const
{
    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);
    typedValue = value.GetDouble();
}

void StateInterface::getTypedValue (int & typedValue, const QString & keyString) const
{
    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);
    typedValue = value.GetInt ();
}

void StateInterface::getTypedValue (int64_t & typedValue, const QString & keyString) const
{
    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);
    typedValue = value.GetInt64 ();
}

void StateInterface::getTypedValue (QString & typedValue, const QString & keyString) const
{
    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);
    typedValue = value.GetString ();
}

void StateInterface::getTypedValue (uint & typedValue, const QString & keyString) const
{
    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);
    typedValue = value.GetUint ();
}

void StateInterface::getTypedValue (uint64_t & typedValue, const QString & keyString) const
{
    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);
    typedValue = value.GetUint64();
}


const Value &
StateInterfaceImpl::getValueAux (const QString & keyString, const Document & state) const
{
    return *(_getValueAux( keyString, state ));
}

Value &
StateInterfaceImpl::getValueAux (const QString & keyString, Document & state) const
{
    return *(_getValueAux( keyString, state ) );
}

Value *
StateInterfaceImpl::_getValueAux( const QString& keyString, const Document& state ) const {

    // Split the keyString up into a vector of keys.

    vector<QString> keys =  getKeys (keyString);

    if (keys.size() == 0 || keys[0].trimmed().size() == 0 ){

        // If there are no keys, just return the whole state document.

        return const_cast<Document*>(&state);
    }

    if (! state.HasMember(keys[0].toUtf8().data())){
        QString message = QString ("StateInterfaceImpl: No such top-level member '%1'")
                              .arg (keys[0]);
        throw invalid_argument (message.toStdString());
    }

    Value * value = const_cast<Value*>( & (state [keys[0].toUtf8().data()]));

    QString keysUsed = keys[0]; // path already used; used for error messages

    for (int i = 1; i < (int) keys.size(); i++){

        // Use each successive key to walk through the value tree.

        if ( value->IsObject()){

            // Check to see if the operation will fail and if so throw an
            // exception.

            if ( ! value->HasMember( keys[i].toUtf8().data())){
                QString errMsg( "StateInterfaceImpl: No such member '" +
                        keysUsed + StateInterface::DELIMITER + keys[i] + "'");
                throw invalid_argument( errMsg.toStdString());
            }

            // Navigate another step down the tree.

            value = & ((* value) [keys[i].toUtf8().data()]);
        }
        else if ( value->IsArray()){

            // Value is an array so the key ought to be a nonnegative number that is
            // within the size of the array.

            bool isValidInt = false;
            int keyAsInteger = keys[i].toInt( &isValidInt );

            if ( ! isValidInt ){
                QString message = QString ( "StateInterfaceImpl:: Array index should be integer '%1' at '%2'")
                                     .arg (keys[i])
                                     .arg (keysUsed);
                throw invalid_argument (message.toStdString());
            }

            if ( keyAsInteger < 0 || keyAsInteger >= static_cast<int>(value->Size())){
                QString errMsg( "StateInterfaceImpl: Index " + keys[i] +
                                " out of bounds for array '" + keysUsed + "'");
                throw invalid_argument( errMsg.toStdString());
            }

            value = & ((* value) [keyAsInteger]);
        }
        else {

            // Cannot use key[i] because the previous value is neither an object nor an array.
            // Throw an exception!

            QString message =
                QString ( "StatInterfaceImpl:: Request for field '%1' is not possible since "
                          "'%2' is neither an array nor object.")
                     .arg (keys[i])
                     .arg (keysUsed);
            throw invalid_argument (message.toStdString());
        }

        keysUsed += StateInterface::DELIMITER + keys[i];
    }

    return value;
}

bool
StateInterface::hasChanged (const QString & keyString) const
{
    const Value & oldValue = impl_p->getValueAux (keyString, impl_p->oldState_p);
    const Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    return oldValue != value;
}

void StateInterface::setTypedValue (const bool & typedValue, const QString & keyString) const
{
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetBool (typedValue);
}

void StateInterface::setTypedValue (const double & typedValue, const QString & keyString) const
{
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetDouble (typedValue);
}

void StateInterface::setTypedValue (const int & typedValue, const QString & keyString) const
{
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetInt  (typedValue);
}

void StateInterface::setTypedValue (const int64_t & typedValue, const QString & keyString) const
{
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetInt64  (typedValue);
}

void StateInterface::setTypedValue (const QString & typedValue, const QString & keyString) const
{
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    // Convert the value to a byte array using Utf8.

    AsUtf8 typedValueUtf8 (typedValue);

    value.SetString  (typedValueUtf8.data(), typedValueUtf8.size(),
                      impl_p->state_p.GetAllocator());
}

void StateInterface::setTypedValue (const uint & typedValue, const QString & keyString) const
{
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetUint  (typedValue);
}

void StateInterface::setTypedValue (const uint64_t & typedValue, const QString & keyString) const
{
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetUint64 (typedValue);
}

void StateInterface::insertNull (const QString & keyString)
{
    insertObject (keyString); // first it's an empty object

    setNull (keyString); // now it's a null
}

void
StateInterface::setObject (const QString & keyString)
{
    // Replace the current value with an empty object

    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetObject();
}

void
StateInterface::setObject (const QString & keyString, const QString & valueInJson)
{
    // Replace the current value with an empty object

    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    Document newDocument;
    newDocument.Parse (valueInJson.toStdString().c_str());

    if (newDocument.HasParseError()){

        QString message = QString ("StateInterface::insertObject: (inserting into '%2') "
                                   "Error parsing JSON representation '%1'")
                              .arg (valueInJson).arg (keyString);
        throw domain_error (message.toStdString());
    }

    value.SetObject();
    value.CopyFrom (newDocument, impl_p->state_p.GetAllocator());
}



void
StateInterface::setNull (const QString & keyString)
{
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetNull (); // it's null now!
}

int StateInterface::getArraySize( const QString& keyString ) const {
    int arraySize = 0;
    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);
    if ( value.IsArray() ){
        arraySize = value.Size();
    }
    return arraySize;
}

void
StateInterface::setArray (const QString & keyString, int size)
{
    // Replace the current value with an empty object

    Value & value = impl_p->getValueAux (keyString, impl_p->state_p);

    value.SetArray();

    resizeArray (keyString, size);
}

QList<QString>
StateInterface::getMemberNames (const QString & keyString) const {

    // Get the requested object
    const Value * value = impl_p->_getValueAux (keyString, impl_p->state_p);
    if (! value->IsObject()){
        QString message = QString ("StateInterface::getMemberNames: '%1' is not an object.")
                          .arg (keyString);
        throw invalid_argument (message.toStdString());
    }

    // Get the object's member iterator, and use it to extract the member names
    Value::ConstMemberIterator iterator = value->MemberBegin();
    QList<QString> result;
    while (iterator != value->MemberEnd()){
        result.append (QString (iterator->name.GetString()));
        iterator ++;
    }
    return result;
}
}
}

