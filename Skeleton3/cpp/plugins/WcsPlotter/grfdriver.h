

#pragma once

#include <QImage>

// getter
//QImage &
//grfImage();

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
