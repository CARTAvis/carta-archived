
/**
 *
 **/

#pragma once


#include <QString>


class State {
public:
	State( const QString& key, const QString& description, bool persistent);
	State( );
	bool operator==(const State& other);
	QString getPath() const;
	bool isPersistent() const;
private:
	void _initialize(const QString& key, const QString& description, bool persistent);
	QString m_key;
	QString m_description;
	bool m_persistent;
};



