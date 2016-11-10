/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/Regions/ICoordSystem.h"

namespace Cartab
{
namespace Lib
{
namespace Hooks
{
/// \brief Hook for deserializing a coordinate system converter from a JSON string
///
class DeserializeCoordSysConverterHook : public BaseHook
{
    CARTA_HOOK_BOILER1( DeserializeCoordSysConverterHook );

public:

    typedef Carta::Lib::Regions::ICoordSystemConverter * ResultType;
    typedef struct {
        QString serializedCS;
    } Params;

    /// standard constructor (could be probably a macro)
    DeserializeCoordSysConverterHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        // force instantiation of templates
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr = nullptr;
};

/// Hook for creating converters for basic coordinate systems (e.g. galactic to ecliptic)
class MakeBasicCoordConverterHook : public BaseHook
{
    CARTA_HOOK_BOILER1( MakeBasicCoordConverterHook );

public:

    typedef Carta::Lib::Regions::ICoordSystemConverter * ResultType;
    typedef struct {
        Carta::Lib::Regions::BasicCoordinateSystemInfo srcCS;
        Carta::Lib::Regions::BasicCoordinateSystemInfo dstCS;
    } Params;

    /// standard constructor (could be probably a macro)
    MakeBasicCoordConverterHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        // force instantiation of templates
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr = nullptr;
};


}
}
}
