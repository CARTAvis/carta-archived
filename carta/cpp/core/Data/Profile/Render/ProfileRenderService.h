/**
 * Manages the production of profile data from an image cube.
 **/

#pragma once


#include "CartaLib/CartaLib.h"
#include "CartaLib/Hooks/ProfileResult.h"

#include <QObject>
#include <QQueue>
#include <memory>


namespace Carta{
namespace Data{

class Layer;
class ProfileRenderWorker;
class ProfileRenderThread;
class ProfileRenderRequest;
class Region;

class ProfileRenderService : public QObject {
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    explicit ProfileRenderService( QObject * parent = 0 );

    /**
     * Initiates the process of rendering the Profile.
     * @param layer - the image that will be the source of the profile.
     * @param region - information about the region within the image that will be profiled.
     * @param profInfo - information about the profile to be rendered such as rest frequency.
     * @param createNew - whether this is a new profile or a replacement for an existing one.
     * @return - whether or not the profile is being rendered.
     */

    bool renderProfile(std::shared_ptr<Layer> layer, std::shared_ptr<Region> region,
            const Carta::Lib::ProfileInfo& profInfo, bool createNew );

    /**
     * Destructor.
     */
    ~ProfileRenderService();

signals:

    /**
     * Notification that new Profile data has been computed.
     */
    void profileResult( const Carta::Lib::Hooks::ProfileResult&,
            std::shared_ptr<Layer> layer,
            std::shared_ptr<Region> region,
            bool createNew);

private slots:

    void _postResult( );

private:
    void _scheduleRender( std::shared_ptr<Layer> layer,
            std::shared_ptr<Region> region, const Carta::Lib::ProfileInfo& profInfo );
    ProfileRenderWorker* m_worker;
    ProfileRenderThread* m_renderThread;
    bool m_renderQueued;
    QQueue<ProfileRenderRequest> m_requests;

    ProfileRenderService( const ProfileRenderService& other);
    ProfileRenderService& operator=( const ProfileRenderService& other );
};
}
}



