#include "ProfileCASA.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/CasaImageLoader/CCMetaDataInterface.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/ProfileHook.h"
#include "CartaLib/RegionInfo.h"
#include "CartaLib/ProfileInfo.h"
#include "CartaLib/IImage.h"

#include <QDebug>


ProfileCASA::ProfileCASA(QObject *parent) :
    QObject(parent){
}


bool ProfileCASA::handleHook(BaseHook & hookData){
    qDebug() << "ProfileCASA plugin is handling hook #" << hookData.hookId();
    if( hookData.is<Carta::Lib::Hooks::Initialize>()) {
        return true;
    }
    else if( hookData.is<Carta::Lib::Hooks::ProfileHook>()) {
        Carta::Lib::Hooks::ProfileHook & hook
                = static_cast<Carta::Lib::Hooks::ProfileHook &>( hookData);

        std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr = hook.paramsPtr->m_dataSource;
        Carta::Lib::RegionInfo regionInfo = hook.paramsPtr->m_regionInfo;
        Carta::Lib::ProfileInfo profileInfo = hook.paramsPtr->m_profileInfo;
        hook.result = _generateProfile( imagePtr, regionInfo, profileInfo );
        return true;
    }
    qWarning() << "Sorry, ProfileCASA doesn't know how to handle this hook";
    return false;
}

std::vector<HookId> ProfileCASA::getInitialHookList(){
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::ProfileHook::staticId
    };
}

std::vector<double> ProfileCASA::_generateProfile( std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr,
        Carta::Lib::RegionInfo regionInfo, Carta::Lib::ProfileInfo profileInfo ) const {
    std::vector<double> profileData;
    qDebug() << "Profile hook generating data";
    return profileData;
}


ProfileCASA::~ProfileCASA(){

}
