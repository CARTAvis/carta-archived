#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/ImageStatisticsHook.h"
#include "CartaLib/IImage.h"
#include "plugins/CasaImageLoader/CCImage.h"

#include "StatisticsCASA.h"
#include "StatisticsCASAImage.h"
#include "StatisticsCASARegion.h"

#include <QDebug>


QString StatisticsCASA::STAT_ID = "name";

StatisticsCASA::StatisticsCASA( QObject * parent ) :
    QObject( parent )
{ }


bool
StatisticsCASA::handleHook( BaseHook & hookData ){
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::ImageStatisticsHook > () ) {
        Carta::Lib::Hooks::ImageStatisticsHook & hook
            = static_cast < Carta::Lib::Hooks::ImageStatisticsHook & > ( hookData );
        std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface> > images = hook.paramsPtr-> m_dataSources;
        int imageCount = images.size();
        if ( imageCount == 0 ) {
            qDebug() << "No image, statistics returning false";
            return false;
        }

        QList< QList< QMap<QString,QString> > > imageResults;
        for ( int i = 0; i < imageCount; i++ ){
            std::shared_ptr<Carta::Lib::Image::ImageInterface> image = images[i];
            if ( !image.get() ){
                qWarning() << "Missing image for statistics";
                continue;
            }
            const casa::ImageInterface<casa::Float>* casaImage = cartaII2casaII_float( image );
            if( ! casaImage) {
                qWarning() << "Image statistics plugin: not an image created by casaimageloader...";
                return false;
            }

            QList< QMap<QString,QString> > statResults;

            //Get the image statistics
            QMap<QString,QString> statResultImage = StatisticsCASAImage::getStats( casaImage );
            statResults.append( statResultImage );

            //Get the region statistics if there are some
            std::vector<Carta::Lib::RegionInfo> regionInfos = hook.paramsPtr->m_regionInfos;
            int regionCount = regionInfos.size();
            for ( int i = 0; i < regionCount; i++ ){
                QMap<QString,QString> statResultRegion = StatisticsCASARegion::getStats( casaImage, regionInfos[i] );
                statResults.append( statResultRegion );
            }
            imageResults.append( statResults );

        }
        hook.result = imageResults;

        return true;
    }
    qWarning() << "Image statistics doesn't know how to handle this hook";
    return false;
} // handleHook

std::vector < HookId >
StatisticsCASA::getInitialHookList()
{
    return {
               Carta::Lib::Hooks::Initialize::staticId,
               Carta::Lib::Hooks::ImageStatisticsHook::staticId
    };
}

StatisticsCASA::~StatisticsCASA() {

}
