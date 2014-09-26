/**
 *
 **/

#include "State.h"

#include <QDebug>

State::State() {
    _initialize("", "", false, true);
}

State::State(const QString& key, const QString& description, bool persistent,
        bool serverOnly) {
    _initialize(key, description, persistent, serverOnly);
}

void State::_initialize(const QString& key, const QString& description,
        bool persistent, bool serverOnly) {
    m_key = key;
    m_persistent = persistent;
    m_description = description;
    m_serverOnly = serverOnly;
}

QString State::getPath() const {
    return m_key;
}

bool State::isPersistent() const {
    return m_persistent;
}

bool State::isServerOnly() const {
    return m_serverOnly;
}

bool State::operator==(const State& other) {
    return other.m_key == m_key;
}
