/**
 *
 **/


#include "ManagedLayerView.h"

namespace Carta
{
namespace Hacks
{

ManagedLayer::ManagedLayer(ManagedLayerViewInh * mlv)
{
    m_mlv = mlv;
    m_mlv-> p_addLayer( this);
}

ManagedLayerViewInh::ManagedLayerViewInh(QString viewName, IConnector * connector, QObject * parent)
{
    m_connector = connector;

    m_rvgv.reset( m_connector-> makeRemoteVGView( viewName));
}

IConnector *ManagedLayerViewInh::connector()
{
    return m_connector;
}

void ManagedLayerViewInh::p_addLayer(ManagedLayer * layer)
{
    m_layers.push_back( layer);
}

}}
