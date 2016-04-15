/**
 * Generates image profile data.
 */
#pragma once

#include "CartaLib/IPlugin.h"
#include "CartaLib/RegionInfo.h"
#include "CartaLib/ProfileInfo.h"

#include <QObject>



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
    std::vector<double> _generateProfile( std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr,
            Carta::Lib::RegionInfo regionInfo, Carta::Lib::ProfileInfo profileInfo ) const;
};
