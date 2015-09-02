/**
 *
 **/

#pragma once

#include "CartaLib/IWcsGridRenderService.h"

#include <QObject>
#include <QTimer>

namespace Carta
{
namespace Core
{
/// grid render service that does nothing except reports empty grid back
///
/// this can be useful if real grid render service is not available, as it can simplify
/// the logic...
class DummyGridRenderer
    : public Carta::Lib::IWcsGridRenderService
{
    Q_OBJECT
    CLASS_BOILERPLATE( DummyGridRenderer );

public:

    DummyGridRenderer();

    virtual void
    setInputImage( Image::ImageInterface::SharedPtr ) override;

    virtual void
    setImageRect( const QRectF & ) override;

    virtual void
    setOutputSize( const QSize & ) override;

    virtual void
    setOutputRect( const QRectF & ) override;

    virtual void
    setPen( Element, const QPen & ) override;

    virtual void
    setFont( Element, int, double ) override;

    virtual void
    setGridDensityModifier( double ) override;

    virtual void
    setGridLinesVisible( bool ) override;

    virtual void
    setAxesVisible( bool ) override;

    virtual void
    setInternalLabels( bool ) override;

    virtual void
    setSkyCS( Carta::Lib::KnownSkyCS ) override;

    virtual void
    setTicksVisible( bool ) override;

    virtual JobId
    startRendering( JobId jobId = - 1 ) override;

    virtual void
    setEmptyGrid( bool ) override;

private slots:

    void
    reportResult();

private:

    QTimer m_timer;
    JobId m_jobId = - 1;
};
}
}
