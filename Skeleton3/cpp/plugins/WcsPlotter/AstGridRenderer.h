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
/// implementation of Carta::Lib::IWcsGridRenderer APIs
class WcsGridRendererAst : public Carta::Lib::IWcsGridRenderer
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
    setLineColor( QColor color ) override;

    virtual void
    startRendering() override;

private slots:

    // part of a hack to simulate delayed signal
    void reportResult();

    void dbgSlot();

private:

    VGList m_vgList;
    Image::ImageInterface::SharedPtr m_iimage = nullptr;
    QRectF m_imgRect, m_outRect;
    QSize m_outSize = QSize( 10, 10);
    QColor m_lineColor = QColor( "yellow");

    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl; // = nullptr;

    // part of a hack to simulate delayed signal
    std::unique_ptr<QTimer> m_dbgTimer = nullptr;

    // another debug timer
    QTimer m_dbgTimer2;
    double m_dbgAngle = 0.0;

};
}
