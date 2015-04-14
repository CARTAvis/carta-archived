/**
 *
 **/

#include "VGList.h"

namespace Carta
{
namespace Lib
{
namespace VectorGraphics
{

VGList::VGList()
{

}

VGList::~VGList()
{

}

void VGList::setQImage(QImage img)
{
    m_qImage = img;
}

QImage VGList::qImage()
{
    return m_qImage;
}

}
}
}
