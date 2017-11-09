#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/PixelToPercentileHook.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/PercentileManku99/PercentileManku99Plugin.h"
#include "plugins/PercentileManku99/PercentileManku99.h"
#include <QDebug>


bool PercentileManku99Plugin::handleHook( BaseHook & hookData ){
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
        return true;
    }
    
    else if ( hookData.is < Carta::Lib::Hooks::PixelToPercentileHook > () ) {
        Carta::Lib::Hooks::PixelToPercentileHook & hook
            = static_cast <Carta::Lib::Hooks::PixelToPercentileHook & > ( hookData );
        
        // TODO this is currently unused, but we should use it to pick a plugin (maybe)
        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = hook.paramsPtr->m_image;
                
        hook.result = PercentileManku99(m_numBuffers, m_bufferCapacity, m_sampleAfter);
        
        return true;
    }
    
    qWarning() << "Percentile histogram plugin doesn't know how to handle this hook";
    return false;
} // handleHook


std::vector < HookId > PercentileManku99Plugin::getInitialHookList() {
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::PixelToPercentileHook::staticId
    };
}

void PercentileManku99Plugin::initialize( const IPlugin::InitInfo & initInfo )
{
    qDebug() << "PercentileManku99Plugin initializing...";
    QJsonDocument doc( initInfo.json );
    qDebug() << doc.toJson();
    
    try {
        m_numBuffers = initInfo.json.value( "numBuffers").toInt();
    } catch (const QString& error) {
        qCritical() << "No valid numBuffers value specified for percentile histogram plugin.";
    }
        
    if( m_numBuffers <= 0) {
        qCritical() << "No valid numBuffers value specified for percentile histogram plugin. Must be a positive integer.";
    }
    
    try {
        m_bufferCapacity = initInfo.json.value( "bufferCapacity").toInt();
    } catch (const QString& error) {
        qCritical() << "No valid bufferCapacity value specified for percentile histogram plugin.";
    }
        
    if( m_bufferCapacity <= 0) {
        qCritical() << "No valid bufferCapacity value specified for percentile histogram plugin. Must be a positive integer.";
    }
    
    try {
        m_sampleAfter = initInfo.json.value( "sampleAfter").toInt();
    } catch (const QString& error) {
        qCritical() << "No valid sampleAfter value specified for percentile histogram plugin.";
    }
        
    if( m_sampleAfter <= 0) {
        qCritical() << "No valid sampleAfter value specified for percentile histogram plugin. Must be a positive integer.";
    }
}

