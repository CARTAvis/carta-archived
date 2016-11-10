/**
 * Initiates the work of computing the Profile in a separate process.
 **/

#pragma once

#include <memory>

#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/Hooks/ProfileResult.h"
#include "CartaLib/ProfileInfo.h"

namespace Carta {
namespace Lib {


namespace Image {
class ImageInterface;
}
}
}

namespace Carta{
namespace Data{

class ProfileRenderWorker{

public:

    /**
     * Constructor.
     */
    ProfileRenderWorker();

    /**
     * Store the parameters needed for computing the Profile.
     * @param dataSource - the image that will be the source of the Profile.
     * @param regionInfo - information about the region used to generate the profile.
     * @param profInfo - information about the profile to be generated.
     * @return - true if the parameters have changed since the last computation; false,
     *      otherwise.
     */
    bool setParameters(std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
        std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo,
        const Carta::Lib::ProfileInfo& profInfo );

    /**
     * Performs the work of computing the Profile data in a separate process.
     * @return - the file descriptor for the pipe that can be used to read the
     *      Profile data.
     */
    int computeProfile();

    /**
     * Destructor.
     */
    ~ProfileRenderWorker();

private:
    std::shared_ptr<Carta::Lib::Image::ImageInterface> m_dataSource;
    std::shared_ptr<Carta::Lib::Regions::RegionBase> m_regionInfo;
    Carta::Lib::ProfileInfo m_profileInfo;
    Carta::Lib::Hooks::ProfileResult m_result;

    ProfileRenderWorker( const ProfileRenderWorker& other);
    ProfileRenderWorker& operator=( const ProfileRenderWorker& other );
};
}
}



