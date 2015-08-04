/**
 * Templated container holding a single value of type T
 *
 * The value can be set at construction time only. The default constructor creates
 * a container with no value set.
 *
 * This is useful for example for functions that may or may not return a value. Alternative
 * would be a std::pair<bool,T> or maybe an exception.
 **/

#pragma once

#include <exception>
#include <stdexcept>
template <typename T>
class Nullable
{

public:

    /// default constructor makes a nulled version
    Nullable()
        : m_isNull( true)
    {}

    /// initializer from value
    Nullable( const T & val)
        : m_isNull( false)
        , m_val( val)
    {}

    /// returns true if the instance holds no value
    bool isNull() const { return m_isNull; }

    /// returns true if the instance holds a value
    bool isSet() const { return ! isNull(); }

    /// safe bool check to see if the value was set
    explicit operator bool() const { return ! isNull(); }

    /// returns a reference to the stored value
    const T & val() const {
        if( isNull()) {
            throw std::logic_error( "Trying to get value of Nullable<> with no value set");
        }
        return m_val;
    }

//    /// returns a reference to the stored value
//    T & val() {
//        if( isNull()) {
//            throw std::logic_error( "Trying to get value of Nullable<> with no value set");
//        }
//        return m_val;
//    }


protected:

    /// is the value set?
    bool m_isNull;

    /// copy of the value
    T m_val;
};

