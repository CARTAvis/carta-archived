/**
 * Hook for converting intensity units.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"

namespace Carta
{
namespace Lib
{
namespace Image {
class ImageInterface;
}
namespace Hooks
{
class ConversionSpectralHook : public BaseHook
{
    CARTA_HOOK_BOILER1( ConversionSpectralHook );

public:

   typedef std::vector<double> ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params( std::shared_ptr<Image::ImageInterface> dataSource,
                    QString oldUnit, QString newUnit,
                    std::vector<double> inputValues ){
                m_dataSource = dataSource;
                m_oldUnit = oldUnit;
                m_newUnit = newUnit;
                m_inputList = inputValues;
            }

            std::shared_ptr<Image::ImageInterface> m_dataSource;
            std::vector<double> m_inputList;
            QString m_newUnit;
            QString m_oldUnit;
        };

    /**
     * @brief PreRender
     * @param pptr
     *
     * @todo make hook constructors protected, so that only hook helper can create them
     */
    ConversionSpectralHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
