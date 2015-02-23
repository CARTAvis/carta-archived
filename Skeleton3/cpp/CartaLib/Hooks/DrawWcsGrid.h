/**
 * Hook loading astronomical images.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include <QImage>
#include <vector>

namespace Carta
{
namespace Lib
{
namespace Hooks
{
/// load an (astronomical) image and convert to an instance of Image::ImageInterface
class DrawWcsGrid : public BaseHook
{
    CARTA_HOOK_BOILER1( DrawWcsGrid )

public:

    typedef QImage ResultType;
    struct Params {
        Params( Image::ImageInterface::SharedPtr image )
        {
            m_astroImage = image;
        }

        Image::ImageInterface::SharedPtr m_astroImage;
        QSize outputSize;
        QRectF frameRect, outputRect;
        QStringList options;
    };

    DrawWcsGrid( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
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
