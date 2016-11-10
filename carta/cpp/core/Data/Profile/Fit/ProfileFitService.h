/**
 * Manages the production of profile data from an image cube.
 **/

#pragma once


#include "CartaLib/CartaLib.h"
#include "CartaLib/Hooks/FitResult.h"
#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/ProfileInfo.h"
#include "CartaLib/IImage.h"
#include <QObject>
#include <QQueue>
#include <memory>




namespace Carta{
namespace Data{

class ProfileFitThread;

class ProfileFitService : public QObject {
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    explicit ProfileFitService( QObject * parent = 0 );

    /**
     * Fit curves to the list of data.
     * @param fitInfos - a list of data to fit along with the corresponding fit parameters.
     * @return - whether or not a fit will be performed.
     */
    bool fitProfile( const std::vector<Carta::Lib::Fit1DInfo>& fitInfos  );


    /**
     * Destructor.
     */
    ~ProfileFitService();

signals:

    /**
     * The results of a fit are available.
     */
    void fitResult( const std::vector<Carta::Lib::Hooks::FitResult>& results);


private slots:

    void _postResult( );

private:
    void _scheduleCompute( const std::vector<Carta::Lib::Fit1DInfo>& fitInfos );

    ProfileFitThread* m_fitThread;
    bool m_fitQueued;


    struct FitRequest {
        bool m_createNew;
        int m_curveIndex;
        QString m_layerName;
        Carta::Lib::ProfileInfo m_profileInfo;
        std::shared_ptr<Carta::Lib ::Regions::RegionBase> m_regionInfo;
        std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image;
    };
    QQueue<FitRequest> m_requests;

    ProfileFitService( const ProfileFitService& other);
    ProfileFitService& operator=( const ProfileFitService& other );
};
}
}



