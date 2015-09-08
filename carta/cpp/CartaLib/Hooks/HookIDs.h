/**
 * I have not decided on an automated system for asigning unique static IDs to hooks,
 * so for now we'll do it manually. And the weapon of choice is template specialization
 * with enums (ugly, awkward, but easy to spot bugs).
 **/

#pragma once

#include <cstdint>

namespace Carta
{
namespace Lib
{
namespace Hooks
{

// unique hook IDs
enum class UniqueHookIDs {
    Initialize_ID,
    LoadAstroImage_ID,
    HistogramHook_ID,
    ColormapsScalarHook_ID,
    LoadPlugin_ID,
    GetWcsGridRendererHook_ID,
    GetInitialFileList_ID,
    GetImageRenderService_ID,

    /// experimental, soon to be removed:
    PreRender_ID,
    LoadImage_ID

};
}
}
}
