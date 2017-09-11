#include "Data/Colormap/Colormaps.h"
#include "Data/Util.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "GrayColormap.h"
#include "State/UtilState.h"

#include "Globals.h"
#include "PluginManager.h"

#include <QDebug>
#include <QColor>
#include <set>

namespace Carta {

namespace Data {

const QString Colormaps::COLOR_LIST = "ColorMaps";
const QString Colormaps::CLASS_NAME = "Colormaps";
const QString Colormaps::COLOR_MAPS = "maps";
const QString Colormaps::COLOR_MAP_COUNT = "colorMapCount";

class Colormaps::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){}

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Colormaps (path, id);
        }
    };


bool Colormaps::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Colormaps::Factory());


Colormaps::Colormaps( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}




QStringList Colormaps::getColorMaps() const {
    QStringList buff;
    int colormapCount = m_colormaps.size();
    for ( int i = 0; i < colormapCount; i++ ){
        buff << m_colormaps[i]->name();
    }
    return buff;
}

void Colormaps::_initializeDefaultState(){
    // get all colormaps provided by core
    m_colormaps.push_back( std::make_shared < Carta::Core::GrayColormap > () );

    // ask plugins for colormaps
    auto hh = Globals::instance()-> pluginManager()-> prepare < Carta::Lib::Hooks::
                                                              ColormapsScalarHook > ();

    auto lam = [=] ( const Carta::Lib::Hooks::ColormapsScalarHook::ResultType &cmaps ) {
        m_colormaps.insert( m_colormaps.end(), cmaps.begin(), cmaps.end() );
    };
    hh.forEach( lam );

    int colorMapCount = m_colormaps.size();
    m_state.insertValue<int>( COLOR_MAP_COUNT, colorMapCount );
    m_state.insertArray( COLOR_MAPS, colorMapCount );
    for ( int i = 0; i < colorMapCount; i++ ){
        QString arrayIndexStr = Carta::State::UtilState::getLookup( COLOR_MAPS, QString::number(i));
        m_state.setValue<QString>(arrayIndexStr, m_colormaps[i]->name());

    }
    m_state.flushState();
}

std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed>  Colormaps::getColorMap( const QString& mapName ) const {
    int mapCount = m_colormaps.size();
    std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> map = nullptr;
    for ( int i = 0; i < mapCount; i++ ){
        if ( m_colormaps[i]->name() == mapName ){
            map = m_colormaps[i];
            break;
        }
    }
    return map;
}


bool Colormaps::isMap( const QString& name ) const {
    int colorMapCount = m_colormaps.size();
    bool colorMap = false;
    for ( int i = 0; i < colorMapCount; i++ ){
        if ( name == m_colormaps[i]->name()){
           colorMap = true;
           break;
        }
    }
    return colorMap;
}



Colormaps::~Colormaps(){

}
}
}
