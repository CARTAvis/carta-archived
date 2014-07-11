/**
 *
 **/

#include "State.h"

#include <QDebug>

State::State( ){
	_initialize( "", "", false );
}

State::State( const QString& key, const QString& description, bool persistent){
	_initialize( key, description, persistent );
}

void State::_initialize(const QString& key, const QString& description, bool persistent){
	m_key = key;
	m_persistent = persistent;
	m_description = description;
}

QString State::getPath() const {
	return m_key;
}

bool State::isPersistent() const {
	return m_persistent;
}

bool State::operator==(const State& other) {
	return other.m_key == m_key;
}
