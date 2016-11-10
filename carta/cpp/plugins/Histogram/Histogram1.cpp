#include "Histogram1.h"
#include "CartaLib/Hooks/Histogram.h"
#include "ImageHistogram.h"
#include "ImageRegionGenerator.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/Initialize.h"
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <QDebug>

Histogram1::Histogram1( QObject * parent ) :
    QObject( parent )
{ }

Carta::Lib::Hooks::HistogramResult Histogram1::_computeHistogram( ){
    std::vector < std::pair < double, double > > data;
    QString name;
    QString unitsX = "";
    QString unitsY = "";
    if ( m_histogram ) {
        bool computed = m_histogram->compute();
        if ( computed ) {
            data = m_histogram->getData();
            name = m_histogram->getName();
            unitsX = m_histogram->getUnitsX();
            unitsY = m_histogram->getUnitsY();
        }
        else {
            qDebug() << "Could not generate histogram data";
        }
    }

    Carta::Lib::Hooks::HistogramResult result( name, unitsX, unitsY, data );
    return result;
} // _computeHistogram

std::pair < int, int >
Histogram1::_getChannelBounds( casa::ImageInterface<casa::Float>* casaImage,
        double freqMin, double freqMax, const QString & unitStr ) const{
    std::pair < int, int > bounds( - 1, - 1 );
    if ( ! casaImage ) {
        qWarning() << "Could not get casacore image <float>.";
        return bounds;
    }

    casa::CoordinateSystem cSys = casaImage->coordinates();
    casa::Int specAx = cSys.findCoordinate( casa::Coordinate::SPECTRAL );
    if ( specAx < 0 ) {
        //qWarning() << "Image did not have a spectral coordinate";
        /// \todo Does this mean we can only compute histograms on spectral coordinates!?!?!?!
        return bounds;
    }

    int channelLow = - 1;
    int channelHigh = - 1;
    std::string units = unitStr.toStdString();

    casa::IPosition imgShape = casaImage->shape();
    int maxChannel = imgShape[specAx] - 1;
    casa::SpectralCoordinate specCoord = cSys.spectralCoordinate( specAx );

    //Minimum frequency
    casa::MVFrequency minMV( casa::Quantity( 0, units ) );
    specCoord.toWorld( minMV, 0 );
    casa::Quantity minQuantity = minMV.get( units );
    double lowBound = minQuantity.getValue();

    //Maximum frequency
    casa::MVFrequency maxMV( casa::Quantity( 0, units ) );
    specCoord.toWorld( maxMV, maxChannel );
    casa::Quantity maxQuantity = maxMV.get( units );
    double highBound = maxQuantity.getValue();
    if ( highBound < lowBound ) {
        std::swap( highBound, lowBound);
    }
    double frequencyMin = freqMin;
    double frequencyMax = freqMax;
    if ( frequencyMin < lowBound ) {
        frequencyMin = lowBound;
    }
    if ( frequencyMax > highBound ) {
        frequencyMax = highBound;
    }

    //Lower bound
    casa::Quantity freqQuantity( frequencyMin, units );
    casa::MVFrequency mvFreq( freqQuantity );
    casa::Double pixel = - 1;
    if ( specCoord.toPixel( pixel, mvFreq ) ) {
        channelLow = qRound( pixel );
        if ( channelLow > maxChannel ) {
            channelLow = maxChannel;
        }
    }

    casa::Quantity freqQuantityMax( frequencyMax, units );
    casa::MVFrequency mvFreqMax( freqQuantityMax );
    casa::Double pixelMax = - 1;
    if ( specCoord.toPixel( pixelMax, mvFreqMax ) ) {
        channelHigh = qRound( pixelMax );
        if ( channelHigh > maxChannel ) {
            channelHigh = maxChannel;
        }
    }

    bounds.first = std::min( channelLow, channelHigh );
    bounds.second = std::max( channelLow, channelHigh );
    return bounds;
} // _getChannelBounds

std::pair < double, double >
Histogram1::_getFrequencyBounds( casa::ImageInterface<casa::Float>* casaImage,
        int channelMin, int channelMax, const QString & unitStr ) const{
    std::pair < double, double > bounds( - 1, - 1 );
    if ( ! casaImage ) {
        qWarning() << "Could not get casacore image <float>.";
        return bounds;
    }

    casa::CoordinateSystem cSys = casaImage->coordinates();
    casa::Int specAx = cSys.findCoordinate( casa::Coordinate::SPECTRAL );
    if ( specAx < 0 ) {
        //qWarning() << "Image did not have a spectral coordinate";
        return bounds;
    }

    casa::IPosition imgShape = casaImage->shape();
    int chanMin = std::max( 0, channelMin);
    int chanMax = std::min( int(imgShape[specAx]) - 1, channelMax);

    casa::SpectralCoordinate specCoord = cSys.spectralCoordinate( specAx );
    std::string units = unitStr.toStdString();

    // Lower bound
    double freqLow = - 1;
    casa::MVFrequency mvFreq( casa::Quantity( 0, units));
    if ( specCoord.toWorld( mvFreq, chanMin ) ) {
        freqLow = mvFreq.get( units ).getValue();
    }

    double freqHigh = - 1;
    casa::MVFrequency mvFreqMax( casa::Quantity( 0, units ) );
    if ( specCoord.toWorld( mvFreqMax, chanMax ) ) {
        freqHigh = mvFreqMax.get( units ).getValue();
    }

    bounds.first = std::min( freqLow, freqHigh);
    bounds.second = std::max( freqLow, freqHigh);;

    return bounds;
}



bool
Histogram1::handleHook( BaseHook & hookData )
{
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::HistogramHook > () ) {
        Carta::Lib::Hooks::HistogramHook & hook
            = static_cast < Carta::Lib::Hooks::HistogramHook & > ( hookData );

        const auto & image = hook.paramsPtr-> dataSource;
        if ( !image ) {
            return false;
        }

        casa::ImageInterface<casa::Float> * casaImage = cartaII2casaII_float( image );
        if( ! casaImage) {
            qWarning() << "Histogram plugin: not an image created by casaimageloader...";
            return false;
        }
        if ( !m_histogram ){
            m_histogram.reset(new ImageHistogram < casa::Float >());
        }
        std::shared_ptr<Carta::Lib::Regions::RegionBase> regionBase = hook.paramsPtr->region;
        QString regionId = hook.paramsPtr->regionId;
        casa::ImageRegion* imageRegion = nullptr;
        if ( regionBase ){
        	imageRegion = ImageRegionGenerator::makeRegion( casaImage, regionBase );
        }
        m_histogram->setRegion( imageRegion, regionId  );
        m_histogram-> setBinCount( hook.paramsPtr->binCount );

        double frequencyMin = hook.paramsPtr->minFrequency;
        double frequencyMax = hook.paramsPtr->maxFrequency;
        QString rangeUnits = hook.paramsPtr->rangeUnits;
        int minChannel = - 1;
        int maxChannel = - 1;
        if ( frequencyMin < 0 || frequencyMax < 0 ) {
            casa::CoordinateSystem cSys = casaImage->coordinates();
            casa::Int specAx = cSys.findCoordinate( casa::Coordinate::SPECTRAL );
            if ( specAx >= 0 ) {
                minChannel = hook.paramsPtr->minChannel;
                maxChannel = hook.paramsPtr->maxChannel;
                //m_histogram->setChannelRange( minChannel, maxChannel );

                std::pair<double,double> bounds = _getFrequencyBounds( casaImage, minChannel, maxChannel, rangeUnits );
                frequencyMin = bounds.first;
                frequencyMax = bounds.second;
            }
            m_histogram->setChannelRange( minChannel, maxChannel );
        }
        else {
            std::pair<int,int> bounds = _getChannelBounds( casaImage, frequencyMin, frequencyMax, rangeUnits );
            m_histogram-> setChannelRange( bounds.first, bounds.second );
        }
        m_histogram-> setImage( casaImage->cloneII() );
        double minIntensity = hook.paramsPtr->minIntensity;
        double maxIntensity = hook.paramsPtr->maxIntensity;
        m_histogram->setIntensityRange( minIntensity, maxIntensity );

        hook.result = _computeHistogram();
        hook.result.setFrequencyBounds( frequencyMin, frequencyMax );

        return true;
    }
    qWarning() << "Histogram doesn't know how to handle this hook";
    return false;
} // handleHook

std::vector < HookId >
Histogram1::getInitialHookList(){
    return {
               Carta::Lib::Hooks::Initialize::staticId,
               Carta::Lib::Hooks::HistogramHook::staticId
    };
}

Histogram1::~Histogram1() {

}
