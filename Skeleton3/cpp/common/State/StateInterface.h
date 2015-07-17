#pragma once

#include <vector>
#include <cassert>

#include <QtCore/QString>

using namespace std;

namespace Carta {

namespace State {


class StateInterfaceImpl;

class StateInterface {

public:

    enum Preservation { PreserveNone, PreserveAll};

    friend class StateInterfaceTestImpl;

    static const QString DELIMITER;
    static const QString FLUSH_STATE;
    static const QString STATE_DATA;
    static const QString OBJECT_TYPE;
    static const QString INDEX;

    StateInterface (const QString & path, const QString& type = "", const QString& initialState="");
    StateInterface (const StateInterface & other);

    virtual ~StateInterface ();

    StateInterface & operator= (const StateInterface & other);

    // fetchState() - loads the state from the central store
    // flushState() - flushes the state back to the central store
    // toString() - converts the state to a QSstring representation (JSON)

    void fetchState ();
    void flushState ();
    void refreshState();
    QString toString() const;
    QString toString (const QString & keyString) const;



    // The routines that follow modify the state as currently stored in this
    // object.  They use a keystring which consists of dot-separated field
    // names (e.g., a.b.c).  If the field being dereferenced is an array then
    // the field name should be an integer (e.g., a.b.anArray.1 would access the
    // second element of anArray---zero-based indexing).


    template <typename T>
    T getOldValue (const QString & keyString, T defaultValue ) const;

    // The getValue routine are used with an explicit template type.  They return
    // the value stored at the location in keyString.  If no such value exists an
    // invalid_argument exception is thrown.

    template <typename T>
    T getValue (const QString & keyString) const;

    // hasChanged - returns true if the specified valuehas changed between the
    // current value and the previous time it was fetched.  Usually called after
    // doing a fetchState().

    bool hasChanged (const QString & keyString) const;

    QList<QString> getMemberNames (const QString & keyString) const;

    // The array routines are used to insert or resize array values.
    //
    // insertArray -- adds a new, array-valued member; doing setArray ("a.b.c", 3) will add
    // a new member, "c", to "a.b" which has the value of a three-member array.  The
    // elements of the array are null-valued.  If the specified location prefix (all but the
    // last field) does not exist or is not an object then an invalid_argument
    // exception will be thrown.
    //
    // setArray -- replaces the value of the specified member with a new array of the
    // specified size.  The specified member must exist or an invalid_argument exception
    // will be thrown.
    //
    // resizeArray -- used to change the size of an existing array.  By default all values
    // of the new array will be null-valued.  If preservation is PreserveAll then up to "size"
    // elements of the existing array will be preserved in the new array; if the array is
    // expanding, then the new values will be null-valued.


    void insertArray (const QString & keyString, int size);
    void setArray( const QString& keyString, int size );
    void resizeArray (const QString & keyString, int size,
                      Preservation preservation = PreserveNone);
    int getArraySize( const QString& keyString ) const;

    // The object routines are used to install an empty object into the specified location.
    //
    // insertObject -- adds a new member at the specified location and sets its value to be
    // an empty object (i.e., "{}").  Example: insertObject ("w.x.y") adds a new member "y" to
    // object w.x and sets its value to {}.  If the specified location prefix (all but the last
    // component of the keystring) does not exist or does not contain an object then an
    // invalid_argument exception will be thrown.
    //
    // setObject -- replaces the value of an existing member with a null object.  If the member
    // does not exist an invalid_argument exception is thrown.

    void insertObject (const QString & keyString);
    void insertObject( const QString& keyString, const QString& valueInJson );
    void setObject( const QString& keyString );
    void setObject( const QString& keyString, const QString& valueInJson );

    // These routines set or insert a value into the state at the specified keystring.  The setters
    // require that the specified path exist.  The inserters require that the path prefix (all but
    // the last component in the keyString) exist and be an object; otherwise an invalid_argument
    // exception is thrown.
    //
    // setValue<type> -- Replaces the current value at keyString with one of thespecified type.
    //
    // insertValue<type> -- Adds a new value at keyString.
    //
    // insertNull -- Adds a new null value at keyString.
    // setNull -- replaces the current value at keyString with null.

    template <typename T>
    void setValue (const QString & keyString, const T & newValue);
    template <typename T>
    void insertValue (const QString & keyString, const T & newValue);
    void insertNull( const QString& keyString );
    void setNull( const QString& keyString );

    void setState( const QString& json );

protected:


private:

    StateInterfaceImpl * impl_p;

// Testing hooks

    virtual QString fetchStateImpl ();
    virtual void flushStateImpl (const QString &);

    void getTypedValue (bool & typedValue, const QString & keyString) const;
    void getTypedValue (double & typedValue, const QString & keyString) const;
    void getTypedValue (int & typedValue, const QString & keyString) const;
    void getTypedValue (int64_t & typedValue, const QString & keyString) const;
    void getTypedValue (QString & typedValue, const QString & keyString) const;
    void getTypedValue (uint & typedValue, const QString & keyString) const;
    void getTypedValue (uint64_t & typedValue, const QString & keyString) const;

    void setTypedValue (const bool & typedValue, const QString & keyString) const;
    void setTypedValue (const double & typedValue, const QString & keyString) const;
    void setTypedValue (const int & typedValue, const QString & keyString) const;
    void setTypedValue (const int64_t & typedValue, const QString & keyString) const;
    void setTypedValue (const QString & typedValue, const QString & keyString) const;
    void setTypedValue (const uint & typedValue, const QString & keyString) const;
    void setTypedValue (const uint64_t & typedValue, const QString & keyString) const;

    void _restoreState( const QString& json );

};




template <typename T>
T StateInterface::getValue (const QString & keyString) const
{
    //QString fullPath = impl_p->path_p + DELIMITER + keyString;
    T typedValue;
    getTypedValue (typedValue, keyString);

    return typedValue;
}

template <typename T>
void StateInterface::insertValue (const QString & keyString, const T & newValue)
{
    insertObject (keyString);
//printf ("Inserted\n");
//flushState();
    setTypedValue (newValue, keyString);
//printf ("Set\n");
//flushState();
}

template <typename T>
void StateInterface::setValue (const QString & keyString, const T & newValue)
{
    setTypedValue (newValue, keyString);
}
}
}

