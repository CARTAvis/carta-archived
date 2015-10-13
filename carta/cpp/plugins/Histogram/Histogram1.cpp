#include "Histogram1.h"
#include "CartaLib/Hooks/Histogram.h"
#include "ImageHistogram.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <QDebug>


Histogram1::Histogram1( QObject * parent ) :
    QObject( parent ),
    m_histogram( nullptr)
{
    m_cartaImage = nullptr;
    }


Carta::Lib::Hooks::HistogramResult Histogram1::_computeHistogram(){
    
    vector<std::pair<double,double>> data;
    QString name;
    QString units = "";
    if ( m_histogram ){
        bool computed = m_histogram->compute();
        if ( computed ){
            data = m_histogram->getData();
            name = m_histogram->getName();
            units = m_histogram->getUnits();
        }
        else {
            qDebug() << "Could not generate histogram data";
        }
    }

    Carta::Lib::Hooks::HistogramResult result(name, units, data);
    return result;
}



std::pair<int,int> Histogram1::_getChannelBounds( double freqMin, double freqMax, const QString& unitStr ) const {
    std::pair<int,int> bounds(-1, -1);
    int channelLow = -1;
    int channelHigh = -1;
    std::string units = unitStr.toStdString();
    if ( m_cartaImage ){
        CCImageBase * ptr1 = dynamic_cast<CCImageBase*>( m_cartaImage);
        if ( ptr1 ){
            if (m_cartaImage->pixelType() == Image::PixelType::Real32 ){
                casa::ImageInterface<casa::Float> * casaImage = nullptr;
                #ifndef Q_OS_MAC
                    casaImage = dynamic_cast<casa::ImageInterface<casa::Float> * > (ptr1-> getCasaImage());
                #else
                    casaImage = static_cast<casa::ImageInterface<casa::Float> * > (ptr1-> getCasaImage());
                #endif
                if( casaImage != nullptr)
                {
                    casa::CoordinateSystem cSys = casaImage->coordinates();
                    casa::Int specAx = cSys.findCoordinate( casa::Coordinate::SPECTRAL);
                    if ( specAx >= 0 ){
                        casa::IPosition imgShape = casaImage->shape();
                        int maxChannel = imgShape[specAx] - 1;
                        casa::SpectralCoordinate specCoord = cSys.spectralCoordinate( specAx );

                        //Minimum frequency
                        casa::MVFrequency minMV(casa::Quantity(0, units.c_str()));
                        specCoord.toWorld(minMV, 0 );
                        casa::Quantity minQuantity = minMV.get( units.c_str() );
                        double lowBound = minQuantity.getValue();

                        //Maximum frequency
                        casa::MVFrequency maxMV( casa::Quantity(0, units.c_str()));
                        specCoord.toWorld(maxMV, maxChannel);
                        casa::Quantity maxQuantity = maxMV.get( units.c_str());
                        double highBound = maxQuantity.getValue();
                        if ( highBound < lowBound ){
                            double tmp = lowBound;
                            lowBound = highBound;
                            highBound = tmp;
                        }
                        double frequencyMin = freqMin;
                        double frequencyMax = freqMax;
                        if ( frequencyMin < lowBound ){
                        frequencyMin = lowBound;
                        }
                        if ( frequencyMax > highBound ){
                            frequencyMax = highBound;
                        }

                        //Lower bound
                        casa::Quantity freqQuantity( frequencyMin, units.c_str());
                        casa::MVFrequency mvFreq( freqQuantity );
                        casa::Double pixel = -1;
                        casa::Bool result = specCoord.toPixel( pixel, mvFreq );
                        if ( result ){
                            channelLow = qRound( pixel );
                            if ( channelLow > maxChannel ){
                                channelLow = maxChannel;
                            }
                        }

                        casa::Quantity freqQuantityMax( frequencyMax, units.c_str());
                        casa::MVFrequency mvFreqMax( freqQuantityMax );
                        casa::Double pixelMax = -1;
                        casa::Bool result2 = specCoord.toPixel( pixelMax, mvFreqMax );
                        if ( result2 ){
                            channelHigh = qRound( pixelMax );
                            if ( channelHigh > maxChannel ){
                                channelHigh = maxChannel;
                            }
                        }
                    }
                    else {
                        //qWarning() << "Image did not have a spectral coordinate";
                    }
                }
                else {
                    qWarning() << "casaImage is nullptr, casting failed";
                }
            }
            else {
                qWarning() << "Unsupported pixel type for channel bounds";
            }
        }
        else {
            qWarning() << "Could not get casacore image.";
        }
    }
    else {
        qWarning() << "No image available for channel bounds.";
    }

    if ( channelLow >= 0 && channelHigh >= 0 ){
        if ( channelLow > channelHigh ){
            double tmp = channelLow;
            channelLow = channelHigh;
            channelHigh = tmp;
        }
        bounds.first = channelLow;
        bounds.second = channelHigh;
    }
    return bounds;
}

std::pair<double,double> Histogram1::_getFrequencyBounds( int channelMin, int channelMax, const QString& unitStr ) const {
    std::pair<double,double> bounds(-1, -1);
    double freqLow = -1;
    double freqHigh = -1;
    std::string units = unitStr.toStdString();
    if ( m_cartaImage ){
        CCImageBase * ptr1 = dynamic_cast<CCImageBase*>( m_cartaImage);
        if ( ptr1 ){
            if (m_cartaImage->pixelType() == Image::PixelType::Real32 ){
                casa::ImageInterface<casa::Float> * casaImage = nullptr;
                #ifndef Q_OS_MAC
                    casaImage = dynamic_cast<casa::ImageInterface<casa::Float> * > (ptr1-> getCasaImage());
                #else
                    casaImage = static_cast<casa::ImageInterface<casa::Float> * > (ptr1-> getCasaImage());
                #endif
                if(casaImage != nullptr)
                {    
                    casa::CoordinateSystem cSys = casaImage->coordinates();
                    casa::Int specAx = cSys.findCoordinate( casa::Coordinate::SPECTRAL);
                    if ( specAx >= 0 ){
                        casa::IPosition imgShape = casaImage->shape();
                        int channelLow = 0;
                        int channelHigh = imgShape[specAx] - 1;
                        casa::SpectralCoordinate specCoord = cSys.spectralCoordinate( specAx );
                        int chanMin = channelMin;
                        int chanMax = channelMax;
                        if ( chanMin < channelLow ){
                            chanMin = channelLow;
                        }
                        if ( chanMax > channelHigh ){
                            chanMax = channelHigh;
                        }

                        //Lower bound
                        casa::Quantity freqQuantity( 0, units.c_str());
                        casa::MVFrequency mvFreq( freqQuantity );
                        casa::Double pixel = chanMin;
                        casa::Bool result = specCoord.toWorld( mvFreq, pixel );
                        if ( result ){
                            casa::Quantity freqLowQuantity = mvFreq.get( units.c_str() );
                            freqLow = freqLowQuantity.getValue();
                        }

                        casa::Quantity freqQuantityMax( 0, units.c_str() );
                        casa::MVFrequency mvFreqMax( freqQuantityMax );
                        casa::Double pixelMax = chanMax;
                        casa::Bool result2 = specCoord.toWorld( mvFreqMax, pixelMax );
                        if ( result2 ){
                            casa::Quantity freqHighQuantity = mvFreqMax.get( units.c_str() );
                            freqHigh = freqHighQuantity.getValue();
                        }
                    }
                    else {
                        //qWarning() << "Image did not have a spectral coordinate";
                    }
                }
                else {
                    qWarning() << "casaImage is nullptr, casting failed";
                }
            }
            else {
                qWarning() << "Unsupported pixel type for channel bounds";
            }
        }
        else {
            qWarning() << "Could not get casacore image.";
        }
    }
    else {
        qWarning() << "No image available for channel bounds.";
    }
    if ( freqHigh >= 0 && freqLow >= 0 ){
        if ( freqLow > freqHigh ){
            double tmp = freqLow;
            freqLow = freqHigh;
            freqHigh = tmp;
        }
        bounds.first = freqLow;
        bounds.second = freqHigh;
    }
    return bounds;
}

bool Histogram1::handleHook( BaseHook & hookData ){
    if ( hookData.is < Initialize > () ) {
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::HistogramHook > () ) {
        bool histSuccess = false;
        Carta::Lib::Hooks::HistogramHook & hook
        = static_cast < Carta::Lib::Hooks::HistogramHook & > ( hookData );

        std::vector<std::shared_ptr<Image::ImageInterface>> images = hook.paramsPtr->dataSource;
        casa::ImageInterface<casa::Float> * casaImage = nullptr;
        if ( images.size() > 0 ){
            CCImageBase * ptr1 = dynamic_cast<CCImageBase*>( images.front().get() );
            if( ! ptr1) {
                throw "not an image created by casaimageloader...";
            }
            if (ptr1->pixelType() == Image::PixelType::Real32 ){
                #ifndef Q_OS_MAC
                    casaImage = dynamic_cast<casa::ImageInterface<casa::Float> * > (ptr1-> getCasaImage());
                #else
                    casaImage = static_cast<casa::ImageInterface<casa::Float> * > (ptr1-> getCasaImage());
                #endif
            }
            //Reset the histogram if the image has changed.
            if ( m_cartaImage != ptr1 ){
                m_cartaImage = images.front().get();

                ImageHistogram<casa::Float>* hist = new ImageHistogram<casa::Float>();
                m_histogram.reset( hist );
                if( casaImage ){
                    hist->setImage( casaImage );
                }
            }
        }

        if ( m_histogram ){

            int count = hook.paramsPtr->binCount;
            m_histogram->setBinCount( count );

            double frequencyMin = hook.paramsPtr->minFrequency;
            double frequencyMax = hook.paramsPtr->maxFrequency;
            QString rangeUnits = hook.paramsPtr->rangeUnits;
            int minChannel = -1;
            int maxChannel = -1;
            if ( frequencyMin < 0 || frequencyMax < 0  ){
                if ( casaImage != nullptr ){
                    casa::CoordinateSystem cSys = casaImage->coordinates();
                    casa::Int specAx = cSys.findCoordinate( casa::Coordinate::SPECTRAL);
                    if ( specAx >= 0 ){
                        minChannel = hook.paramsPtr->minChannel;
                        maxChannel = hook.paramsPtr->maxChannel;
                        m_histogram->setChannelRange(minChannel, maxChannel);
                    }
                }
                else {
                    m_histogram->setChannelRange( -1, -1 );
                }
                std::pair<double,double> bounds = _getFrequencyBounds( minChannel, maxChannel, rangeUnits);
                frequencyMin = bounds.first;
                frequencyMax = bounds.second;
            }
            else {
                std::pair<int,int> bounds = _getChannelBounds( frequencyMin, frequencyMax, rangeUnits );
                minChannel = bounds.first;
                maxChannel = bounds.second;
                m_histogram->setChannelRange( minChannel, maxChannel);
            }
            double minIntensity = hook.paramsPtr->minIntensity;
            double maxIntensity = hook.paramsPtr->maxIntensity;
            m_histogram->setIntensityRange(minIntensity, maxIntensity);

            hook.result = _computeHistogram();
            hook.result.setFrequencyBounds( frequencyMin, frequencyMax );
            histSuccess = true;
        }
        else {
            hook.result = _computeHistogram();

            histSuccess = false;
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


