/***
 * Meshes together data, animator(s), and view(s).
 *
 */

#pragma once

#include <memory>
#include <QObject>

class TestView3;

class DataController : public QObject
{

    Q_OBJECT

public:

    // instantiate a DataController
    DataController( const QString& fileName );

    //Sets an animator this data controller will listen to.
    void setId( const QString& animId );

    //Sets a view that will be updated.
    void createImageView( const QString& winId );

private:
	void _initializeStates();
	void _initializeCommands();
	void _loadView( int frame );
	int _setFrame( const QString& params );

	QList<std::shared_ptr<TestView3> > m_views;
	QString m_id;
	QString m_fileName;

};
