/**
 *    Implementation of the core's grid rendering APIs using Starlink's AST library.
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/ICoordinateGridPlotter.h"
#include <QColor>
#include <QObject>

namespace WcsPlotterPluginNS
{
/// implementation of Carta::Lib::IWcsGridRenderer APIs
class AstGridRenderer : public Carta::Lib::IWcsGridRenderer
{
    Q_OBJECT
    CLASS_BOILERPLATE( AstGridRenderer );

public:

    AstGridRenderer();

    ~AstGridRenderer();

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

private:

    VGList m_vgList;
    Image::ImageInterface::SharedPtr m_iimage = nullptr;
};
}
