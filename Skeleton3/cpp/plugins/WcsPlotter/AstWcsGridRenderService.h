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

//    virtual void
//    setLineThickness( double thickness ) override;

    virtual void
    startRendering() override;

//    virtual double
//    lineThickness() override;

//    virtual void
//    setLineColor( QColor color ) override;

//    virtual QColor
//    lineColor() override;

    virtual void
    setGridDensityModifier( double density ) override;

    virtual void
    setInternalLabels( bool on ) override;

    virtual void
    setSkyCS( Carta::Lib::KnownSkyCS cs ) override;

    virtual void
    setPen( Element e, const QPen & pen ) override;

    virtual const QPen &
    pen( Element e ) override;

    virtual void
    setFont( Element e, int fontIndex, double pointSize) override;

//    virtual const QFont &
//    font( Element e ) override;

    virtual void
    setShadowColor( const QColor & color ) override;

    virtual const QColor &
    shadowColor() override;

private slots:

    // internal slot - does the actual rendering
    void renderNow();

    // part of a hack to simulate delayed signal
    void
    reportResult();

    void
    dbgSlot();

private:

    VGList m_vgList;
    Image::ImageInterface::SharedPtr m_iimage = nullptr;
    QRectF m_imgRect, m_outRect;
    QSize m_outSize = QSize( 10, 10 );
    QColor m_lineColor = QColor( "yellow" );
    double m_gridDensity = 0.5;
    bool m_internalLabels = false;

    struct Pimpl;
    std::unique_ptr < Pimpl > m_pimpl; // = nullptr;
    // shortcut to pimpl
    inline Pimpl &
    m();

    // the render timer
    QTimer m_renderTimer;

    // part of a hack to simulate delayed signal
    std::unique_ptr < QTimer > m_dbgTimer = nullptr;

    // another debug timer
    QTimer m_dbgTimer2;
    double m_dbgAngle = 0.0;
};
}
