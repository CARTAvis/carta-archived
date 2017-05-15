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


namespace casacore {
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

    casacore::MFrequency::Types _determineRefFrame(
            std::shared_ptr<casacore::ImageInterface<casacore::Float> > img ) const;
    Carta::Lib::Hooks::ProfileResult _generateProfile( casacore::ImageInterface < casacore::Float > * imagePtr,
            std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo, Carta::Lib::ProfileInfo profileInfo ) const;
    casa::ImageCollapserData::AggregateType _getCombineMethod( Carta::Lib::ProfileInfo profileInfo ) const;
    casacore::ImageRegion* _getEllipsoid(const casacore::CoordinateSystem& cSys,
            const casacore::Vector<casacore::Double>& x, const casacore::Vector<casacore::Double>& y) const;
    casacore::ImageRegion* _getPolygon(const casacore::CoordinateSystem& cSys,
            const casacore::Vector<casacore::Double>& x, const casacore::Vector<casacore::Double>& y) const;
    casacore::Record _getRegionRecord( const QString& shape, const casacore::CoordinateSystem& cSys,
            const casacore::Vector<casacore::Double>& x, const casacore::Vector<casacore::Double>& y) const;

    casacore::Vector<casacore::Double> _toWorld( const casacore::CoordinateSystem& cSys,
    		double x, double y, bool* successful ) const;
    const QString PIXEL_UNIT;
    const QString RADIAN_UNIT;
};
