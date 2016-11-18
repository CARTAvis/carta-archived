#include "DataFactory.h"
#include "Data/DataLoader.h"
#include "Data/Image/Controller.h"
#include "Data/Image/DataSource.h"
#include "Data/Region/Region.h"
#include "Data/Region/RegionControls.h"
#include "Data/Region/RegionFactory.h"
#include "Data/Util.h"
#include "CartaLib/Hooks/LoadRegion.h"
#include "Globals.h"

#include <QDebug>

namespace Carta {

namespace Data {


DataFactory::DataFactory(){
}


QString DataFactory::addData( Controller* controller, const QString& fileName, bool* success ){
    QString result;
    *success = false;
    if ( controller ){
        QFileInfo fileInfo( fileName );
        bool dataFile = fileInfo.isFile();
        if ( dataFile ){
            bool regionFile = _isRegion( fileName );
            //If we think it is a region, see if any of the region parsing
            //plugins can handle it.
            if ( regionFile ){
                std::vector<std::shared_ptr<Region> > regions =
                        _loadRegions( controller, fileName, success, result );
                if ( regions.size() > 0 ){
                	std::shared_ptr<RegionControls> regionController = controller->getRegionControls();
                	if ( regionController ){
                		regionController->_addDataRegions( regions );
                	}
                }
            }
        }
        //Try loading it as an image.
        if ( !(*success) ){
            result = controller->_addDataImage( fileName, success );
        }
    }
    else {
        result = "The data in "+fileName +" could not be added because no controller was specified.";
    }
    return result;
}


bool DataFactory::_isRegion( const QString& fileName ){
    bool regionFile = false;
    if ( fileName.endsWith( DataLoader::CRTF) ){
        regionFile = true;
    }
    else if ( fileName.endsWith( ".reg")){
        regionFile = true;
    }
    else {
        QFile file( fileName );
        if ( file.open( QIODevice::ReadOnly | QIODevice::Text)){
            char buf[1024];
            qint64 lineLength = file.readLine( buf, sizeof(buf));
            if ( lineLength > 0 ){
                QString line( buf );
                if ( line.startsWith( "#CRTF") ){
                    regionFile = true;
                }
                else if ( line.startsWith( "# Region file format: DS9") ){
                    regionFile = true;
                }
                //Region files for unspecified plug-ins?
                else if ( line.contains( "region", Qt::CaseInsensitive) ){
                    regionFile = true;
                }
            }
        }
    }
    return regionFile;
}


std::vector<std::shared_ptr<Region> > DataFactory::_loadRegions( Controller* controller,
        const QString& fileName, bool* success, QString& errorMsg ){
    std::vector< std::shared_ptr<Region> > regions;
    std::shared_ptr<DataSource> dataSource = controller->getDataSource();
    if ( dataSource ){

        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = dataSource->_getImage();
        auto result = Globals::instance()-> pluginManager()
              -> prepare <Carta::Lib::Hooks::LoadRegion>(fileName, image );
        auto lam = /*[=]*/[&regions,fileName] ( const Carta::Lib::Hooks::LoadRegion::ResultType &data ) {
            int regionCount = data.size();
            //Return whether to continue the loop or not.  We continue until we
            //find the first plugin that can handle the region format and generate
            //one or more regions.
            bool continueLoop = true;
            if ( regionCount > 0 ){
                continueLoop = false;
            }
            for ( int i = 0; i < regionCount; i++ ){
                if ( data[i] ){
                    std::shared_ptr<Region> regionPtr = RegionFactory::makeRegion( data[i] );
                    regions.push_back( regionPtr );
                }
            }
            return continueLoop;
        };

        try {
            //Find the first plugin that can load the region.
            result.forEachCond( lam );
            *success = true;
        }
        catch( char*& error ){
            errorMsg = QString( error );
            *success = false;
        }
    }
    return regions;
}


DataFactory::~DataFactory(){

}
}
}
