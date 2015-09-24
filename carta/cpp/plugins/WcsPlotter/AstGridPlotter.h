/// c++ code for plotting WCS grids using AST library
///
///     http://starlink.eao.hawaii.edu/starlink/AST
///
/// This is essentially my attempt to make a simple C++ interface for interacting with AST,
/// at least for drawing grids.
///
/// \warning This class is very unfriendly to multithreading because it uses
/// ASTlib's plotting, which I could only get to work using global variables.
///
/// \todo there is a way to tell AST to use a different graphics driver without
/// resorting to globals, but I feel it introduces a small performance penalty.
/// One day we should investigate whether it's actually signifficant penalty.

#pragma once

#include "CartaLib/VectorGraphics/VGList.h"
#include <QSize>
#include <QString>
#include <QRectF>
#include <QStringList>
#include <QFont>

class QImage;
class AstFrame;
class AstFrameSet;

namespace WcsPlotterPluginNS
{
///
/// Renders a wcs grid to a vector graphics composer using starlink's AST library.
///
class AstGridPlotter
{
public:

    /// short type alias
    typedef Carta::Lib::VectorGraphics::VGComposer VGComposer;

    AstGridPlotter();
    ~AstGridPlotter();

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

    void
    setAxisPermutation( std::vector<int>& perm );

    /// read/write access to indexed fonts
    std::vector<QFont> & fonts() { return m_qfonts; }

    /// read/write access to pens
    std::vector<QPen> & pens() { return m_pens; }

    /// set shadow color index
    void setShadowPenIndex( int penIndex) { m_shadowPenIndex = penIndex; }

    /// set various options for grid drawing
    /// this is a temporary method, and it is dependent on AST lib
    /// \todo Replace this with implementation independent API
    void
    setPlotOption( const QString & option );

    void setDensityModifier( double dm) {
        m_densityModifier = dm;
    }

    /// perform the actual plot on the image
    /// returns success/failure
    bool
    plot();

    /// return the error message
    QString
    getError();

protected:

    bool m_carLin = false;
    QString m_errorString;
    QString m_fitsHeader;
    QStringList m_plotOptions;
    QString m_system;
    QRectF m_orect, m_irect;
    std::vector<QPen> m_pens;
    std::vector<QFont> m_qfonts;
    std::vector<int> m_axisPerms;
    double m_densityModifier = 1.0;

    int m_shadowPenIndex;
//    QPen m_shadowPen = QPen( QColor( 0, 0, 0, 0), 1);

    VGComposer * m_vgc = nullptr;

private:
    AstFrame* _permuteAxes( AstFrameSet* frameSet ) const;
};
}
