#include "Plot2D/Plot2DTextMarker.h"
#include <QDebug>

namespace Carta {
namespace Plot2D {


Plot2DTextMarker::Plot2DTextMarker(){
    setLabelAlignment( Qt::AlignCenter | Qt::AlignTop );
}


void Plot2DTextMarker::setContent( const QString& labelText ){
    QwtText qText( labelText, QwtText::TextFormat::RichText );
    QFont font;
    font = qText.usedFont( font );
    font.setPointSize( 6 );
    qText.setFont(font );
    qText.setColor( Qt::black );
    QColor backColor( "#F6F6F6" );
    backColor.setAlpha( 200 );
    QBrush backBrush( backColor );
    qText.setBackgroundBrush( backBrush );
    qText.setBorderRadius( 1 );
    qText.setPaintAttribute( QwtText::PaintAttribute::PaintBackground, true );
    setLabel( qText );
}


Plot2DTextMarker::~Plot2DTextMarker(){
}
}
}
