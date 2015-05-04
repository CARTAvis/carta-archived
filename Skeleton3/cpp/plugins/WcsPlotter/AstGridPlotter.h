/// c++ code for plotting WCS grids using AST library
///
///     http://starlink.eao.hawaii.edu/starlink/AST
///
/// This is essentially my attempt to make a dumb C++ interface for interacting with AST,
/// at least for drawing grids.
///
/// \warning This class is very unfriendly to multithreading because it uses
/// ASTlib's plotting, which I could only get to work using global variables.

#pragma once

#include "CartaLib/VectorGraphics/VGList.h"
#include <QSize>
#include <QString>
#include <QRectF>
#include <QStringList>

class QImage;

namespace WcsPlotterPluginNS
{
///
/// Renders a wcs grid to a VG composer using starlink's AST library.
///
class AstGridPlotterQImage
{
public:

    typedef Carta::Lib::VectorGraphics::VGComposer VGComposer;

    AstGridPlotterQImage();
    ~AstGridPlotterQImage();

    /// feed the plotter the raw FITS header (concatenated 80-char strings)
    /// returns whether the header has enough information about
    /// about sky coordinates
    bool
    setFitsHeader( const QString & hdr );

    /// set whether to use the old CAR interpretation (i.e. CAR is linear)
    void
    setCarLin( bool flag );

    /// set which system to plot
    void
    setSystem( const QString & system = QString() );

    /// sets the output image (does not take ownership)
    void
    setOutputImage( QImage * img );

    /// sets the output VGComposer
    void
    setOutputVGComposer( VGComposer * vgc);

    /// sets the output rectangle in screen coordinates for the grid
    /// the annotation text will be rendered outside of this rectangle, so
    /// leave some space for it...
    void
    setOutputRect( const QRectF & rect );

    /// sets which part of the image to draw coordinates for,
    /// specified in CASA coordinates (i.e. center of left-bottom pixel is 0,0)
    void
    setInputRect( const QRectF & rect );

    /// set various options for grid drawing
    /// this is a temporary method, and it is dependent on AST lib
    /// \todo Replace this with implementation independent API
    void
    setPlotOption( const QString & option );

    /// set line thickness
//    void setLineThickness( double t);

    /// perform the actual plot on the image
    /// returns success/failure
    bool
    plot();

    /// return the error message
    QString
    getError();

    /// static function to set text and line colors
    static void
    setLineColor( QString color );

    static void
    setTextColor( QString color );

protected:

    bool m_carLin = false;
    QString m_errorString;
    QString m_fitsHeader;
    QStringList m_plotOptions;
    QString m_system;
    QRectF m_orect, m_irect;
    QImage * m_img = nullptr;
//    double m_lineThickness = 1.0;

    VGComposer * m_vgc = nullptr;
};
}
