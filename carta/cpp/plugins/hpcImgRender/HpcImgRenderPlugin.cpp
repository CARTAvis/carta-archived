#include "HpcImgRenderPlugin.h"
#include "MyImageRenderService.h"
#include "CartaLib/Hooks/GetImageRenderService.h"
#include "CartaLib/Hooks/Initialize.h"
#include <QDebug>
#include <memory>
#include <algorithm>
#include <vector>

HpcImageRenderServicePlugin::HpcImageRenderServicePlugin( QObject * parent ) :
    QObject( parent )
{ }

bool
HpcImageRenderServicePlugin::handleHook( BaseHook & hookData )
{
    qDebug() << "HpcImageRenderServicePlugin is handling hook #" << hookData.hookId();
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
        // insert initialization stuff here that depends on core running
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::GetImageRenderService > () ) {
        Carta::Lib::Hooks::GetImageRenderService & hook =
            static_cast < Carta::Lib::Hooks::GetImageRenderService & > ( hookData );
        try {
            hook.result = std::make_shared < MyImageRenderService > ();
        }
        catch ( ... ) {
            return false;
        }

        return true;
    }

    qWarning() << "HpcImageRenderServicePlugin: Sorrry, dont' know how to handle this hook";
    return false;
} // handleHook

std::vector < HookId >
HpcImageRenderServicePlugin::getInitialHookList()
{
    return {
               Carta::Lib::Hooks::Initialize::staticId,
               Carta::Lib::Hooks::GetImageRenderService::staticId
    };
}

void
HpcImageRenderServicePlugin::initialize( const IPlugin::InitInfo & initInfo )
{
    // this is a chance to see what needs/needs not be enabled based on initInfo

    // for example, based on initInfo.json, you may are may not decide to handle
    // some hooks
    Q_UNUSED( initInfo );
}

