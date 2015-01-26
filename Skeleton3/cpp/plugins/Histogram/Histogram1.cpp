#include "Histogram1.h"
#include "CartaLib/Hooks/Histogram.h"
#include "ImageHistogram.h"
#include "Globals.h"
#include "PluginManager.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include <QDebug>


Histogram1::Histogram1( QObject * parent ) :
    QObject( parent ),
    m_histogram( nullptr)
{
    }


vector<std::pair<double,double> > Histogram1::_computeHistogram(){
    vector<std::pair<double,double>> result;
    if ( m_histogram ){
        bool computed = m_histogram->compute();
        if ( computed ){
            result = m_histogram->getData();
        }
        else {
            qDebug() << "Could not generate histogram data";
        }
    }
    else {
        qDebug() << "Histogram not initialized";
    }
    return result;
}

bool Histogram1::handleHook( BaseHook & hookData ){
    if ( hookData.is < Initialize > () ) {
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::HistogramHook > () ) {
        bool histSuccess = false;
        Carta::Lib::Hooks::HistogramHook & hook
                            = static_cast < Carta::Lib::Hooks::HistogramHook & > ( hookData );
        // auto fname = hook.paramsPtr->fileName;
        // m_cartaImage = Globals::instance()-> pluginManager()
        //                                 -> prepare <Carta::Lib::Hooks::LoadAstroImage>( fname )
        //                                 .first().val();


        //CCImageBase * ptr1 = dynamic_cast<CCImageBase*>( m_cartaImage.get());
        //if( ! ptr1) {
        //    throw "not an image created by casaimageloader...";
        //}

        //if (m_cartaImage->pixelType() == Image::PixelType::Real32 ){
            // casa::ImageInterface<casa::Float> * casaImage =
            //         dynamic_cast<casa::ImageInterface<casa::Float> * > (ptr1-> getCasaImage());
            // if (! casaImage) throw "Not a CasaImage.";

            std::vector<std::shared_ptr<Image::ImageInterface>> images = hook.paramsPtr->dataSource;
            m_cartaImage = images.front();

            CCImageBase * ptr1 = dynamic_cast<CCImageBase*>( m_cartaImage.get());
            if( ! ptr1) {
                throw "not an image created by casaimageloader...";
            }

            if (m_cartaImage->pixelType() == Image::PixelType::Real32 ){

                casa::ImageInterface<casa::Float> * casaImage =
                    dynamic_cast<casa::ImageInterface<casa::Float> * > (ptr1-> getCasaImage());


                ImageHistogram<casa::Float>* hist = new ImageHistogram<casa::Float>();
                m_histogram.reset( hist );
                hist->setImage( casaImage );
                
                
                // hist->setImage( casaPtr );

                //casa::ImageRegion * region;
                //hist->setRegion(region);

                auto count = hook.paramsPtr->binCount;
                m_histogram->setBinCount( count );
                // hist->setBinCount( count );


                auto minChannel = hook.paramsPtr->minChannel;
                auto maxChannel = hook.paramsPtr->maxChannel;
                auto spectralIndex = hook.paramsPtr->spectralIndex;
                m_histogram->setChannelRange(minChannel, maxChannel, spectralIndex);
                // hist->setChannelRange(minChannel, maxChannel, spectralIndex);

                auto minIntensity = hook.paramsPtr->minIntensity;
                auto maxIntensity = hook.paramsPtr->maxIntensity;
                m_histogram->setIntensityRange(minIntensity, maxIntensity);
                // hist->setIntensityRange(minIntensity, maxIntensity);

            hook.result = _computeHistogram();
            if ( hook.result.size() > 0 ){
                histSuccess = true;
            }
        }
        else {
            throw "Unimplemented data type";
        }
        return histSuccess;
    }
    qWarning() << "Sorry, histogram doesn't know how to handle this hook";
    return false;
}

std::vector < HookId > Histogram1::getInitialHookList(){
    return {
               Initialize::staticId,
               Carta::Lib::Hooks::HistogramHook::staticId
    };
}


Histogram1::~Histogram1(){

}


