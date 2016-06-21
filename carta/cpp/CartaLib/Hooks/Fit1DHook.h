/**
 * Hook for finding a fit to 1-d curve data.
 *
 **/

#pragma once
#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/Fit1DInfo.h"
#include "CartaLib/Hooks/FitResult.h"
#include <future>

namespace Carta
{
namespace Lib
{

namespace Hooks
{


class Fit1DHook : public BaseHook
{
    CARTA_HOOK_BOILER1( Fit1DHook );

public:
   //The results from generating a 1D fit to a curve.
    typedef FitResult ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params( const Carta::Lib::Fit1DInfo& fitInfo ){
                m_fitInfo = fitInfo;
            }

            Carta::Lib::Fit1DInfo m_fitInfo;
        };

    /**
     * @brief PreRender
     * @param pptr
     *
     * @todo make hook constructors protected, so that only hook helper can create them
     */
    Fit1DHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
