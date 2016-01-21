/**
 *
 **/


#pragma once

#include "CartaLib/CartaLib.h"
#include "ManagedLayerView.h"
#include <QObject>

namespace Carta
{
namespace Hacks
{
class LayeredViewDemo : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( LayeredViewDemo);

public:
    explicit LayeredViewDemo(QObject *parent = 0);
    ~LayeredViewDemo();

signals:

public slots:

private:
    struct Pimpl;
    Pimpl * m_pimpl = nullptr;

};

}
}
