#include "RegionDs9.h"
#include "ContextDs9.h"
#include "ParserDs9.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/LoadRegion.h"
#include "CartaLib/IImage.h"

#include <QDebug>


RegionDs9::RegionDs9(QObject *parent) :
    QObject(parent){
}


bool RegionDs9::handleHook(BaseHook & hookData){
    qDebug() << "RegionDs9 plugin is handling hook #" << hookData.hookId();
    bool hookHandled = false;
    if ( hookData.is<Carta::Lib::Hooks::Initialize>()) {
        hookHandled = true;
    }
    else if ( hookData.is<Carta::Lib::Hooks::LoadRegion>()) {
        Carta::Lib::Hooks::LoadRegion & hook
        = static_cast<Carta::Lib::Hooks::LoadRegion &>( hookData);
        QString fileName = hook.paramsPtr->fileName;
        if ( fileName.length() > 0 ){
            ContextDs9 context;
            ParserDs9 parser;
            bool result = parser.parse_file( context, fileName.toStdString());
            std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr = hook.paramsPtr->image;
            hook.result = context.getRegions();
            qDebug() << "Ds9 read region file result="<<result;;
            hookHandled = true;
        }
    }
    return hookHandled;
}


std::vector<HookId> RegionDs9::getInitialHookList(){
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::LoadRegion::staticId
    };
}


RegionDs9::~RegionDs9(){

}
