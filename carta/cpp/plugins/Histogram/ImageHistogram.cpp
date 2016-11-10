#include <assert.h>
#include <QDebug>
#include <QtCore/qmath.h>
#include "CartaLib/IImage.h"
#include "ImageHistogram.h"
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/casa/version.h>
#ifdef CASACORE_VERSION
#include <casacore/lattices/LatticeMath/LatticeHistograms.h>

#else
#include <casacore/lattices/Lattices/LatticeHistograms.h>
#endif
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "plugins/CasaImageLoader/CasaImageLoader.h"
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/BasicSL/String.h>


template <class T>
ImageHistogram<T>::ImageHistogram( ):
	m_histogramMaker(NULL), m_region(NULL),
	ALL_CHANNELS(-1),
	ALL_INTENSITIES( -1),
	m_image(nullptr),
	m_channelMin( ALL_CHANNELS ),
	m_channelMax( ALL_CHANNELS ),
	m_intensityMin( ALL_INTENSITIES ),
	m_intensityMax( ALL_INTENSITIES),
	m_binCount( 25 ){
}

template <class T>
void ImageHistogram<T>::setChannelRangeDefault(){
	m_channelMin = ALL_CHANNELS;
	m_channelMax = ALL_CHANNELS;
}

template <class T>
void ImageHistogram<T>::setChannelRange( int minChannel, int maxChannel ){
    int oldMinChannel = m_channelMin;
    int oldMaxChannel = m_channelMax;
    if ( minChannel < 0 || maxChannel < 0 ){
        m_channelMin = ALL_CHANNELS;
        m_channelMax = ALL_CHANNELS;
    }
    else {
        m_channelMin = minChannel;
        m_channelMax = maxChannel;
    }
    if ( m_channelMin != oldMinChannel || m_channelMax != oldMaxChannel ){
        _reset();
    }
}

template <class T>
void ImageHistogram<T>::setBinCount( int count ){
	m_binCount = count;
}

template <class T>
void ImageHistogram<T>::setIntensityRangeDefault(){
	m_intensityMin = ALL_INTENSITIES;
	m_intensityMax = ALL_INTENSITIES;
}

template <class T>

void ImageHistogram<T>::setIntensityRange( double minimumIntensity, double maximumIntensity ){

	m_intensityMin = minimumIntensity;
	m_intensityMax = maximumIntensity;
}

template <class T>
bool ImageHistogram<T>::compute( ){
	bool success = true;
	if ( m_histogramMaker != NULL ){

		//Set the number of bins.
		m_histogramMaker->setNBins( m_binCount );

		//Set the intensity range.
		casa::Vector<T> includeRange;
		if ( m_intensityMin != ALL_INTENSITIES && m_intensityMax != ALL_INTENSITIES ){
			includeRange.resize(2);
			includeRange[0] = m_intensityMin;
			includeRange[1] = m_intensityMax;
		}
		m_histogramMaker->setIncludeRange( includeRange );
		try {

			//Calculate the histogram
			casa::Array<T> values;
			casa::Array<T> counts;
			success = m_histogramMaker->getHistograms( values, counts );
			if ( success ){
				//Store the data
				m_xValues.resize( values.size());
				m_yValues.resize( counts.size());
				values.tovector( m_xValues );
				counts.tovector( m_yValues );
			}
		}
		catch( casa::AipsError& error ){
			success = false;
			qDebug() << "Exception: "<<error.what();
		}
	}
	else {
		success = false;
	}
	return success;
}

template <class T>
void ImageHistogram<T>::_filterByChannels( const casa::ImageInterface<T>* image ){
	if ( m_channelMin != ALL_CHANNELS && m_channelMax != ALL_CHANNELS ){
		//Create a slicer from the image
		casa::CoordinateSystem cSys = image->coordinates();
		if ( cSys.hasSpectralAxis() ){
			//We use the preset spectral coordinate, if it
			//exists because images can be rotated when they
			//come into the viewer.  CoordinateSystem does not
			//take rotation into account.
			int spectralIndex = cSys.spectralAxisNumber();
			if ( spectralIndex >= 0 ){
                casa::IPosition imShape = image->shape();

                int shapeCount = imShape.nelements();
                casa::IPosition startPos( shapeCount, 0);
                casa::IPosition endPos(imShape - 1);
                casa::IPosition stride( shapeCount, 1);

                int endIndex = m_channelMax;
                if ( m_channelMax >= imShape(spectralIndex) && m_channelMin < imShape(spectralIndex)){
                    endIndex = imShape(spectralIndex) - 1;
                }

                startPos[spectralIndex] = m_channelMin;
                endPos[spectralIndex] = endIndex;

                casa::Slicer channelSlicer( startPos, endPos, stride, casa::Slicer::endIsLast );
                casa::ImageInterface<T>* img = new casa::SubImage<T>( *(image), channelSlicer );
                delete m_histogramMaker;
                m_histogramMaker = new casa::LatticeHistograms<casa::Float>( *img );
			}
		}
	}
	else {
	    delete m_histogramMaker;
	    m_histogramMaker = new casa::LatticeHistograms<casa::Float>( *image );
	}
}

template <class T>
void ImageHistogram<T>::setImage( const casa::ImageInterface<T>*  val ){
    if ( val != nullptr ){
        if ( m_image == nullptr || m_image->name(true) != val->name(true) ){
            m_image = val;
            _reset();
        }
	}
}

template <class T>
void ImageHistogram<T>::setRegion( casa::ImageRegion* region, const QString& id ){
	if ( m_region == nullptr || m_regionId != id ){
		delete m_region;
		m_region = region;
		m_regionId = id;
	}
}

template <class T>
bool ImageHistogram<T>::_reset(){
	bool success = true;
	if ( m_image != nullptr ){
	    if ( !m_histogramMaker ){
	        m_histogramMaker = new casa::LatticeHistograms<T>( *(m_image) );
	    }
		try {
			if ( m_region == NULL ){
				//Make the histogram based on the image
				_filterByChannels( m_image );
			}
			else {
				//Make the histogram based on the region
				casa::SubImage<T>* subImage = new casa::SubImage<T>( *m_image, *m_region );
				 _filterByChannels( subImage );
				//Filter will make a new image based on this one so we can delete this sub
				//image once filter is done.
				delete subImage;
			}
			success = compute();
		}
		catch( casa::AipsError& error ){
			success = false;
			qDebug() << "Error making histogram: "<<error.getMesg().c_str();
		}
	}
	else {
		success = false;
	}
	return success;
}

template <class T>
void ImageHistogram<T>::defineLine( int index, QVector<double>& xVals,
		QVector<double>& yVals, bool useLogY ) const{
	assert( xVals.size() == 2 );
	assert( yVals.size() == 2 );
	int dataCount = m_xValues.size();
	assert( index >= 0 && index < dataCount);
	xVals[0] = m_xValues[index];
	xVals[1] = m_xValues[index];
	yVals[0] = computeYValue( 0, useLogY );
	yVals[1] = computeYValue( m_yValues[index], useLogY );
}

template <class T>
void ImageHistogram<T>::defineStepHorizontal( int index, QVector<double>& xVals,
		QVector<double>& yVals, bool useLogY ) const{
	assert( xVals.size() == 2 );
	assert( yVals.size() == 2 );
	int pointCount = m_xValues.size();
	assert( index >= 0 && index < pointCount);
	if ( index > 0 ){
		xVals[0] = (m_xValues[index] + m_xValues[index-1])/2;
	}
	else {
		xVals[0] = m_xValues[0];
	}
	if ( index < pointCount - 1){
		xVals[1] = ( m_xValues[index] + m_xValues[index+1] ) / 2;
	}
	else {
		xVals[1] = m_xValues[index];
	}
	yVals[0] = computeYValue(m_yValues[index], useLogY);
	yVals[1] = yVals[0];
}

template <class T>
void ImageHistogram<T>::defineStepVertical( int index, QVector<double>& xVals,
		QVector<double>& yVals, bool useLogY ) const {
	assert( xVals.size() == 2 );
	assert( yVals.size() == 2 );
	int count = m_xValues.size();
	assert( index >= 0 && index < count );
	if ( index > 0 ){
		xVals[0] = (m_xValues[index] + m_xValues[index-1])/2;
	}
	else {
		xVals[0] = m_xValues[0];
	}
	xVals[1] = xVals[0];

	if ( index > 0 ){
		yVals[0] = computeYValue(m_yValues[index-1], useLogY );
	}
	else {
		yVals[0] = computeYValue( 0, useLogY);
	}
	yVals[1] = computeYValue(m_yValues[index], useLogY );
}

template <class T>
double ImageHistogram<T>::computeYValue( double value, bool useLog ){
	double resultValue = value;
	//Log of 0 becomes infinity, and some of the counts are 0.
	if ( useLog ){
		if (value < 1 ){
			resultValue = 1;
		}
	}
	return resultValue;
}

template <class T>
std::pair<float,float> ImageHistogram<T>::getMinMaxBinCount() const {
	std::pair<float,float> minMaxBinCount;
	int valueCount = m_yValues.size();
	for ( int i = 0; i < valueCount; i++ ){
		if ( m_yValues[i]>minMaxBinCount.second){
			minMaxBinCount.second = m_yValues[i];
		}
		if ( m_yValues[i] < minMaxBinCount.first){
			minMaxBinCount.first = m_yValues[i];
		}
	}
	return minMaxBinCount;
}

template <class T>
int ImageHistogram<T>::getDataCount() const {
	return m_xValues.size();
}

template <class T>
vector<T> ImageHistogram<T>::getXValues() const {
	return m_xValues;
}

template <class T>
vector<T> ImageHistogram<T>::getYValues() const {
	return m_yValues;
}

template <class T>
std::vector< std::pair<double,double> > ImageHistogram<T>::getData() const {
    int dataCount = getDataCount();
    std::vector< std::pair<double,double> > data( dataCount );
    for ( int i = 0; i < dataCount; i++ ){
        data[i] = std::pair<double,double>(m_xValues[i],m_yValues[i]);
    }
    return data;
}

template <class T>
QString ImageHistogram<T>::getName() const {
    casa::String strname = ImageHistogram::m_image->name(true);
    QString qname(strname.c_str());
    qname = qname + m_regionId;

    return qname;
}

template<class T>
QString ImageHistogram<T>::getUnitsX() const {
    return "pixels";
}

template<class T>
QString ImageHistogram<T>::getUnitsY() const {
    const casa::Unit pixelUnit = ImageHistogram::m_image->units();
    return pixelUnit.getName().c_str();
}

template <class T>
pair<float,float> ImageHistogram<T>::getDataRange() const {
    int count = m_xValues.size();
    pair<float,float> range;
    if ( count >= 1 ){
        double minValue = m_xValues[0];
        double maxValue = m_xValues[0];
        for ( int i = 1; i < count; i++ ){
            if ( m_xValues[i] < minValue ){
                minValue = m_xValues[i];
            }
            else if ( m_xValues[i] > maxValue ){
                maxValue = m_xValues[i];
            }
        }
        range.first = minValue;
        range.second = maxValue;
    }
    return range;
}



template <class T>
void ImageHistogram<T>::toAscii( QTextStream& out ) const {
	const QString LINE_END( "\n");
	QString centerStr( "#Bin Center(");
	casa::Unit unit = ImageHistogram::m_image->units();
	QString unitStr( unit.getName().c_str());
	centerStr.append( unitStr + ")");
	out << centerStr << "Count";
	out << LINE_END;
	out.flush();
	int count = m_xValues.size();
	for ( int i = 0; i < count; i++ ){
		out << QString::number(m_xValues[i]) << QString::number( m_yValues[i]);
		out << LINE_END;
		out.flush();
	}
}

template <class T>
ImageHistogram<T>::~ImageHistogram() {
	delete m_region;
}

template class ImageHistogram<float>;

