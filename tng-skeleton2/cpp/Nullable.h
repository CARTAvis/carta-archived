/** Templated container holding a single value of type T
 *
 * The value can be set at construction time only. The default constructor creates
 * a container with no value set.
 *
 * This is useful for example for functions that may or may not return a value. Alternative
 * would be a std::pair<bool,T> or maybe an exception.
 **/

#pragma once

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

    bool isNull() const { return m_isNull; }
    bool isSet() const { return ! isNull(); }
    explicit operator bool() const { return ! isNull(); }

    const T & val() const { return m_val; }

protected:

    bool m_isNull;
    T m_val;
};

