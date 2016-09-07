/**
 * Hook loading astronomical regions.
 *
 **/

#pragma once

#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"

namespace Carta
{
namespace Lib
{
namespace Hooks
{
/// load a region and convert to an instance of RegionInfo
class LoadRegion : public BaseHook
{
    CARTA_HOOK_BOILER1( LoadRegion );

public:

    typedef std::vector<Carta::Lib::Regions::RegionBase* > ResultType;
    struct Params {
        Params( QString p_fileName, std::shared_ptr<Image::ImageInterface> p_imagePtr )
        {
            fileName = p_fileName;
            image = p_imagePtr;
        }

        QString fileName;
        std::shared_ptr<Image::ImageInterface> image;
    };

    LoadRegion( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        // force instantiation of templates
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
