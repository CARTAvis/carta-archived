#pragma once

// new APIs for using vector graphics

#include "CartaLib/VectorGraphics/VGList.h"

struct GrfDriverGlobals {
    // externally configurable:
    QColor gridColor = QColor( "white" );
    QColor borderColor = QColor( "blue" );
    QColor axesColor = QColor( "pink" );
    QColor textColor = QColor( "yellow" );
    double penWidth = 1.0;

    // internal details
    enum Colors { Grid = 0, Border, Axes, Text };
    int currentColorIndex = 0;
    std::vector < QColor > colors {
        QColor( "white" ),
        QColor( "blue" ),
        QColor( "pink" ),
        QColor( "yellow" )
    };

    QImage * image = nullptr;
//    QColor lineColor = QColor( "#ffff00" );
    QPainter * painter = nullptr;

    Carta::Lib::VectorGraphics::VGComposer * vgComposer = nullptr;

};

GrfDriverGlobals & grfDriverGlobals();

void
grfdriverSetVGComposer( Carta::Lib::VectorGraphics::VGComposer * vgComp );

// old APIs for using QImage

#include <QImage>

/// set the destination image
/// \note ownership remains with caller
void
grfSetImage( QImage * img );

/// set text color
//void
//grfSetLineColor( QString color );

/// set line color
//void
//grfSetTextColor( QString color );
