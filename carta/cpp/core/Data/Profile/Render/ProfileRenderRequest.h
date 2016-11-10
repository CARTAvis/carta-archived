/**
 * Request to render a profile of a cube.
 **/

#pragma once

#include <memory>
#include "CartaLib/ProfileInfo.h"

namespace Carta{
namespace Data{

class Layer;
class Region;

class ProfileRenderRequest {

public:

	/**
	 * Constructor.
	 */
	explicit ProfileRenderRequest( std::shared_ptr<Layer> layer, std::shared_ptr<Region> region,
			Carta::Lib::ProfileInfo profileInfo, bool createNew );

	/**
	 * Returns the layer in the stack that will be profiled.
	 * @return - the stack layer to be profiled.
	 */
	std::shared_ptr<Layer> getLayer() const;

	/**
	 * Returns the region to be profiled.
	 * @return - the region to be profiled.
	 */
	std::shared_ptr<Region> getRegion() const;

	/**
	 * Return information about how the profile will be computed.
	 * @return - information about how to compute the profile.
	 */
	Carta::Lib::ProfileInfo getProfileInfo() const;

	/**
	 * Returns whether or not a new profile should be created.
	 * @return - whether or not a new profile should be created.
	 */
	bool isCreateNew() const;

	/**
	 * Returns whether or not the other request is equal to this one.
	 * @param other - a potentially different request to render a profile.
	 * @return - true, if the other request is the same as this one; false, otherwise.
	 */
	bool operator==( const ProfileRenderRequest& other );

	/**
	 * Destructor.
	 */
	~ProfileRenderRequest();

private:
	QString _getId() const;
	bool m_createNew;
	Carta::Lib::ProfileInfo m_profileInfo;
	std::shared_ptr<Layer> m_layer;
	std::shared_ptr<Region> m_region;
};
}
}



