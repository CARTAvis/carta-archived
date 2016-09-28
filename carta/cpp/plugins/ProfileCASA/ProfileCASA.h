/**
 * Generates image profile data.
 */
#pragma once

#include "CartaLib/IPlugin.h"
#include "CartaLib/ProfileInfo.h"
#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/Hooks/ProfileResult.h"
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
    Carta::Lib::Hooks::ProfileResult _generateProfile( casa::ImageInterface < casa::Float > * imagePtr,
            std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo, Carta::Lib::ProfileInfo profileInfo ) const;
    casa::ImageCollapserData::AggregateType _getCombineMethod( Carta::Lib::ProfileInfo profileInfo ) const;
    casa::ImageRegion* _getEllipsoid(const casa::CoordinateSystem& cSys,
            const casa::Vector<casa::Double>& x, const casa::Vector<casa::Double>& y) const;
    casa::ImageRegion* _getPolygon(const casa::CoordinateSystem& cSys,
            const casa::Vector<casa::Double>& x, const casa::Vector<casa::Double>& y) const;
    casa::Record _getRegionRecord( const QString& shape, const casa::CoordinateSystem& cSys,
            const casa::Vector<casa::Double>& x, const casa::Vector<casa::Double>& y) const;

    casa::Vector<casa::Double> _toWorld( const casa::CoordinateSystem& cSys,
    		double x, double y, bool* successful ) const;
    const QString PIXEL_UNIT;
    const QString RADIAN_UNIT;
};
