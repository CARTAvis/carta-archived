/***
 * Factory for adding data (images or regions) to a controller.
 */

#pragma once
#include <QString>
#include <memory>
#include <vector>

namespace Carta {

namespace Data {

class Controller;
class Region;

class DataFactory {


public:
    /**
     * Add data from a file to the given controller; this could be region or
     * image data.
     * @param controller - the controller that will manage the data.
     * @param fileName - the absolute path to a file or directory containing the
     *      data.
     * @param success - set to true for a successful image load; false otherwise.
     * @return - an error message if the data could not be added; an identifier for
     *      the carta object managing the image if the data was added.
     */
    static QString
    addData( Controller* controller, const QString& fileName, bool* success );


    virtual ~DataFactory();

private:

    /**
     * Returns true if the file is a recognized region file; false otherwise.
     * @param fileName - an identifier for a file.
     * @return - true if the file is a recognized format for region files; false otherwise.
     */
    static bool _isRegion( const QString& fileName );

    static std::vector<std::shared_ptr<Region> > _loadRegions( Controller* controller,
            const QString& fileName, bool* success, QString& errorMsg );

    /**
     * Constructor.
     */
    DataFactory();

    DataFactory( const DataFactory& other);
    DataFactory& operator=( const DataFactory& other );

};
}
}
