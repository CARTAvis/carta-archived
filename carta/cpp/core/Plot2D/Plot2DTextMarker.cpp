#include "Plot2D/Plot2DTextMarker.h"
#include <QDebug>

namespace Carta {
namespace Plot2D {


Plot2DTextMarker::Plot2DTextMarker(){
}


void Plot2DTextMarker::setContent( const QString& labelText ){
    QwtText qText( labelText );
    setLabel( labelText );
}


Plot2DTextMarker::~Plot2DTextMarker(){
}
}
}
