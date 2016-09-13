#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/ConversionSpectralHook.h"
#include "CartaLib/IImage.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/CasaImageLoader/CCMetaDataInterface.h"
#include "plugins/ConversionSpectral/Converter.h"
#include "plugins/ConversionSpectral/SpectralConversionPlugin.h"

#include <QDebug>


SpectralConversionPlugin::SpectralConversionPlugin( QObject * parent ) :
    QObject( parent )
{ }


bool
SpectralConversionPlugin::handleHook( BaseHook & hookData ){
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::ConversionSpectralHook > () ) {
        Carta::Lib::Hooks::ConversionSpectralHook & hook
            = static_cast < Carta::Lib::Hooks::ConversionSpectralHook & > ( hookData );
        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = hook.paramsPtr->m_dataSource;
        if ( image ){
            QString newUnits = hook.paramsPtr->m_newUnit;
            QString oldUnits = hook.paramsPtr->m_oldUnit;
            if ( oldUnits.isEmpty() || oldUnits.trimmed().length() == 0 ){
                oldUnits = "pixel";
            }
            Converter* converter = Converter::getConverter( oldUnits, newUnits );
            if ( converter ){
                CCImageBase * base = dynamic_cast<CCImageBase*>( image.get() );
                if ( base ){
                    Carta::Lib::Image::MetaDataInterface::SharedPtr metaPtr = base->metaData();
                    CCMetaDataInterface* metaData = dynamic_cast<CCMetaDataInterface*>(metaPtr.get());
                    if ( metaData ){
                        std::shared_ptr<casa::CoordinateSystem> cs = metaData->getCoordinateSystem();
                        int spectralIndex = cs->findCoordinate(casa::Coordinate::SPECTRAL,  -1);
                        if ( spectralIndex >= 0 ){
                            casa::SpectralCoordinate sc = cs->spectralCoordinate( spectralIndex );
                            std::vector<double> inputValues = hook.paramsPtr->m_inputList;
                            int dataCount = inputValues.size();
                            casa::Vector<double> inputs( dataCount );
                            for ( int i = 0; i < dataCount; i++ ){
                                inputs[i] = inputValues[i];
                            }
                            std::vector<double> resultValues;
                            if ( !newUnits.isEmpty() ){
                                casa::Vector<double> outputs = converter->convert( inputs, sc );
                                resultValues = outputs.tovector();
                            }
                            else {
                                for ( int i = 0; i < dataCount; i++ ){
                                    double converted = inputs[i];
                                    if ( oldUnits != "pixel"){
                                        converted = converter->toPixel( inputs[i], sc );
                                    }
                                    resultValues.push_back( converted );
                                }
                            }
                            hook.result = resultValues;
                        }
                        else {
                            //qDebug() << "Not converting spectral units, no spectral coordinate";
                        }
                    }
                }
                delete converter;
            }
        }

        return true;
    }
    qWarning() << "Spectral conversion doesn't know how to handle this hook";
    return false;
} // handleHook

std::vector < HookId >
SpectralConversionPlugin::getInitialHookList(){
    return {
               Carta::Lib::Hooks::Initialize::staticId,
               Carta::Lib::Hooks::ConversionSpectralHook::staticId
    };
}

SpectralConversionPlugin::~SpectralConversionPlugin() {

}
