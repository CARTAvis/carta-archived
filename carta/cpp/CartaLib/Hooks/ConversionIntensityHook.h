/**
 * Hook for converting intensity units.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/IntensityUnitConverter.h"

namespace Carta
{
namespace Lib
{
namespace Image {
class ImageInterface;
}
namespace Hooks
{
class ConversionIntensityHook : public BaseHook
{
    CARTA_HOOK_BOILER1( ConversionIntensityHook );

public:

   typedef Carta::Lib::IntensityUnitConverter::SharedPtr ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params( std::shared_ptr<Image::ImageInterface> dataSource,
                    const QString& oldUnit, const QString& newUnit,
                    double maxYValue, const QString& maxUnit){
                m_dataSource = dataSource;
                m_oldUnit = oldUnit;
                m_newUnit = newUnit;
                m_maxUnit = maxUnit;
                m_maxValueY = maxYValue;
            }

            std::shared_ptr<Image::ImageInterface> m_dataSource;
            QString m_newUnit;
            QString m_oldUnit;
            QString m_maxUnit;
            double m_maxValueY;

        };

    /**
     * @brief PreRender
     * @param pptr
     *
     * @todo make hook constructors protected, so that only hook helper can create them
     */
    ConversionIntensityHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
