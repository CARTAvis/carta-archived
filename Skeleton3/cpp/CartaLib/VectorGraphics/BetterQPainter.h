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
        qDebug() << "fontIndex" << fontIndex;
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

    void
    fillRect( const QRectF & rect, const QColor & color )
    {
        m_qPainter.fillRect( rect, color );
    }

    void
    drawText( QString text, QPointF pos )
    {
        m_qPainter.drawText( pos, text );
    }

private:

    QPainter & m_qPainter;
    std::vector < QFont > m_fonts;
};
}
}
}
