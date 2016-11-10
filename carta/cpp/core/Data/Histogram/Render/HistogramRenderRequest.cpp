#include "Data/Image/Layer.h"
#include "Data/Region/Region.h"
#include "Data/Histogram/Render/HistogramRenderRequest.h"

namespace Carta {
namespace Data {

HistogramRenderRequest::HistogramRenderRequest( std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
        int binCount, int minChannel, int maxChannel, double minFrequency, double maxFrequency,
        const QString& rangeUnits, double minIntensity, double maxIntensity,
        const QString& fileName, std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
		const QString& regionId ) :
		m_image (nullptr),
		m_region (nullptr ){
	m_image = dataSource;
	m_region = region;
	m_binCount = binCount;
	m_minChannel = minChannel;
	m_maxChannel = maxChannel;
	m_minFrequency = minFrequency;
	m_maxFrequency = maxFrequency;
	m_minIntensity = minIntensity;
	m_maxIntensity = maxIntensity;
	m_rangeUnits = rangeUnits;
	m_fileName = fileName;
	m_regionId = regionId;
}

int HistogramRenderRequest::getBinCount() const {
	return m_binCount;
}

int HistogramRenderRequest::getChannelMax() const {
	return m_maxChannel;
}

int HistogramRenderRequest::getChannelMin() const {
	return m_minChannel;
}

QString HistogramRenderRequest::getFileName() const {
	return m_fileName;
}

int HistogramRenderRequest::getFrequencyMax() const {
	return m_maxFrequency;
}

int HistogramRenderRequest::getFrequencyMin() const {
	return m_minFrequency;
}

QString HistogramRenderRequest::getId() const {
	QString id = m_fileName;
	id = id + m_regionId;
	id = id +  QString::number(m_binCount);
	id = id + "[" + QString::number(m_minChannel) +"," + QString::number(m_maxChannel)+"]";
	id = id + "[" + QString::number(m_minFrequency) + "," + QString::number( m_maxFrequency)+"]";
	id = id + "[" + QString::number(m_minIntensity) + "," + QString::number( m_maxIntensity )+"]";
	id = id + m_rangeUnits;
	return id;
}

std::shared_ptr<Carta::Lib::Image::ImageInterface> HistogramRenderRequest::getImage() const {
	return m_image;
}

int HistogramRenderRequest::getIntensityMax() const {
	return m_maxIntensity;
}


	int HistogramRenderRequest::getIntensityMin() const {
		return m_minIntensity;
	}


QString HistogramRenderRequest::getRangeUnits() const {
	return m_rangeUnits;
}

std::shared_ptr<Carta::Lib::Regions::RegionBase> HistogramRenderRequest::getRegion() const {
	return m_region;
}

QString HistogramRenderRequest::getRegionId() const {
	return m_regionId;
}


bool HistogramRenderRequest::operator==( const HistogramRenderRequest& other ){
	bool equalRequests = false;
	if ( other.getId() == getId() ){
		equalRequests = true;
	}
	return equalRequests;
}


HistogramRenderRequest::~HistogramRenderRequest(){

}
}
}

