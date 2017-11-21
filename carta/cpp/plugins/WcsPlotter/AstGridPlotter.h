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
#include "CartaLib/AxisDisplayInfo.h"
#include <QSize>
#include <QString>
#include <QRectF>
#include <QStringList>
#include <QFont>
#include <QObject>

#include <QThread>

class QImage;
class AstFrame;
class AstFrameSet;

namespace WcsPlotterPluginNS
{
///
/// Renders a wcs grid to a vector graphics composer using starlink's AST library.
///
class AstGridPlotter: public QObject
{
    Q_OBJECT

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
    /*void
    setSystem( const QString & system = QString() );*/

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
    QString getError();

    static QThread astThread;
//    static int test;

signals:
    void startPlotSignal();
    void plotResultSignal();

public slots:
    void startPlotSlot();

protected:

    bool m_carLin = false;
    QString m_errorString;
    QString m_fitsHeader;
    QStringList m_plotOptions;
    //QString m_system;
    QRectF m_orect, m_irect;
    std::vector<QPen> m_pens;
    std::vector<QFont> m_qfonts;

    double m_densityModifier = 1.0;

    int m_shadowPenIndex;
//    QPen m_shadowPen = QPen( QColor( 0, 0, 0, 0), 1);

    VGComposer * m_vgc = nullptr;

private:

    /*
    *  Purpose:
    *     Create a FrameSet describing 2 axes of a 3-d FrameSet.

    *  Description:
    *     This function returns a FrameSet which describes a 2-d slice
    *     through a cube. A FrameSet describing the cube must be supplied -
    *     the base Frame must represent pixel coordinates and the current
    *     Frame must represent world coordinates. The current Frame must
    *     contain a pair of celestial longitude and latitude axes, although
    *     the third axis can be anything (time, spectral position, etc). The
    *     required slice must span the non-celestial axis and one of the
    *     celestial axes.
    *
    *     The returned Frame has a 2-d base Frame respresenting pixel coordinates
    *     within the extracted slice, and a 2-d current Frame respresenting the
    *     selected world coordinates (the non-celstial axis plus one of the
    *     celestial axes).
    *
    *     The returned celestial axis must vary monotonically along the
    *     slice, otherwise an error will be reported. For instance, it is not
    *     possible to use this function to extract a (Dec,time) slice from
    *     an (RA,Dec,Time) cube if the slice passes close to a pole, since
    *     in such cases Dec will not vary monotonically (it will take its
    *     greatest value at the point on the slice that is closest to the pole).
    *
    *     The returned FrameSet contains two 1-d Mappings in parallel - one
    *     transforms the non-celestial pixel values, and the other is a
    *     look-up-table that gives the required celestial axis value as a
    *     function of pixel position along the slice.
    *  Parameters:
    *  @param fs - Pointer to the supplied FrameSet.
    *  @param opaxis - 1-based index of retained celestial pixel axis.
    *  @param owaxis - 1-based index of retained celestial world axis.
    *  @param zpaxis - 1-based index of non-celestial pixel axis.
    *  @param zwaxis - 1-based index of non-celestial world axis.
    *  @param paxisval - an array of pixel axis values (one for each axis) at which the slice is to be taken.
    *           The value will be -1 for the two axis that are not sliced (i.e., display axes).
    *  @param naxis - An array holding the number of pixel along each pixel axis of
    *        the cube.
    *  @param axisCount - the dimensions of the cube and the size of paxisval and naxis.
    *  @param celestialFirst - true if the retained celestial axis should be displayed horizontally; false
    *       otherwise.
    *  @return - a pointer to the 2-d FrameSet, or NULL if an error occurs.
    */
    //Please note that the code in this method has been adapted from code donated by Dave Berry,
    //who supports the AST Library.
    AstFrameSet * _make2dFrameSet( AstFrameSet *fs,
            int opaxis, int owaxis, int zpaxis, int zwaxis,
            int* paxisvals, int* naxis, int axisCount, bool celestialFirst);

    /**
     * Decides which worker function to call based on the characteristics of the frameset.
     * @param wcsinfo - the base frameset created from the image.
     * @return - a pointer to the 2-d display frame set, or NULL if an error occurs.
     */
    AstFrameSet * _make2dFrame( AstFrameSet* wcsinfo );

    /**
     * Entry point for creating a 2-d frameset that consists of one celestial axis and one
     * non-celestial axis.
     * @param frameSet - the base frameset created from the image.
     * @return - a pointer to the 2-d display frameset, or NULL if an error occurs.
     */
    AstFrameSet* _make2dFrameCelestialExclude( AstFrameSet* frameSet );

};


}
