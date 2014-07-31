#include "DataSource.h"
#include "Globals.h"
#include "PluginManager.h"

#include <QDebug>


DataSource::DataSource( const QString& fileName ){
	m_fileName = fileName;
}

Nullable<QImage> DataSource::load(int frameIndex) const {
	Nullable<QImage> res;
	if ( m_fileName.trimmed().length() > 0  ){
		auto & globals = * Globals::instance();
		auto loadImageHookHelper = globals.pluginManager()-> prepare<LoadImage>( m_fileName, frameIndex );
		res = loadImageHookHelper.first();
	}
	return res;
}


bool DataSource::contains( const QString& fileName ) const {
	bool representsData = false;
	if ( m_fileName == fileName ){
		representsData = true;
	}
	return representsData;
}

DataSource::~DataSource(){

}
