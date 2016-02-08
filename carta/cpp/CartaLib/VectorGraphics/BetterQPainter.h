/**
 * A rendering class that is very similar to QPainter (or at least it's design was strongly
 * motivated by QPainter's API). The difference is that it only contains APIs that can be
 * easily emulated in HTML5.
 *
 * This is the class that is used to convert VGList to anything Qt can render using QPainter.
 * In that sense this class offers an API that makes it easy to convert VGList entries to
 * rendering commands, and it then translates it to QPainter APIs. That way we can render
 * VGList to anything that QPainter can render to, i.e. QImage, PDF, Printer etc. At least
 * that is the theory :)
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include <QPainter>
#include <QString>
#include <QDebug>

namespace Carta
{
namespace Lib
{
namespace VectorGraphics
{
/// our QPainter with an API that should be reproducible in HTML5
class BetterQPainter
{
public:

    class FontInfo
    {
    public:

        QString
        name() { return "some font"; }

    private:

        QString m_name;
        friend class BetterQPainter;
    };

    BetterQPainter( QPainter & qPainter )
        : m_qPainter( qPainter )
    {
        auto oldHints = qPainter.renderHints();
        reset();
        qPainter.setRenderHints( oldHints);
    }

    void reset() {
        /// @todo is this the best way to reset QPainter to defaults?
        /// a documented way (and presumably preferred way) to do this would be via
        /// save/restore mechanism, could we do that?
        auto device = m_qPainter.device();
        m_qPainter.end();
        m_qPainter.begin(device);

        m_fonts.clear();
        m_indexedPens.clear();
        m_indexedBrushes.clear();

        // setup default fonts
        m_fonts.push_back( QFont( "Helvetica", 10 ) ); // <-- default
        m_fonts.push_back( QFont( "Monospace", 10 ) );
        m_fonts.push_back( QFont( "Courier", 10 ) );
        m_fonts.push_back( QFont( "Purisa", 10 ) );

        // default pen
        m_qPainter.setPen( QPen( QColor( "red" ), 1 ) );

        // default font
        m_qPainter.setFont( m_fonts.front() );
    }

    /// get info about the existing fonts
    std::vector < FontInfo >
    fontInfos()
    {
        std::vector < FontInfo > infos;
        for ( QFont & font : m_fonts ) {
            FontInfo finfo;
            finfo.m_name = QFontInfo( font ).family();
            infos.push_back( finfo );
        }
        return infos;
    }

    /// draw a line from point p1 to p2
    void
    drawLine( const QPointF & p1, const QPointF & p2 )
    {
        m_qPainter.drawLine( p1, p2 );
    }

    /// draw a polyline
    void
    drawPolyline( const QPolygonF & poly )
    {
        m_qPainter.drawPolyline( poly );
    }

    /// draw a polygon
    void
    drawPolygon( const QPolygonF & poly )
    {
        m_qPainter.drawPolygon( poly );
    }


    /// draw an ellipse
    void
    drawEllipse( const QRectF & rect)
    {
        m_qPainter.drawEllipse( rect);
    }

    /// set the width of lines
    void
    setPenWidth( double width )
    {
        QPen pen = m_qPainter.pen();
        pen.setWidth( width );
        m_qPainter.setPen( pen );
    }

    /// set the width of lines
    void
    setPenColor( const QColor & color )
    {
        QPen pen = m_qPainter.pen();
        pen.setColor( color );
        m_qPainter.setPen( pen );
    }

    void
    setPen( const QPen & pen )
    {
        m_qPainter.setPen( pen );
    }

    void
    setFontIndex( int fontIndex )
    {
        CARTA_ASSERT( fontIndex >= 0 && fontIndex < int ( m_fonts.size() ) );
        fontIndex = Carta::Lib::clamp < int > ( fontIndex, 0, int (m_fonts.size() - 1) );
        QFont font = m_fonts[fontIndex];
        font.setPointSizeF( m_qPainter.font().pointSizeF() );
        m_qPainter.setFont( font );
    }

    void
    setFontSize( int size )
    {
        QFont f = m_qPainter.font();
        f.setPointSizeF( size );
        m_qPainter.setFont( f );
    }

    /// save the state of the painter
    void
    save()
    {
        m_qPainter.save();
    }

    /// restore the state of the painter
    void
    restore()
    {
        m_qPainter.restore();
    }

    void
    setTransform( const QTransform & t, bool combine = false )
    {
        m_qPainter.setTransform( t, combine );
    }

    /// draw a rectangle filled with the given color, no outline
    void
    fillRect( const QRectF & rect, const QColor & color )
    {
        m_qPainter.fillRect( rect, color );
    }

    /// draw a rectangle filled with the current brush & outlined with current
    /// pen
    void
    drawRect( const QRectF & rect) {
        m_qPainter.drawRect( rect);
    }

    void
    drawText( QString text, QPointF pos )
    {
        m_qPainter.drawText( pos, text );
    }

    //

    /// \brief Store an indexed pen at position penId.
    /// \param penId at what position to store the pen
    /// \param pen pen to store
    /// \note The indices should be consecutive, starting at 0, as we use a simple
    /// array to store them.
    void
    storeIndexedPen( int penId, const QPen & pen)
    {
        CARTA_ASSERT( penId >= 0);
        if( penId >= int( m_indexedPens.size())) {
            if( CARTA_RUNTIME_CHECKS) {
                if( penId > 1000) {
                    qCritical() << "Are you sure you want " << penId << "pens???";
                }
            }
            m_indexedPens.resize( penId + 1);
        }
        m_indexedPens[ penId] = pen;
    }

    /// \brief Set the current pen to be an indexed pen.
    /// \param penId Which pen to use.
    void
    setIndexedPen( int penId)
    {
        if( CARTA_RUNTIME_CHECKS) {
            if( penId < 0 || penId >= int( m_indexedPens.size())) {
                qCritical() << "called setIndexedPen with" << penId << "but only have"
                            << m_indexedPens.size() << "pens.";
                return;
            }
        }
        setPen( m_indexedPens[ penId]);
    }

    /// \brief Store an indexed brush at position brushId.
    /// \param brushId at what position to store the brush
    /// \param brush brush to store
    /// \note The indices should be consecutive, starting at 0, as we use a simple
    /// array to store them.
    void storeIndexedBrush( int brushId, const QBrush & brush) {
        CARTA_ASSERT( brushId >= 0);
        if( brushId >= int( m_indexedBrushes.size())) {
            if( CARTA_RUNTIME_CHECKS) {
                if( brushId > 1000) {
                    qCritical() << "Are you sure you want " << brushId << "brushes???";
                }
            }
            m_indexedBrushes.resize( brushId + 1);
        }
        m_indexedBrushes[ brushId] = brush;
    }

    /// \brief Set the current brush to an indexed one.
    /// \param brushId Which indexed brush to use.
    void
    setIndexedBrush( int brushId)
    {
        if( CARTA_RUNTIME_CHECKS) {
            if( brushId < 0 || brushId >= int( m_indexedBrushes.size())) {
                qCritical() << "called setIndexedBrush with" << brushId << "but only have"
                            << m_indexedBrushes.size() << "brushes.";
                return;
            }
        }
        setBrush( m_indexedBrushes[ brushId]);
    }

    /// set a brush
    void setBrush( const QBrush & brush) {
        m_qPainter.setBrush( brush);
    }

private:

    QPainter & m_qPainter;
    std::vector < QFont > m_fonts;
    std::vector < QPen > m_indexedPens;
    std::vector < QBrush > m_indexedBrushes;
};
}
}
}
