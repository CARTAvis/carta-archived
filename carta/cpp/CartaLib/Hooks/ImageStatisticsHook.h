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

   //Each list item consists of a complete set of statistics for an entire
   //image or region.  Statistics for an image/region consist of (key,value)
   //pairs where the key identifies the type of statistics and the value is its
   //numerical value.
   typedef QList< QMap<QString,QString> > ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params( std::shared_ptr<Image::ImageInterface> p_dataSource,
                    std::vector<Carta::Lib::RegionInfo> regionInfos
                    ){
                m_dataSource = p_dataSource;
                m_regionInfos = regionInfos;
            }

            std::shared_ptr<Image::ImageInterface> m_dataSource;
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
