/**
 *
 **/


#include "ManagedLayerView.h"

namespace Carta
{
namespace Hacks
{

ManagedLayer::ManagedLayer(ManagedLayerViewInh * mlv, QString name)
{
    m_mlv = mlv;
    m_layerName = name;
    m_mlv-> p_addLayer( this);
}

void ManagedLayer::setRaster(const QImage & image)
{

}

ManagedLayerViewInh::ManagedLayerViewInh(QString viewName, IConnector * connector, QObject * parent)
    : QObject( parent)
{
    m_connector = connector;

    m_rvgv.reset( m_connector-> makeRemoteVGView( viewName));

}

IConnector *ManagedLayerViewInh::connector()
{
    return m_connector;
}

ManagedLayer::ID ManagedLayerViewInh::p_addLayer(ManagedLayer * layer)
{
    m_layers.push_back( layer);
    return m_nextId ++ ;
}

}}
