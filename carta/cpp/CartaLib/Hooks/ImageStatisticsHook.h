/**
 * Hook for adding image/region statistics to the system.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/RegionInfo.h"
#include <QMap>
#include <QList>

namespace Carta
{
namespace Lib
{
namespace Image {
class ImageInterface;
}
namespace Hooks
{
class ImageStatisticsHook : public BaseHook
{
    CARTA_HOOK_BOILER1( ImageStatisticsHook );

public:
   //The outer list has statistics for each image that is loaded (a list item
   //for each image.

   //The inner list has statistics for a particular image and regions in that
   //image.  The first list item is the statistics for the image.

   //Each inner list item consists of a complete set of statistics for an entire
   //image or region.  Statistics for an image/region consist of (key,value)
   //pairs where the key identifies the type of statistics and the value is its
   //numerical value.
   typedef QList< QList< QMap<QString,QString> > > ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params( std::vector< std::shared_ptr<Image::ImageInterface> > p_dataSources,
                    std::vector<Carta::Lib::RegionInfo> regionInfos
                    ){
                m_dataSources = p_dataSources;
                m_regionInfos = regionInfos;
            }

            std::vector<std::shared_ptr<Image::ImageInterface> > m_dataSources;
            std::vector<Carta::Lib::RegionInfo> m_regionInfos;
        };

    /**
     * @brief PreRender
     * @param pptr
     *
     * @todo make hook constructors protected, so that only hook helper can create them
     */
    ImageStatisticsHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
