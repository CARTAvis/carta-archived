/**
 * Generates image profile data.
 */
#pragma once

#include "CartaLib/IPlugin.h"
#include "CartaLib/RegionInfo.h"
#include "CartaLib/ProfileInfo.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include <imageanalysis/ImageAnalysis/ImageCollapserData.h>

#include <QObject>

namespace casa {
    class ImageRegion;
}


class ProfileCASA : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:

    /**
     * Constructor.
     */
    ProfileCASA(QObject *parent = 0);
    virtual bool handleHook(BaseHook & hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
    virtual ~ProfileCASA();
private:
    casa::MFrequency::Types _determineRefFrame(
            std::shared_ptr<casa::ImageInterface<casa::Float> > img ) const;
    std::vector<double> _generateProfile( casa::ImageInterface < casa::Float > * imagePtr,
            Carta::Lib::RegionInfo regionInfo, Carta::Lib::ProfileInfo profileInfo ) const;
    casa::ImageCollapserData::AggregateType _getCombineMethod( Carta::Lib::ProfileInfo profileInfo ) const;
    casa::ImageRegion* _getEllipsoid(const casa::CoordinateSystem& cSys,
            const casa::Vector<casa::Double>& x, const casa::Vector<casa::Double>& y) const;
    casa::ImageRegion* _getPolygon(const casa::CoordinateSystem& cSys,
            const casa::Vector<casa::Double>& x, const casa::Vector<casa::Double>& y) const;
    casa::Record _getRegionRecord( Carta::Lib::RegionInfo::RegionType shape, const casa::CoordinateSystem& cSys,
            const casa::Vector<casa::Double>& x, const casa::Vector<casa::Double>& y) const;
};
