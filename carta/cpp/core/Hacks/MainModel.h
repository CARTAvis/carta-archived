/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "PluginManager.h"
#include "GrayColormap.h"
#include <QObject>

/// helper mixin for plugin manager setter/getter
#define PM_MIXIN \
public: \
    inline void setPluginManager( PluginManager::SharedPtr pm ) { \
        CARTA_ASSERT( ! m_pluginManager ); \
        m_pluginManager = pm; \
    } \
    inline PluginManager::SharedPtr pluginManager() { \
        CARTA_ASSERT( m_pluginManager ); \
        return m_pluginManager; \
    } \
private: \
    PluginManager::SharedPtr m_pluginManager = nullptr

namespace Hacks
{
namespace Model
{
/// helper mixin for plugin manager setter/getter
/// crtp experiment
///
template < class T >
struct PMMix : public T {
    void
    setPluginManager( PluginManager::SharedPtr pm )
    {
        CARTA_ASSERT( ! m_pluginManager );
        m_pluginManager = pm;
    }

    PluginManager::SharedPtr
    pluginManager()
    {
        CARTA_ASSERT( m_pluginManager );
        return m_pluginManager;
    }

protected:

    PluginManager::SharedPtr m_pluginManager = nullptr;
};

/// container for all colormaps
class KnownColormaps : public QObject
{
    using IColormapNamed = Carta::Lib::PixelPipeline::IColormapNamed;

    Q_OBJECT
    CLASS_BOILERPLATE( KnownColormaps );
    Q_DISABLE_COPY( KnownColormaps )

public:

    KnownColormaps( PluginManager::SharedPtr pm )
    {
        m_cmaps.push_back( std::make_shared < Carta::Core::GrayColormap > () );
        auto hh =
            pm-> prepare < Carta::Lib::Hooks::ColormapsScalarHook > ();

        auto lam = [&] ( const Carta::Lib::Hooks::ColormapsScalarHook::ResultType & cmaps ) {
            m_cmaps.insert( m_cmaps.end(), cmaps.begin(), cmaps.end() );
        };
        hh.forEach( lam );

        qDebug() << "We have" << m_cmaps.size() << "colormaps:";
        for ( auto & cmap : m_cmaps ) {
            qDebug() << "    " << cmap-> name();
        }
    }

    virtual
    ~KnownColormaps() { }

 // add a colormap
    void
    add( IColormapNamed::SharedPtr cmap )
    {
        m_cmaps.push_back( cmap );
    }

    size_t
    count()
    {
        return m_cmaps.size();
    }

    IColormapNamed::ConstSharedPtr
    cmap( int ind )
    {
        // \todo should we use const_pointer_cast<> ???
        return m_cmaps[ind];
    }

signals:

    void
    updated();

private:

    std::vector < IColormapNamed::SharedPtr > m_cmaps;
};

class MainModel : public QObject // , public Model::PMMix < MainModel >
{
    Q_OBJECT
    CLASS_BOILERPLATE( MainModel );
    PM_MIXIN;

public:

    explicit
    MainModel( PluginManager::SharedPtr pm, QObject * parent = 0 );

    ~MainModel();

    std::shared_ptr < KnownColormaps >
    knownColormaps();

signals:

public slots:

private:

    Carta::Lib::PixelPipeline::IClippedPixelPipeline::SharedPtr m_ppipe = nullptr;
    QString m_statePrefix;
};
}

// singleton - container of globals for hacks
class GlobalsH // : public Model::PMMix < GlobalsH >
{
    Q_DISABLE_COPY( GlobalsH )
    CLASS_BOILERPLATE( GlobalsH );
    PM_MIXIN;

public:

    /// singleton pattern
    static GlobalsH &
    instance();

    /// get the main model
    Model::MainModel &
    mainModel();

private:

    static GlobalsH * m_instance;
    Model::MainModel::UniquePtr m_mainModel = nullptr;

    // private constructor
    GlobalsH();
};
}

#undef PM_MIXIN

Q_DECLARE_SMART_POINTER_METATYPE( std::shared_ptr )
