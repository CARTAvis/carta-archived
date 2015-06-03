/**
 *
 **/

#include "VGList.h"
#include <QPainter>

namespace Carta
{
namespace Lib
{
namespace VectorGraphics
{
VGList::VGList()
{ }

VGList::~VGList()
{
//    for ( auto & entry : m_entries ) {
//        if ( entry ) {
//            delete entry;
//        }
//    }
}

//void
//VGList::setQImage( QImage img )
//{
//    m_qImage = img;
//}

//const QImage &
//VGList::qImage() const
//{
//    return m_qImage;
//}

bool
VGListQPainterRenderer::render( const VGList & vgList, QPainter & qPainter )
{
    BetterQPainter bp( qPainter );
    for ( auto entry : vgList.entries() ) {
        entry-> cplusplus( bp );
    }

//    qPainter.drawImage( 0, 0, vgList.qImage() );
    return true;
}
}
}
}
