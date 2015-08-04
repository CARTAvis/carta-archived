/// \warning the code below is still little messy as it implements painting to both QImage

/// and vector graphics
/// \todo clean up the code that draws into QImage, only leave vector graphics code
/// \todo convert this to astGetGrfContext/astGrfSet APIs?
/// \todo combine this file with the C++ code that drives this

#include "grfdriver.h"
#include "CartaLib/CartaLib.h"
#include <float.h>
#include <math.h>
#include <string.h>
#include <QPainter>

static GrfDriverGlobals globals;

GrfDriverGlobals *
grfGlobals()
{
    return & globals;
}

namespace VG = Carta::Lib::VectorGraphics;
namespace VGE = VG::Entries;

void GrfDriverGlobals::prepare()
{
    CARTA_ASSERT( vgComposer );

//    vgComposer-> append < VGE::SetPen > ( pens[0] );

    if ( !  image ) {
         image = new QImage( 100, 100, QImage::Format_ARGB32_Premultiplied );
         painter = new QPainter(  image );
         painter-> setRenderHint( QPainter::Antialiasing, false );
         painter-> setFont( QFont( "Helvetica", 10 ) );
         painter-> setPen( QPen( QColor( "white" ), 0.5 ) );
         painter-> setBrush( Qt::NoBrush );
    }

//     lineShadowColor =  lineShadowPen.color();
//     lineShadowWidth =  lineShadowPen.widthF();
//     lineShadowOn = lineShadowPenIndex >= 0;

    // reset color index
     currentPenIndex = 0;

    // reset font index
     currentFontIndex = 0;
}

static VG::VGComposer *
vgc()
{
    return grfGlobals()->vgComposer;
}

static QPainter &
painter()
{
    CARTA_ASSERT( grfGlobals()-> painter );
    return * grfGlobals()->painter;
}

/// helper to draw text & calculate bounding boxes around the text
static void
drawText( const char * text, float x, float y, const char * just,
          float upx, float upy, float * xb = 0, float * yb = 0 )
{
    QString jst = just ? QString( just ).toUpper() : "CC";

    QFontMetricsF fm( painter().font(), painter().device() );
    double tw = fm.width( text );
    double asc = fm.ascent() * 0.8;
    double th = fm.descent() + 1 + asc;

    // create an offset tx,ty based on adjustments
    double offx, offy;
    if ( jst[0] == 'M' ) {
        offy = 0;
    }
    else if ( jst[0] == 'B' ) {
        offy = - fm.descent();
    }
    else if ( jst[0] == 'T' ) {
        offy = asc + 1;
    }
    else {
        offy = asc + 1 - th / 2.0;
    }
    if ( jst[1] == 'L' ) {
        offx = 0;
    }
    else if ( jst[1] == 'R' ) {
        offx = - tw;
    }
    else {
        offx = - tw / 2.0;
    }

    // prepare the transform
    QTransform tr;
    tr.translate( x, y );
    tr.rotate( atan2( upx, upy ) * 180 / M_PI );

    if ( ! xb ) {
//        p().save();
//        p().setTransform( tr );
//        p().fillRect( offx, offy + fm.descent(), tw, - th, QColor( 0, 0, 0, 128 ) );
//        p().setPen( grfGlobals()->textColor );
//        p().drawText( offx, offy, text );
//        p().restore();

        vgc()->append < VGE::Save > ();
        vgc()->append < VGE::SetTransform > ( tr, true );

//            QRectF rect( offx, offy + fm.descent(), tw, - th);
//            vgc()->append<VGE::FillRect>( rect, QColor( 0, 0, 0, 128 ));
        if ( grfGlobals()-> lineShadowPenIndex >= 0 ) {
            vgc()->append < VGE::Save > ();
            vgc()->append < VGE::SetIndexedPen > ( grfGlobals()-> lineShadowPenIndex );
            vgc()->append < VGE::DrawText > ( text, QPointF( offx + 1, offy + 1 ) );
            vgc()->append < VGE::Restore > ();
        }
        vgc()->append < VGE::DrawText > ( text, QPointF( offx, offy ) );
        vgc()->append < VGE::Restore > ();
    }

    // compute the bounding rectangle...
    if ( xb && yb ) {
        QRectF rect( offx, offy + fm.descent(), tw, - th );
        QPointF p1 = tr.map( rect.bottomLeft() );
        QPointF p2 = tr.map( rect.bottomRight() );
        QPointF p3 = tr.map( rect.topRight() );
        QPointF p4 = tr.map( rect.topLeft() );
        xb[0] = p1.x();
        yb[0] = p1.y();
        xb[1] = p2.x();
        yb[1] = p2.y();
        xb[2] = p3.x();
        yb[2] = p3.y();
        xb[3] = p4.x();
        yb[3] = p4.y();
    }
} // drawText

/* =====================================================================================
 *
 * The set of functions below is the actual implementation of the low level AST graphics
 * driver. These functions are called by AST lib, in particular during the invocation of
 * astGrid(), but possibly in other places too (e.g. astGetC on some plot related
 * attributes).
 *
 * The code below is an adaptation of the pgplot based routines that come with astlib.
 * The original comments are left intact.
 *
 * Since AST is a C library, the following must be enclosed in extern "C".
 *
 * ======================================================================================
 */

extern "C" {
#include <ast.h>
#include <grf.h>                 /* Interface to this module */
#include <ast_err.h>             /* AST error codes */

int
astGBBuf( void )
{
    /*
*+
*  Name:
*     astGBBuf

*  Purpose:
*     Start a new graphics buffering context.

*  Synopsis:
*     #include "grf.h"
*     int astGBBuf( void )

*  Description:
*     This function begins saving graphical output commands in an
*     internal buffer; the commands are held until a matching astGEBuf
*     call (or until the buffer is emptied by astGFlush). This can
*     greatly improve the efficiency of some graphics systems. astGBBuf
*     increments an internal counter, while astGEBuf decrements this
*     counter and flushes the buffer to the output device when the
*     counter drops to zero.  astGBBuf and astGEBuf calls should always
*     be paired.

*  Parameters:
*     None.

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*-
*/

    vgc()-> append < VGE::Save > ();

    return 1;
}

int
astGEBuf( void )
{
    /*
*+
*  Name:
*     astGEBuf

*  Purpose:
*     End a graphics buffering context.

*  Synopsis:
*     #include "grf.h"
*     int astGEBuf( void )

*  Description:
*     This function marks the end of a batch of graphical output begun
*     with the last call of astGBBuf.  astGBBuf and astGEBUF calls should
*     always be paired. Each call to astGBBuf increments a counter, while
*     each call to astGEBuf decrements the counter. When the counter
*     reaches 0, the batch of output is written on the output device.

*  Parameters:
*     None.

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*-
*/

    //   ccpgebuf();
//    dbg(1) << "Trace\n";

    vgc()-> append < VGE::Restore > ();

    return 1;
}

int
astGFlush( void )
{
    /*
*+
*  Name:
*     astGFlush

*  Purpose:
*     Flush all pending graphics to the output device.

*  Synopsis:
*     #include "grf.h"
*     int astGFlush( void )

*  Description:
*     This function ensures that the display device is up-to-date,
*     by flushing any pending graphics to the output device.

*  Parameters:
*     None.

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*-
*/

    return 1;
}

int
astGCap( int cap, int /* value */ )
{
    /*
*+
*  Name:
*     astGCap

*  Purpose:
*     Indicate if this grf module has a given capability.

*  Synopsis:
*     #include "grf.h"
*     int astGCap( int cap, int value )

*  Description:
*     This function is called by the AST Plot class to determine if the
*     grf module has a given capability, as indicated by the "cap"
*     argument.

*  Parameters:
*     cap
*        The capability being inquired about. This will be one of the
*        following constants defined in grf.h:
*
*        GRF__SCALES: This function should return a non-zero value if
*        it implements the astGScales function, and zero otherwise. The
*        supplied "value" argument should be ignored.
*
*        GRF__MJUST: This function should return a non-zero value if
*        the astGText and astGTxExt functions recognise "M" as a
*        character in the justification string. If the first character of
*        a justification string is "M", then the text should be justified
*        with the given reference point at the bottom of the bounding box.
*        This is different to "B" justification, which requests that the
*        reference point be put on the baseline of the text, since some
*        characters hang down below the baseline. If the astGText or
*        astGTxExt function cannot differentiate between "M" and "B",
*        then this function should return zero, in which case "M"
*        justification will never be requested by Plot. The supplied
*        "value" argument should be ignored.
*
*        GRF__ESC: This function should return a non-zero value if the
*        astGText and astGTxExt functions can recognise and interpret
*        graphics escape sequences within the supplied string. These
*        escape sequences are described below. Zero should be returned
*        if escape sequences cannot be interpreted (in which case the
*        Plot class will interpret them itself if needed). The supplied
*        "value" argument should be ignored only if escape sequences cannot
*        be interpreted by astGText and astGTxExt. Otherwise, "value"
*        indicates whether astGText and astGTxExt should interpret escape
*        sequences in subsequent calls. If "value" is non-zero then
*        escape sequences should be interpreted by astGText and
*        astGTxExt. Otherwise, they should be drawn as literal text.

*  Returned Value:
*     The return value, as described above. Zero should be returned if
*     the supplied capability is not recognised.

*  Escape Sequences:
*     Escape sequences are introduced into the text string by a percent
*     "%" character. The following escape sequences are currently recognised
*     ("..." represents a string of one or more decimal digits):
*
*       %%      - Print a literal "%" character (type GRF__ESPER ).
*
*       %^...+  - Draw subsequent characters as super-scripts. The digits
*                 "..." give the distance from the base-line of "normal"
*                 text to the base-line of the super-script text, scaled
*                 so that a value of "100" corresponds to the height of
*                 "normal" text (type GRF__ESSUP ).
*       %^+     - Draw subsequent characters with the normal base-line.
*
*       %v...+  - Draw subsequent characters as sub-scripts. The digits
*                 "..." give the distance from the base-line of "normal"
*                 text to the base-line of the sub-script text, scaled
*                 so that a value of "100" corresponds to the height of
*                 "normal" text (type GRF__ESSUB ).
*
*       %v+     - Draw subsequent characters with the normal base-line
*                 (equivalent to %^+).
*
*       %>...+  - Leave a gap before drawing subsequent characters.
*                 The digits "..." give the size of the gap, scaled
*                 so that a value of "100" corresponds to the height of
*                 "normal" text (type GRF__ESGAP ).
*
*       %<...+  - Move backwards before drawing subsequent characters.
*                 The digits "..." give the size of the movement, scaled
*                 so that a value of "100" corresponds to the height of
*                 "normal" text (type GRF_ESBAC).
*
*       %s...+  - Change the Size attribute for subsequent characters. The
*                 digits "..." give the new Size as a fraction of the
*                 "normal" Size, scaled so that a value of "100" corresponds
*                 to 1.0  (type GRF__ESSIZ ).
*
*       %s+     - Reset the Size attribute to its "normal" value.
*
*       %w...+  - Change the Width attribute for subsequent characters. The
*                 digits "..." give the new width as a fraction of the
*                 "normal" Width, scaled so that a value of "100" corresponds
*                 to 1.0  (type GRF__ESWID ).
*
*       %w+     - Reset the Size attribute to its "normal" value.
*
*       %f...+  - Change the Font attribute for subsequent characters. The
*                 digits "..." give the new Font value  (type GRF__ESFON ).
*
*       %f+     - Reset the Font attribute to its "normal" value.
*
*       %c...+  - Change the Colour attribute for subsequent characters. The
*                 digits "..." give the new Colour value  (type GRF__ESCOL ).
*
*       %c+     - Reset the Colour attribute to its "normal" value.
*
*       %t...+  - Change the Style attribute for subsequent characters. The
*                 digits "..." give the new Style value  (type GRF__ESSTY ).
*
*       %t+     - Reset the Style attribute to its "normal" value.
*
*       %-      - Push the current graphics attribute values onto the top of
*                 the stack - see "%+" (type GRF__ESPSH).
*
*       %+      - Pop attributes values of the top the stack - see "%-". If
*                 the stack is empty, "normal" attribute values are restored
*                 (type GRF__ESPOP).
*
*     The astFindEscape function (in libast.a) can be used to locate escape
*     sequences within a text string. It has the following signature:
*
*     #include "plot.h"
*     int astFindEscape( const char *text, int *type, int *value, int *nc )
*
*     Parameters:
*        text
*           Pointer to the string to be checked.
*        type
*           Pointer to a location at which to return the type of escape
*           sequence. Each type is identified by a symbolic constant defined
*           in grf.h and is indicated in the above section. The returned value
*           is undefined if the supplied text does not begin with an escape
*           sequence.
*        value
*           Pointer to a lcation at which to return the integer value
*           associated with the escape sequence. All usable values will be
*           positive. Zero is returned if the escape sequence has no associated
*           integer. A value of -1 indicates that the attribute identified by
*           "type" should be reset to its "normal" value (as established using
*           the astGAttr function, etc). The returned value is undefined if
*           the supplied text does not begin with an escape sequence.
*        nc
*           Pointer to a location at which to return the number of
*           characters read by this call. If the text starts with an escape
*           sequence, the returned value will be the number of characters in
*           the escape sequence. Otherwise, the returned value will be the
*           number of characters prior to the first escape sequence, or the
*           length of the supplied text if no escape sequence is found.

*     Returned Value:
*        A non-zero value is returned if the supplied text starts with a
*        graphics escape sequence, and zero is returned otherwise.

*-
*/

//    dbg(1) << "Trace astGCap(" << cap << "," << value << ")\n";
    if ( cap == GRF__MJUST ) {
        return 1;
    }
    if ( cap == GRF__ESC ) {
        return 0;
    }
    if ( cap == GRF__SCALES ) {
        return 0;
    }
    return 0;
}

int
astGLine( int n, const float * x, const float * y )
{
    /*
*+
*  Name:
*     astGLine

*  Purpose:
*     Draw a polyline (i.e. a set of connected lines).

*  Synopsis:
*     #include "grf.h"
*     int astGLine( int n, const float *x, const float *y )

*  Description:
*     This function displays lines joining the given positions.

*  Parameters:
*     n
*        The number of positions to be joined together.
*     x
*        A pointer to an array holding the "n" x values.
*     y
*        A pointer to an array holding the "n" y values.

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*  Notes:
*     -  Nothing is done if "n" is less than 2, or if a NULL pointer is
*     given for either "x" or "y".

*-
*/

//    dbg(1) << "Trace\n";

    if ( n < 2 || ( ! x ) || ( ! y ) ) {
        return 1;
    }

    if ( n >= 2 ) {
        QPolygonF qpts( n );
        for ( int i = 0 ; i < n ; i++ ) {
            qpts[i].setX( x[i] );
            qpts[i].setY( y[i] );
        }

        if ( grfGlobals()-> lineShadowPenIndex >= 0 ) {
//            QPen shadowPen( grfGlobals()-> lineShadowColor, grfGlobals()->penWidth +
//                            grfGlobals()-> lineShadowWidth );
            vgc()-> append < VGE::Save > ();
            vgc()-> append < VGE::SetIndexedPen > ( grfGlobals()-> lineShadowPenIndex );
            vgc()-> append < VGE::DrawPolyline > ( qpts );
            vgc()-> append < VGE::Restore > ();
        }
        vgc()-> append < VGE::DrawPolyline > ( qpts );
    }

    return 1;
} // astGLine

int
astGMark( int n, const float * x, const float * y, int type )
{
    /*
*+
*  Name:
*     astGMark

*  Purpose:
*     Draw a set of markers.

*  Synopsis:
*     #include "grf.h"
*     int astGMark( int n, const float *x, const float *y, int type )

*  Description:
*     This function displays markers at the given positions.

*  Parameters:
*     n
*        The number of markers to draw.
*     x
*        A pointer to an array holding the "n" x values.
*     y
*        A pointer to an array holding the "n" y values.
*     type
*        An integer which can be used to indicate the type of marker symbol
*        required.

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*  Notes:
*     -  Nothing is done if "n" is less than 1, or if a NULL pointer is
*     given for either "x" or "y".

*-
*/

//    qDebug() << "astgmark";

    if ( n > 0 && x && y ) {
        for ( int i = 0 ; i < n ; i++ ) {
            painter().drawEllipse( x[i], y[i], type, type );
        }
    }
    return 1;
} // astGMark

int
astGText( const char * text, float x, float y, const char * just,
          float upx, float upy )
{
    /*
*+
*  Name:
*     astGText

*  Purpose:
*     Draw a character string.

*  Synopsis:
*     #include "grf.h"
*     int astGText( const char *text, float x, float y, const char *just,
*                   float upx, float upy )

*  Description:
*     This function displays a character string at a given position
*     using a specified justification and up-vector.

*  Parameters:
*     text
*        Pointer to a null-terminated character string to be displayed.
*     x
*        The reference x coordinate.
*     y
*        The reference y coordinate.
*     just
*        A character string which specifies the location within the
*        text string which is to be placed at the reference position
*        given by x and y. The first character may be 'T' for "top",
*        'C' for "centre", or 'B' for "bottom", and specifies the
*        vertical location of the reference position. Note, "bottom"
*        corresponds to the base-line of normal text. Some characters
*        (eg "y", "g", "p", etc) descend below the base-line. The second
*        character may be 'L' for "left", 'C' for "centre", or 'R'
*        for "right", and specifies the horizontal location of the
*        reference position. If the string has less than 2 characters
*        then 'C' is used for the missing characters.
*     upx
*        The x component of the up-vector for the text, in graphics world
*        coordinates. If necessary the supplied value should be negated
*        to ensure that positive values always refer to displacements from
*        left to right on the screen.
*     upy
*        The y component of the up-vector for the text, in graphics world
*        coordinates. If necessary the supplied value should be negated
*        to ensure that positive values always refer to displacements from
*        bottom to top on the screen.

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*  Notes:
*     -  Any graphics within the rotated box enclosing the text are erased.
*     -  A NULL value for "just" causes a value of "CC" to be used.
*     -  Both "upx" and "upy" being zero causes an error.
*     -  Any unrecognised character in "just" causes an error.
*-
*/

//    qDebug() << "asttext" << text;

    drawText( text, x, y, just, upx, upy );
    return 1;
}

int
astGScales( float * alpha, float * beta )
{
    /*
*+
*  Name:
*     astGScales

*  Purpose:
*     Get the axis scales.

*  Synopsis:
*     #include "grf.h"
*     int astGScales( float *alpha, float *beta )

*  Description:
*     This function returns two values (one for each axis) which scale
*     increments on the corresponding axis into a "normal" coordinate
*     system in which:
*        1 - The axes have equal scale in terms of (for instance)
*            millimetres per unit distance.
*        2 - X values increase from left to right.
*        3 - Y values increase from bottom to top.

*  Parameters:
*     alpha
*        A pointer to the location at which to return the scale for the
*        X axis (i.e. Xnorm = alpha*Xworld).
*     beta
*        A pointer to the location at which to return the scale for the
*        Y axis (i.e. Ynorm = beta*Yworld).

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*-
*/

    if ( alpha ) {
        * alpha = 1;
    }
    if ( beta ) {
        * beta = - 1;
    }

    return 1;
}

int
astGTxExt( const char * text, float x, float y, const char * just,
           float upx, float upy, float * xb, float * yb )
{
    /*
*+
*  Name:
*     astGTxExt

*  Purpose:
*     Get the extent of a character string.

*  Synopsis:
*     #include "grf.h"
*     int astGTxExt( const char *text, float x, float y, const char *just,
*                   float upx, float upy, float *xb, float *yb )

*  Description:
*     This function returns the corners of a box which would enclose the
*     supplied character string if it were displayed using astGText.
*
*     The returned box INCLUDES any leading or trailing spaces.

*  Parameters:
*     text
*        Pointer to a null-terminated character string to be displayed.
*     x
*        The reference x coordinate.
*     y
*        The reference y coordinate.
*     just
*        A character string which specifies the location within the
*        text string which is to be placed at the reference position
*        given by x and y. The first character may be 'T' for "top",
*        'C' for "centre", or 'B' for "bottom", and specifies the
*        vertical location of the reference position. Note, "bottom"
*        corresponds to the base-line of normal text. Some characters
*        (eg "y", "g", "p", etc) descend below the base-line. The second
*        character may be 'L' for "left", 'C' for "centre", or 'R'
*        for "right", and specifies the horizontal location of the
*        reference position. If the string has less than 2 characters
*        then 'C' is used for the missing characters.
*     upx
*        The x component of the up-vector for the text, in graphics world
*        coordinates. If necessary the supplied value should be negated
*        to ensure that positive values always refer to displacements from
*        left to right on the screen.
*     upy
*        The y component of the up-vector for the text, in graphics world
*        coordinates. If necessary the supplied value should be negated
*        to ensure that positive values always refer to displacements from
*        bottom to top on the screen.
*     xb
*        An array of 4 elements in which to return the x coordinate of
*        each corner of the bounding box.
*     yb
*        An array of 4 elements in which to return the y coordinate of
*        each corner of the bounding box.

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*  Notes:
*     -  The order of the corners is anti-clockwise (in world coordinates)
*        starting at the bottom left.
*     -  A NULL value for "just" causes a value of "CC" to be used.
*     -  Both "upx" and "upy" being zero causes an error.
*     -  Any unrecognised character in "just" causes an error.
*     -  Zero is returned for all bounds of the box if an error occurs.

*-
*/

    drawText( text, x, y, just, upx, upy, xb, yb );
    return 1;
}

int
astGQch( float * chv, float * chh )
{
    /*
*+
*  Name:
*     astGQch

*  Purpose:
*     Return the character height in world coordinates.

*  Synopsis:
*     #include "grf.h"
*     int astGQch( float *chv, float *chh )

*  Description:
*     This function returns the heights of characters drawn vertically and
*     horizontally in world coordinates.

*  Parameters:
*     chv
*        A pointer to the double which is to receive the height of
*        characters drawn with a vertical baseline . This will be an
*        increment in the X axis.
*     chh
*        A pointer to the double which is to receive the height of
*        characters drawn with a horizontal baseline. This will be an
*        increment in the Y axis.

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*-
*/

    if ( chh ) {
        * chh = painter().font().pointSizeF();
    }
    if ( chv ) {
        * chv = painter().font().pointSizeF();
    }
    return 1;
}

int
astGAttr( int attr, double value, double * old_value, int /* prim */ )
{
    /*
*+
*  Name:
*     astGAttr

*  Purpose:
*     Enquire or set a graphics attribute value.

*  Synopsis:
*     #include "grf.h"
*     int int astGAttr( int attr, double value, double *old_value, int prim )

*  Description:
*     This function returns the current value of a specified graphics
*     attribute, and optionally establishes a new value. The supplied
*     value is converted to an integer value if necessary before use.

*  Parameters:
*     attr
*        An integer value identifying the required attribute. The
*        following symbolic values are defined in grf.h:
*
*           GRF__STYLE  - Line style.
*           GRF__WIDTH  - Line width.
*           GRF__SIZE   - Character and marker size scale factor.
*           GRF__FONT   - Character font.
*           GRF__COLOUR - Colour index.
*     value
*        A new value to store for the attribute. If this is AST__BAD
*        no value is stored.
*     old_value
*        A pointer to a double in which to return the attribute value.
*        If this is NULL, no value is returned.
*     prim
*        The sort of graphics primitive to be drawn with the new attribute.
*        Identified by the following values defined in grf.h:
*           GRF__LINE
*           GRF__MARK
*           GRF__TEXT

*  Returned Value:
*     A value of 0 is returned if an error occurs, and 1 is returned
*     otherwise.

*  Notes:

*-
*/

    // in case we forget to handle old_value below, let's set it pre-emptively to 1
    if ( old_value ) {
        * old_value = 1.0;
    }

    // we don't handle different line styles, mostly because HTML5 does not handle it yet
    // \todo revisit this in the figure when <canvas> can do dashed/dotted lines?
    // Update: canvas.setLineDash() is now supported in many browsers?
    if ( attr == GRF__STYLE ) {
        if ( old_value ) {
            * old_value = 1.0;
        }
    }

    // line width - we handle this with colors
    else if ( attr == GRF__WIDTH ) {
        if ( old_value ) {
            * old_value = 1.0;
        }
    }

    // 'mostly' font size, although it could apply to markers, which I don't think are
    // used for grids... we certainly don't draw them
    else if ( attr == GRF__SIZE ) {
        if ( old_value ) {
            * old_value = painter().fontInfo().pointSizeF();
        }
        if ( value != AST__BAD ) {
            QFont font = painter().font();
            font.setPointSizeF( value );
            painter().setFont( font );

            vgc()-> append < VGE::SetFontSize > ( painter().font().pointSizeF() );
        }
    }

    // set font
    else if ( attr == GRF__FONT ) {
        if ( old_value ) {
            * old_value = grfGlobals()-> currentFontIndex;
        }
        if ( value != AST__BAD ) {
            int ind = value;
            grfGlobals()->currentFontIndex = ind;
            vgc()-> append < VGE::SetFontIndex > ( ind );
        }
    }

    // set color
    else if ( attr == GRF__COLOUR ) {
//        QString dbg = "xxyyzz COLOUR";
        if ( old_value ) {
            * old_value = grfGlobals()-> currentPenIndex;

//            dbg += " get";
        }
        if ( value != AST__BAD ) {
            int ind = value;
//            ind = Carta::Lib::clamp < int > ( ind, 0, grfGlobals()->pens.size() - 1 );
            grfGlobals()->currentPenIndex = ind;
//            vgc()-> append < VGE::SetPen > ( grfGlobals()->pens[ind] );
//            grfGlobals()-> penWidth = grfGlobals()->pens[ind].widthF();

            vgc()-> append < VGE::SetIndexedPen > ( ind );
//            grfGlobals()-> penWidth = grfGlobals()->pens[ind].widthF();


//            dbg += " set " + QString::number( value);
        }

//        qDebug() << dbg;
    }
    else { }

    return 1;
} // astGAttr
}


