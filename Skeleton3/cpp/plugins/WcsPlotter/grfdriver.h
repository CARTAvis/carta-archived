#ifndef GRFGLOBALS_H
#define GRFGLOBALS_H

#pragma once

#include <QImage>

// getter
QImage &
grfImage();

// setter
void
grfSetImage( QImage * img );

// set text/line colors
void
grfSetLineColor( QString color );

void
grfSetTextColor( QString color );

#endif // GRFGLOBALS_H
