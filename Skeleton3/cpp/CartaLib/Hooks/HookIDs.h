/**
 * I have not decided on an automated system for asigning unique static IDs to hooks,
 * so for now we'll do it manually. And the weapon of choice template specialization
 * with enums (ugly, awkward, but easy to spot bugs)
 **/

#pragma once

#include <cstdint>

namespace Carta
{
namespace Lib
{
namespace Hooks
{
//
// we leave this undefined... so if a hook forgets to specialize this, we get a nice
// compiler error. I am kidding about 'nice'.
//
//template < typename Hook >
//struct GetStaticHookId {
//};

//
// unique hook IDs
enum class UniqueHookIDs {
    Initialize,
    LoadAstroImage,
    ColormapsScalar,
    LoadPlugin,

    PreRender,
    LoadImage
};
}
}
}
