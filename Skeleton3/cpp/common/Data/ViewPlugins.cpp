#include "Data/ViewPlugins.h"
#include "Globals.h"
#include "PluginManager.h"
#include "Animator.h"
#include "Histogram.h"
#include "Colormap.h"

#include <QDir>
#include <QDebug>

namespace Carta {

namespace Data {

class ViewPlugins::Factory : public CartaObjectFactory {

public:

    Factory():
        CartaObjectFactory( PLUGINS ){};

    CartaObject * create (const QString & path, const QString & id)
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
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new ViewPlugins::Factory());

ViewPlugins::ViewPlugins( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

void ViewPlugins::_insertPlugin( int ind, const QString& name, const QString& description,
        const QString& type, const QString& version, const QString& errors ){
    QString index = QString("%1").arg(ind);
    QString arrayIndex = PLUGINS + StateInterface::DELIMITER + index;
    m_state.insertValue<QString>( arrayIndex + StateInterface::DELIMITER + NAME, name);
    m_state.insertValue<QString>( arrayIndex + StateInterface::DELIMITER + DESCRIPTION, description);
    m_state.insertValue<QString>( arrayIndex + StateInterface::DELIMITER + TYPE, type);
    m_state.insertValue<QString>( arrayIndex + StateInterface::DELIMITER + VERSION, version);
    m_state.insertValue<QString>( arrayIndex + StateInterface::DELIMITER + ERRORS, errors);
}

void ViewPlugins::_initializeDefaultState(){
    auto pm = Globals::instance()-> pluginManager();
    auto infoList = pm-> getInfoList();
    int ind = 0;
    int infoListSize = infoList.size()+4;
    m_state.insertArray( PLUGINS, infoListSize );
    for( auto & entry : infoList) {
        //qDebug() << "  path:" << entry.soPath;
        _insertPlugin( ind, entry.json.name, entry.json.description, entry.json.typeString, entry.json.version, entry.errors.join("|"));
        ind ++;
    }
    _insertPlugin( ind, Animator::CLASS_NAME, "Animation of data sets", "", "", "");
    ind++;
    _insertPlugin( ind, "statistics", "Placeholder for statistics plugin", "", "", "");
    ind++;
    _insertPlugin( ind, Histogram::CLASS_NAME, "Histogram", "", "", "");
    ind++;
    _insertPlugin( ind, Colormap::CLASS_NAME, "Colormap", "", "", "");
    ind++;
    m_state.insertValue<int>( STAMP, ind);
    m_state.flushState();
}

ViewPlugins::~ViewPlugins(){

}
}
}
