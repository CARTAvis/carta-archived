/**
 * Hook loading astronomical images.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include <QImage>
#include <QColor>
#include <vector>
#include <functional>
#include <memory>

namespace Carta
{
namespace Lib
{
typedef QImage VectorGraphics;

/// Synchronous API for world coordinate renderer
class IWcsGridRenderer : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IWcsGridRenderer );

public:

    virtual void
    setInputImage( Image::ImageInterface::SharedPtr image ) = 0;

    virtual void
    setOutputSize( const QSize & size ) = 0;

    virtual void
    setImageRect( const QRectF & rect ) = 0;

    virtual void
    setOutputRect( const QRectF & rect ) = 0;

    virtual void
    setLineColor( QColor color) = 0;

    virtual void
    startRendering() = 0;

    virtual
    ~IWcsGridRenderer();

signals:

    void
    done( VectorGraphics & vg );

//    virtual void
//    startDrawing();

//    typedef std::shared_ptr < VectorGraphics > VGSharedPtr;

//    /// \brief callback type
//    ///
//    typedef std::function < void ( bool, VGSharedPtr ) > Callback;
//    virtual void
//    setCallback( Callback cb ) = 0;
};

namespace Hooks
{
/// draw a grid on top of an image
class DrawWcsGridHook : public BaseHook
{
    CARTA_HOOK_BOILER1( DrawWcsGridHook )

public:

    typedef QImage ResultType;
    struct Params {
        Params( Image::ImageInterface::SharedPtr image )
        {
            m_astroImage = image;
        }

        Image::ImageInterface::SharedPtr m_astroImage;
        QSize outputSize;

        /// casa pixel coordinates of the region of the image for which to draw
        /// the grid for
        QRectF frameRect;

        /// screen coordinates for the output (should match up with frameRect above)
        QRectF outputRect;
        QStringList options;
    };

    DrawWcsGridHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        // force instantiation of templates
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};

/// get an instance of an synchronous grid renderer
class GetWcsGridRendererHook : public BaseHook
{
    CARTA_HOOK_BOILER1( GetWcsGridRendererHook );

public:

    /// the result of this hook is a pointer to a render object
    typedef IWcsGridRenderer::SharedPtr ResultType;

    /// no input parameters are needed
    struct Params { };

    GetWcsGridRendererHook( Params * pptr )
        : BaseHook( staticId )
          , paramsPtr( pptr )
    {
        // force instantiation of templates
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
