/**
 *
 **/

#include "MainModel.h"

namespace Hacks
{
namespace Model
{
MainModel::MainModel( QObject * parent )
    : QObject( parent )
{ }

MainModel::~MainModel()
{ }
}

GlobalsH * GlobalsH::m_instance = nullptr;

GlobalsH *GlobalsH::instance() {
    if( ! m_instance) {
        m_instance = new GlobalsH;
    }
    CARTA_ASSERT( m_instance);
    return m_instance;
}

GlobalsH::GlobalsH()
{

}

}
