/**
 *    Implementation of the core's grid rendering APIs using Starlink's AST library.
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/ICoordinateGridPlotter.h"
#include <QColor>
#include <QObject>
#include <QTimer>

namespace WcsPlotterPluginNS
{

class AstGridPlotter;

/// implementation of Carta::Lib::IWcsGridRenderService APIs
class AstWcsGridRenderService : public Carta::Lib::IWcsGridRenderService
{
    Q_OBJECT
    CLASS_BOILERPLATE( AstWcsGridRenderService );

public:

    AstWcsGridRenderService();

    ~AstWcsGridRenderService();

    virtual void
    setInputImage( Image::ImageInterface::SharedPtr image ) override;

    virtual void
    setOutputSize( const QSize & size ) override;

    virtual void
    setImageRect( const QRectF & rect ) override;

    virtual void
    setOutputRect( const QRectF & rect ) override;

    virtual JobId
    startRendering( JobId jobId = 0) override;

    virtual void
    setAxesVisible( bool flag ) override;

    virtual void
    setGridDensityModifier( double density ) override;

    virtual void
    setGridLinesVisible( bool flag ) override;

    virtual void
    setInternalLabels(bool flag ) override;

    virtual void
    setSkyCS( Carta::Lib::KnownSkyCS cs ) override;

    virtual void
    setPen( Element e, const QPen & pen ) override;

//    virtual const QPen &
//    pen( Element e ) override;

    virtual void
    setFont( Element e, int fontIndex, double pointSize) override;

    virtual void
    setEmptyGrid( bool flag) override;

    virtual void
    setTicksVisible( bool flag ) override;

private slots:

    // internal slot - does the actual rendering
    void renderNow();

    // part of a hack to simulate delayed signal
//    void
//    reportResult();

//    void
//    dbgSlot();

private:
    //Don't draw tick marks.
    void _turnOffTicks(WcsPlotterPluginNS::AstGridPlotter* sgp);

    Carta::Lib::VectorGraphics::VGComposer m_vgc;
//    VGList m_vgList;

    Image::ImageInterface::SharedPtr m_iimage = nullptr;
    QRectF m_imgRect, m_outRect;
    QSize m_outSize = QSize( 10, 10 );
    QColor m_lineColor = QColor( "yellow" );
    double m_gridDensity = 0.5;
    bool m_internalLabels = false;
    bool m_emptyGridFlag = true;

    struct Pimpl;
    std::unique_ptr < Pimpl > m_pimpl; // = nullptr;
    // shortcut to pimpl
    inline Pimpl &
    m();

    // we use this timer inside startRendering() to delay the actual rendering
    // by a fraction of a second, which allows us to call startRendering() multiple
    // times, but only one rendering will really go through...
    QTimer m_renderTimer;

    // part of a hack to simulate delayed signal
    std::unique_ptr < QTimer > m_dbgTimer = nullptr;

    // flag to indicate whether the currently stored VGlist is valid
    bool m_vgValid = false;
    // flag for whether or not to show grid lines.
    bool m_gridLines = true;
    // flag to indicate whether to draw the axes/border.
    bool m_axes = true;
    // flag to indicate whether or not to draw tick marks.
    bool m_ticks = true;

    bool m_axisLabels = false;

    // another debug timer
//    QTimer m_dbgTimer2;
//    double m_dbgAngle = 0.0;
};
}
