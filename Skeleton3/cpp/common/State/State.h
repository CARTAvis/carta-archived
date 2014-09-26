/**
 *
 **/

#pragma once

#include <QString>

class State {
public:
    State(const QString& key, const QString& description, bool persistent,
            bool serverOnly);
    State();
    bool operator==(const State& other);

    /**
     * Returns the lookup path for the State.
     * @return a QString representing a lookup identifier for the State.
     */
    QString getPath() const;

    /**
     * Returns whether or not the State should be stored.
     * @return true if the State should be stored; false otherwise.
     */
    bool isPersistent() const;

    /**
     * Returns whether or not the State should be propagated to the client(s).
     * @return true if the state should not be propagated; false otherwsise.
     */
    bool isServerOnly() const;
private:
    void _initialize(const QString& key, const QString& description,
            bool persistent, bool serverOnly);
    QString m_key;
    QString m_description;
    bool m_serverOnly;
    bool m_persistent;
};

