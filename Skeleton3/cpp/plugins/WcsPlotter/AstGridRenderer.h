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
class WcsGridRendererAst : public Carta::Lib::IWcsGridRenderService
{
    Q_OBJECT
    CLASS_BOILERPLATE( WcsGridRendererAst );

public:

    WcsGridRendererAst();

    ~WcsGridRendererAst();

    virtual void
    setInputImage( Image::ImageInterface::SharedPtr image ) override;

    virtual void
    setOutputSize( const QSize & size ) override;

    virtual void
    setImageRect( const QRectF & rect ) override;

    virtual void
    setOutputRect( const QRectF & rect ) override;

    virtual void
    setLineThickness( double thickness ) override;

    virtual void
    startRendering() override;

    virtual double
    lineThickness() override;

    virtual void
    setLineColor( QColor color ) override;

    virtual QColor
    lineColor() override;

    virtual void
    setGridDensity( double density) override;

    virtual void
    setInternalLabels( bool on) override;

private slots:

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
    double m_lineThickness = 1.0;
    double m_gridDensity = 0.5;
    bool m_internalLabels = false;

    struct Pimpl;
    std::unique_ptr < Pimpl > m_pimpl; // = nullptr;

    // part of a hack to simulate delayed signal
    std::unique_ptr < QTimer > m_dbgTimer = nullptr;

    // another debug timer
    QTimer m_dbgTimer2;
    double m_dbgAngle = 0.0;
};
}
