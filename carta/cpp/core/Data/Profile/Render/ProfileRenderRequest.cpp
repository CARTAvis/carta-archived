#include "Data/Image/Layer.h"
#include "Data/Region/Region.h"
#include "Data/Profile/Render/ProfileRenderRequest.h"

namespace Carta {
namespace Data {

ProfileRenderRequest::ProfileRenderRequest( std::shared_ptr<Layer> layer, std::shared_ptr<Region> region,
		Carta::Lib::ProfileInfo profileInfo, bool createNew) :
		m_layer (nullptr),
		m_region (nullptr ){
	m_layer = layer;
	m_region = region;
	m_profileInfo = profileInfo;
	m_createNew = createNew;
}

std::shared_ptr<Layer> ProfileRenderRequest::getLayer() const {
	return m_layer;
}

std::shared_ptr<Region> ProfileRenderRequest::getRegion() const {
	return m_region;
}

Carta::Lib::ProfileInfo ProfileRenderRequest::getProfileInfo() const {
	return m_profileInfo;
}

QString ProfileRenderRequest::_getId() const {
	 QString id;
	 if ( m_layer ){
		 id = m_layer->_getLayerId();
	 }
	 if ( m_region ){
		 id = id + m_region->getRegionName();
	 }
	 return id;
}

bool ProfileRenderRequest::isCreateNew() const {
	return m_createNew;
}

bool ProfileRenderRequest::operator==( const ProfileRenderRequest& other ){
	bool equalRequests = false;
	if ( other._getId() == _getId() ){
		if ( other.m_profileInfo == m_profileInfo ){
			equalRequests = true;
		}
	}
	return equalRequests;
}

ProfileRenderRequest::~ProfileRenderRequest(){

}
}
}

