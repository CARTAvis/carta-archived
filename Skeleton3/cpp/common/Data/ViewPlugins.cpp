#include "ViewPlugins.h"
#include "Globals.h"
#include "PluginManager.h"
#include "Animator/Animator.h"
#include "Controller.h"
#include "Statistics.h"
#include "Histogram.h"
#include "Colormap/Colormap.h"
#include "State/UtilState.h"

#include <QDir>
#include <QDebug>

namespace Carta {

namespace Data {

typedef Carta::State::UtilState UtilState;

class ViewPlugins::Factory : public Carta::State::CartaObjectFactory {

public:

    Factory():
        CartaObjectFactory( PLUGINS ){};

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new ViewPlugins (path, id);
    }
};


const QString ViewPlugins::PLUGINS = "pluginList";
const QString ViewPlugins::NAME = "name";
const QString ViewPlugins::DESCRIPTION = "description";
const QString ViewPlugins::TYPE = "type";
const QString ViewPlugins::VERSION = "version";
const QString ViewPlugins::ERRORS = "loadErrors";
const QString ViewPlugins::STAMP = "pluginCount";
const QString ViewPlugins::CLASS_NAME = "ViewPlugins";
bool ViewPlugins::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new ViewPlugins::Factory());

ViewPlugins::ViewPlugins( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

void ViewPlugins::_insertPlugin( int ind, const QString& name, const QString& description,
        const QString& type, const QString& version, const QString& errors ){
    QString index = QString("%1").arg(ind);
    QString arrayIndex = UtilState::getLookup(PLUGINS, index);
    m_state.insertValue<QString>( UtilState::getLookup( arrayIndex, NAME), name);
    m_state.insertValue<QString>( UtilState::getLookup( arrayIndex, DESCRIPTION), description);
    m_state.insertValue<QString>( UtilState::getLookup( arrayIndex, TYPE), type);
    m_state.insertValue<QString>( UtilState::getLookup(arrayIndex, VERSION), version);
    m_state.insertValue<QString>( UtilState::getLookup(arrayIndex, ERRORS), errors);
}

void ViewPlugins::_initializeDefaultState(){
    /*auto pm = Globals::instance()-> pluginManager();
    auto infoList = pm-> getInfoList();
    int ind = 0;
    int infoListSize = infoList.size()+4;
    m_state.insertArray( PLUGINS, infoListSize );
    for( auto & entry : infoList) {
        //qDebug() << "  path:" << entry.soPath;
        _insertPlugin( ind, entry.json.name, entry.json.description, entry.json.typeString, entry.json.version, entry.errors.join("|"));
        ind ++;
    }*/
    m_state.insertArray( PLUGINS, 5 );
    int ind = 0;
    _insertPlugin( ind, Controller::PLUGIN_NAME, "Image display", "", "", "");
    ind++;
    _insertPlugin( ind, Animator::CLASS_NAME, "Animation of data sets", "", "", "");
    ind++;
    _insertPlugin( ind, Statistics::CLASS_NAME, "Cursor information", "", "", "");
    ind++;
    _insertPlugin( ind, Histogram::CLASS_NAME, "Histogram", "", "", "");
    ind++;
    _insertPlugin( ind, Colormap::CLASS_NAME, "Color map", "", "", "");
    ind++;
    m_state.insertValue<int>( STAMP, ind);
    m_state.flushState();
}

ViewPlugins::~ViewPlugins(){

}
}
}
