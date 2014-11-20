#include "Data/Colormap.h"

#include <QDebug>


const QString Colormap::CLASS_NAME = "Colormap";

bool Colormap::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Colormap::Factory());

Colormap::Colormap( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

void Colormap::_initializeDefaultState(){

}

Colormap::~Colormap(){

}
