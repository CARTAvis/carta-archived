/**
 * Hook loading astronomical images.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "core/Hacks/ProfileExtractor.h"
#include <vector>

namespace Carta
{
namespace Lib
{
namespace Hooks
{
/// load an (astronomical) image and convert to an instance of Image::ImageInterface
class GetProfileExtractor : public BaseHook
{
    CARTA_HOOK_BOILER1( GetProfileExtractor );

public:

    struct ResultType {
        double priority = 1e9;
        Carta::Lib::Profiles::IProfileExtractor::SharedPtr extractor;
    };

    struct Params {
        Params( QString p_fileName )
        {
            fileName = p_fileName;
        }

        QString fileName;
    };

    GetProfileExtractor( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
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
