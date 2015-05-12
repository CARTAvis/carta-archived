#include "WcsPlotterPlugin.h"
#include "AstWcsGridRenderService.h"
#include "CartaLib/Hooks/GetWcsGridRenderer.h"
//#include "AstGridPlotter.h"
//#include "CartaLib/Hooks/GetWcsGridRenderer.h"
//#include "../CasaImageLoader/CCImage.h"
//#include "AstWcsGridRenderService.h"
//#include "FitsHeaderExtractor.h"
//#include <QDebug>
//#include <QPainter>
//#include <QTime>
//#include <casa/Exceptions/Error.h>
//#include <images/Images/FITSImage.h>
//#include <images/Images/MIRIADImage.h>
//#include <images/Images/HDF5Image.h>
//#include <images/Images/ImageExpr.h>
//#include <images/Images/ImageExprParse.h>
//#include <images/Images/ImageOpener.h>
//#include <casa/Arrays/ArrayMath.h>
//#include <casa/Quanta.h>
//#include <memory>
//#include <algorithm>
//#include <cstdint>
//#include <QString>

namespace WcsPlotterPluginNS
{
typedef Carta::Lib::Hooks::GetWcsGridRendererHook GetWcsGridRendererHook;

WcsPlotterPlugin::WcsPlotterPlugin( QObject * parent ) :
    QObject( parent )
{ }

bool
WcsPlotterPlugin::handleHook( BaseHook & hookData )
{
    qDebug() << "WcsPlotter plugin is handling hook #" << hookData.hookId();
    if ( hookData.is < GetWcsGridRendererHook > () ) {
        auto & hook = static_cast < GetWcsGridRendererHook & > ( hookData );
        hook.result.reset( new AstWcsGridRenderService() );
        return true;
    }

    qCritical() << "Sorrry, WcsPlotterPlugin doesnt' know how to handle this hook" <<
    hookData.hookId();
    return false;
} // handleHook

std::vector < HookId >
WcsPlotterPlugin::getInitialHookList()
{
    return {
               GetWcsGridRendererHook::staticId
    };
}
}
