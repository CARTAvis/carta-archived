/**
 * Hook for adding image/region statistics to the system.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/StatInfo.h"
#include "CartaLib/Regions/IRegion.h"
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
   //image or region.  Statistics for an image/region consist of a type, a label,
   //and a value.
   typedef QList< QList< QList<Carta::Lib::StatInfo> > > ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params( std::vector< std::shared_ptr<Image::ImageInterface> > p_dataSources,
                    std::vector< std::shared_ptr<Carta::Lib::Regions::RegionBase> > regionInfos,
                    std::vector<int> slice
                    ){
                m_dataSources = p_dataSources;
                m_regionInfos = regionInfos;
                m_slice = slice;
            }

            std::vector<std::shared_ptr<Image::ImageInterface> > m_dataSources;
            std::vector<std::shared_ptr<Carta::Lib::Regions::RegionBase> > m_regionInfos;
            std::vector<int> m_slice;
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
