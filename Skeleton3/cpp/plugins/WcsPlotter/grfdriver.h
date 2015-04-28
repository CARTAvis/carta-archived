

#pragma once

// new APIs for using vector graphics

#include "CartaLib/VectorGraphics/VGList.h"

void grfdriverSetVGComposer( Carta::Lib::VectorGraphics::VGComposer * vgComp);


// old APIs for using QImage


#include <QImage>

/// set the destination image
/// \note ownership remains with caller
void
grfSetImage( QImage * img );

/// set text color
void
grfSetLineColor( QString color );

/// set line color
void
grfSetTextColor( QString color );
