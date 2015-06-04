#pragma once

// new APIs for using vector graphics

#include "CartaLib/VectorGraphics/VGList.h"
#include <QPen>
#include <QFont>

struct GrfDriverGlobals {
    // externally configurable:
    // ========================
    int lineShadowPenIndex = 0;

    // internal details - these are computed and set inside grfdriverSetVGComposer()
    // then they are used during the plot
    // =====================
    int currentPenIndex = 0;
    int currentFontIndex = 0;
    QImage * image = nullptr;
    QPainter * painter = nullptr;
    Carta::Lib::VectorGraphics::VGComposer * vgComposer = nullptr;

    /// call this righ before astPlot()
    void prepare();
};

// c-style singleton access
GrfDriverGlobals * grfGlobals();
