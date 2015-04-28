/**
 *
 **/

#include "../CartaLib.h"
#include <QMetaType>
#include <QImage>
#include <QStringList>
#include <QPainter>

#pragma once

namespace Carta
{
namespace Lib
{
namespace VectorGraphics
{
/// our QPainter on stereoids
class BetterQPainter
{
public:

    BetterQPainter( QPainter & qPainter )
        : m_qPainter( qPainter )
    {
        m_qPainter.setPen( QPen( QColor( "red" ), 1 ) );
        m_qPainter.setFont( QFont( "Helvetica", 10));
    }

    /// draw a line from point p1 to p2
    void
    drawLine( const QPointF & p1, const QPointF & p2 )
    {
        m_qPainter.drawLine( p1, p2 );
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
    fillRect( const QRectF & rect, const QColor & color) {
        m_qPainter.fillRect( rect, color);
    }

    void drawText( QString text, QPointF pos) {
        m_qPainter.drawText( pos, text);
    }

private:

    QPainter & m_qPainter;
};

/// api for an entry in a VGList
class IVGListEntry
{
    CLASS_BOILERPLATE( IVGListEntry );

public:

    /// an entry needs to be able to render itself into our qpainter
    virtual void
    cplusplus( BetterQPainter & painter ) = 0;

    /// an entry needs to be able to render itself on javascript side as well
    virtual QStringList
    javascript() = 0;

    virtual
    ~IVGListEntry() { }
};

namespace Entries
{
/// line entry implementation
class Line : public IVGListEntry
{
    CLASS_BOILERPLATE( Line );

public:

    Line( const QPointF & p1, const QPointF & p2 )
    {
        m_p1 = p1;
        m_p2 = p2;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.drawLine( m_p1, m_p2 );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.line(%1,%2,%3,%4);" )
                   .arg( m_p1.x() ).arg( m_p1.y() ).arg( m_p2.x() ).arg( m_p2.y() );
    }

private:

    QPointF m_p1, m_p2;
};

/// pen width implementation
class PenWidth : public IVGListEntry
{
    CLASS_BOILERPLATE( PenWidth );

public:

    PenWidth( double width )
    {
        m_width = width;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setPenWidth( m_width );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.setPenWidth(%1);" )
                   .arg( m_width );
    }

private:

    double m_width = 1.0;
};

/// pen width implementation
class PenColor : public IVGListEntry
{
    CLASS_BOILERPLATE( PenColor );

public:

    PenColor( QColor color )
    {
        m_color = color;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setPenColor( m_color );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.setPenColor('%1');" )
                   .arg( m_color.name( QColor::HexArgb ) );
    }

private:

    QColor m_color = QColor( 255, 255, 255 );
};

/// save the state of the painter
class Save : public IVGListEntry
{
    CLASS_BOILERPLATE( Save );

public:

    Save()
    { }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.save();
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.save();" );
    }

private:
};

/// restore the state of the painter
class Restore : public IVGListEntry
{
    CLASS_BOILERPLATE( Restore );

public:

    Restore()
    { }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.restore();
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.restore();" );
    }

private:
};

/// set a transform
class SetTransform : public IVGListEntry
{
    CLASS_BOILERPLATE( SetTransform );

public:

    SetTransform( const QTransform & transform, bool combine = false )
    {
        m_transform = transform;
        m_combine = combine;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setTransform( m_transform, m_combine );
    }

    virtual QStringList
    javascript() override
    {
        QString matrix = "1,23,5,6,7";
        return QStringList()
               << QString( "p.setTransform(%1,%2);" )
                   .arg( matrix ).arg( m_combine );
    }

private:

    QTransform m_transform;
    bool m_combine = false;
};

/// draw a filled rectangle
class FillRect : public IVGListEntry
{
    CLASS_BOILERPLATE( FillRect );

public:

    FillRect( const QRectF & rect, const QColor & color )
    {
        m_rect = rect;
        m_color = color;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.fillRect( m_rect, m_color );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.fillRect(%1,%2,%3,%4,%5);" )
                   .arg( m_rect.left() )
                   .arg( m_rect.right() )
                   .arg( m_rect.width() )
                   .arg( m_rect.height() )
                   .arg( m_color.name( QColor::HexArgb ) );
    }

private:

    QRectF m_rect = QRectF( 0, 0, 10, 10 );
    QColor m_color = QColor( 255, 0, 0, 128 );
};

/// draw text
class DrawText : public IVGListEntry
{
    CLASS_BOILERPLATE( DrawText );

public:

    DrawText( QString text, const QPointF & pos = QPointF(0,0) )
    {
        m_text = text;
        m_pos = pos;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.drawText( m_text, m_pos );
    }

    virtual QStringList
    javascript() override
    {
        /// \todo implement escaping
        QString escapedText = m_text;

        return QStringList()
               << QString( "p.drawText(%1,%2,%3);" )
                   .arg( escapedText)
                   .arg( m_pos.x() )
                   .arg( m_pos.y() );
    }

private:

    QString m_text = "";
    QPointF m_pos = QPointF( 0, 0);
};

}

class VGComposer;

/// container for vector graphics with enough APIs to rasterize it/convert it to PDF/EPS
class VGList
{
public:

    VGList();

    /// copy constructor
//    VGList( const VGList & other) {
//        m_entries = other.m_entries;
//        for( size_t i = 0 ; i < m_entries.size() ; i ++ ) {

//        }
//    };
    /// assignment operator
//    VGList & operator= (const VGList& other) = delete;

    ~VGList();

//    void
//    setQImage( QImage img );

//    const QImage &
//    qImage() const;

    const std::vector < IVGListEntry::SharedPtr > &
    entries() const { return m_entries; }

private:

    friend class VGComposer;

//    QImage m_qImage;

    /// list of entries
    /// we use shared pointers to simplify memory management/copy constructor/assignments/etc
    std::vector < IVGListEntry::SharedPtr > m_entries;
};

/// this class offers functionality to render a VG list onto a qpainter
class VGListQPainterRenderer
{
public:

    ///
    /// \brief renders the given vgList to a qPainter
    /// \param vgList the vector graphics to render
    /// \param qPainter where to render it
    /// \return true on success
    ///
    bool
    render( const VGList & vgList, QPainter & qPainter );
};

/// this is the class you want to use to create vector graphics
class VGComposer
{
public:

    ///
    /// \brief returns the vector graphics list
    /// \return the vector graphics list
    ///
    const VGList &
    vgList() const { return m_vgList; }

    /// append an entry, ownership is transferred to this container
    int64_t
    appendEntry( IVGListEntry * entry )
    {
        m_vgList.m_entries.push_back( std::shared_ptr < IVGListEntry > ( entry ) );
        return m_vgList.m_entries.size() - 1;
    }

    /// set a specific entry to something else
    /// old entry will be deleted!
    void
    setEntry( int64_t ind, IVGListEntry * entry )
    {
        CARTA_ASSERT( ind >= 0 && size_t( ind ) < m_vgList.m_entries.size() );
        m_vgList.m_entries[ind].reset( entry );
    }

    /// templated version of appendEntry
    template < typename EntryType, typename ... Args >
    int64_t
    append( Args && ... params )
    {
        return appendEntry( new EntryType( std::forward < Args > ( params ) ... ) );
    }

    /// templated version of setEntry()
    template < typename EntryType, typename ... Args >
    void
    set( int64_t ind, Args && ... params )
    {
        return setEntry( ind, new EntryType( std::forward < Args > ( params ) ... ) );
    }

private:

    VGList m_vgList;
};
}
}
}

Q_DECLARE_METATYPE( Carta::Lib::VectorGraphics::VGList )
