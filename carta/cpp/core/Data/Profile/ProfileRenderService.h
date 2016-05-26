/**
 * Manages the production of profile data from an image cube.
 **/

#pragma once


#include "CartaLib/CartaLib.h"
#include "CartaLib/RegionInfo.h"
#include "CartaLib/Hooks/ProfileResult.h"

#include <QObject>
#include <QQueue>
#include <memory>

namespace Carta {
namespace Lib {

namespace Image {
class ImageInterface;
}
}
}

namespace Carta{
namespace Data{

class ProfileRenderWorker;
class ProfileRenderThread;

class ProfileRenderService : public QObject {
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    explicit ProfileRenderService( QObject * parent = 0 );

    /**
     * Initiates the process of rendering the Profile.
     * @param dataSource - the image that will be the source of the profile.
     * @param regionInfo - information about the region within the image that will be profiled.
     * @param profInfo - information about the profile to be rendered such as rest frequency.
     * @param curveIndex - the profile curve to be rendered.
     * @param layerName - the name of the layer responsible for the profile.
     * @param createNew - whether this is a new profile or a replacement for an existing one.
     * @return - whether or not the profile is being rendered.
     */
    bool renderProfile(std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
            Carta::Lib::RegionInfo& regionInfo, Carta::Lib::ProfileInfo& profInfo,
            int curveIndex, const QString& layerName, bool createNew );

    /**
     * Destructor.
     */
    ~ProfileRenderService();

signals:

    /**
     * Notification that new Profile data has been computed.
     */
    void profileResult( const Carta::Lib::Hooks::ProfileResult&,
            int curveIndex, const QString& layerName, bool createNew,
            std::shared_ptr<Carta::Lib::Image::ImageInterface> image );

private slots:

    void _postResult( );

private:
    void _scheduleRender( std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
            Carta::Lib::RegionInfo& regionInfo, Carta::Lib::ProfileInfo& profInfo );
    ProfileRenderWorker* m_worker;
    ProfileRenderThread* m_renderThread;
    bool m_renderQueued;


    struct RenderRequest {
        bool m_createNew;
        int m_curveIndex;
        QString m_layerName;
        Carta::Lib::ProfileInfo m_profileInfo;
        Carta::Lib ::RegionInfo m_regionInfo;
        std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image;
    };
    QQueue<RenderRequest> m_requests;

    ProfileRenderService( const ProfileRenderService& other);
    ProfileRenderService& operator=( const ProfileRenderService& other );
};
}
}



