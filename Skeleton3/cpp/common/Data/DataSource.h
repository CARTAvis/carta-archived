/***
 * Manages and loads a single source of data.
 */

#pragma once

#include "Region.h"
#include "Nullable.h"

#include <QImage>

class DataSource {

public:

    /**
     * Constructor.
     * @param fileName a locator for finding the data.
     */
    DataSource( const QString& fileName );

    /**
     * Loads the data source as a QImage.
     * @param frameIndex the channel to load.
     */
    Nullable<QImage> load( int frameIndex ) const;

    /**
     * Returns true if this data source manages the data corresponding
     * to the fileName; false, otherwise.
     * @param fileName a locator for data.
     */
    bool contains( const QString& fileName ) const;

    virtual ~DataSource();

private:
    QString m_fileName;
	Region m_region;
};
