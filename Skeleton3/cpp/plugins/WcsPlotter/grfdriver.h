#pragma once

// new APIs for using vector graphics

#include "CartaLib/VectorGraphics/VGList.h"
#include <QPen>
#include <QFont>

struct GrfDriverGlobals {
    // externally configurable:
    std::vector< QFont> qfonts;
    std::vector < QColor > colors {
        QColor( "white" ),
        QColor( "blue" ),
        QColor( "pink" ),
        QColor( "yellow" )
    };
    QPen lineShadowPen = QPen( QColor( 0, 0, 0, 16), 2);
    QColor textShadowColor = QColor( "yellow");

    // internal details
    double penWidth = 1.0;
    int currentColorIndex = 0;
    QImage * image = nullptr;
    QPainter * painter = nullptr;
    Carta::Lib::VectorGraphics::VGComposer * vgComposer = nullptr;
    // computed from lineShadowPen, true if alpha > 0
    bool lineShadowOn = true;
    QColor lineShadowColor = QColor( "green");
    double lineShadowWidth = 1.0;
};

GrfDriverGlobals * grfGlobals();

void
grfdriverSetVGComposer( Carta::Lib::VectorGraphics::VGComposer * vgComp );
