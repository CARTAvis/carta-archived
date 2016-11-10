/**
 *
 **/

#include "../CartaLib.h"
#include "BetterQPainter.h"
#include <QMetaType>
#include <QImage>
#include <QStringList>
#include <QPainter>
#include <QFontInfo>

#pragma once

namespace Carta
{
namespace Lib
{
namespace VectorGraphics
{
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
/// reset the state of painter to defaults
class Reset : public IVGListEntry
{
    CLASS_BOILERPLATE( Reset);
public:
    virtual void cplusplus(BetterQPainter & painter) override
    {
        painter.reset();
    }
    virtual QStringList javascript() override
    {
        return QStringList()
               << QString( "p.reset();" );
    }
};

/// line entry implementation
class DrawLine : public IVGListEntry
{
    CLASS_BOILERPLATE( DrawLine );

public:

    DrawLine( const QPointF & p1, const QPointF & p2 )
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

/// polyline entry implementation
class DrawPolyline : public IVGListEntry
{
    CLASS_BOILERPLATE( DrawPolyline );

public:

    DrawPolyline( const QPolygonF & poly )
    {
        m_poly = poly;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.drawPolyline( m_poly );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.polyline(not implemented yet);" );
    }

private:

    QPolygonF m_poly;
};



/// polyline entry implementation
class DrawPolygon : public IVGListEntry
{
    CLASS_BOILERPLATE( DrawPolygon );

public:

    DrawPolygon( const QPolygonF & poly )
    {
        m_poly = poly;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.drawPolygon( m_poly );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.polygon(not implemented yet);" );
    }

private:

    QPolygonF m_poly;
};
/// set pen width implementation
class SetPenWidth : public IVGListEntry
{
    CLASS_BOILERPLATE( SetPenWidth );

public:

    SetPenWidth( double width )
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

/// set pen color implementation
class SetPenColor : public IVGListEntry
{
    CLASS_BOILERPLATE( SetPenColor );

public:

    SetPenColor( QColor color )
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

/// set pen entry
class SetPen : public IVGListEntry
{
    CLASS_BOILERPLATE( SetPen );

public:

    SetPen( const QPen & pen )
    {
        m_pen = pen;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setPen( m_pen );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.setPenColor('%1');" )
                   .arg( m_pen.color().name( QColor::HexArgb ) )
               << QString( "p.setPenWidth(%1);" )
                   .arg( m_pen.width() );
    }

private:

    QPen m_pen = QPen( QColor( 255, 255, 255 ) );
};

/// set font entry
class SetFontIndex : public IVGListEntry
{
    CLASS_BOILERPLATE( SetFontIndex );

public:

    SetFontIndex( int fontIndex )
    {
        m_fontIndex = fontIndex;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setFontIndex( m_fontIndex );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.setFont('%1');" )
                   .arg( m_fontIndex );
    }

private:

    int m_fontIndex = 0;
};

/// set fontSize entry
class SetFontSize : public IVGListEntry
{
    CLASS_BOILERPLATE( SetFontSize );

public:

    SetFontSize( double size )
    {
        m_size = size;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setFontSize( m_size );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.setFontSize('%1');" )
                   .arg( m_size );
    }

private:

    double m_size = 0;
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

/// draw a rectangle filled with current brush and outlined with current pen
class DrawRect : public IVGListEntry
{
    CLASS_BOILERPLATE( DrawRect );

public:

    DrawRect( const QRectF & rect)
    {
        m_rect = rect;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.drawRect( m_rect);
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.drawRect(%1,%2,%3,%4);" )
                   .arg( m_rect.left() )
                   .arg( m_rect.right() )
                   .arg( m_rect.width() )
                   .arg( m_rect.height() );
    }

private:

    QRectF m_rect = QRectF( 0, 0, 10, 10 );
};

/// draw an ellipse filled with current brush and outlined with current pen
class DrawEllipse : public IVGListEntry
{
    CLASS_BOILERPLATE( DrawEllipse );

public:

    DrawEllipse( const QRectF & rect)
    {
        m_rect = rect;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.drawEllipse( m_rect);
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.drawEllipse(%1,%2,%3,%4);" )
                   .arg( m_rect.left() )
                   .arg( m_rect.right() )
                   .arg( m_rect.width() )
                   .arg( m_rect.height() );
    }

private:

    QRectF m_rect = QRectF( 0, 0, 10, 10 );
};

/// draw text
class DrawText : public IVGListEntry
{
    CLASS_BOILERPLATE( DrawText );

public:

    DrawText( QString text, const QPointF & pos = QPointF( 0, 0 ) )
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
        /// \todo implement escaping of quotes, so that it's possible
        /// to pass in a string containing quotes...
        QString escapedText = m_text;

        return QStringList()
               << QString( "p.drawText(%1,%2,%3);" )
                   .arg( escapedText )
                   .arg( m_pos.x() )
                   .arg( m_pos.y() );
    }

private:

    QString m_text = "";
    QPointF m_pos = QPointF( 0, 0 );
};

/// stores a pen at a given index
class StoreIndexedPen : public IVGListEntry
{
    CLASS_BOILERPLATE( StoreIndexedPen );

public:

    StoreIndexedPen( int ind, const QPen & pen )
    {
        m_ind = ind;
        m_pen = pen;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.storeIndexedPen( m_ind, m_pen );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.storeIndexedPen(%1,%2,%3);" )
                   .arg( m_ind )
                   .arg( m_pen.color().name( QColor::HexArgb ) )
                   .arg( m_pen.widthF() );
    }

private:

    int m_ind = 0;
    QPen m_pen;
};

/// uses a previously indexed pen
class SetIndexedPen : public IVGListEntry
{
    CLASS_BOILERPLATE( SetIndexedPen );

public:

    SetIndexedPen( int ind )
    {
        m_ind = ind;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setIndexedPen( m_ind );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.setIndexedPen(%1);" )
                   .arg( m_ind );
    }

private:

    int m_ind = 0;
};

/// stores a brush at a given index
class StoreIndexedBrush : public IVGListEntry
{
    CLASS_BOILERPLATE( StoreIndexedBrush );

public:

    StoreIndexedBrush( int ind, const QBrush & brush )
    {
        m_ind = ind;
        m_brush = brush;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.storeIndexedBrush( m_ind, m_brush );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.storeIndexedBrush(%1,%2);" )
                   .arg( m_ind )
                   .arg( m_brush.color().name( QColor::HexArgb ) );
    }

private:

    int m_ind = 0;
    QBrush m_brush;
};

/// uses a previously indexed brush
class SetIndexedBrush : public IVGListEntry
{
    CLASS_BOILERPLATE( SetIndexedBrush );

public:

    SetIndexedBrush( int ind )
    {
        m_ind = ind;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setIndexedBrush( m_ind );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.setIndexedBrush(%1);" )
                   .arg( m_ind );
    }

private:

    int m_ind = 0;
};

/// uses a previously indexed brush
class SetBrush : public IVGListEntry
{
    CLASS_BOILERPLATE( SetBrush );

public:

    SetBrush( const QBrush & brush )
    {
        m_brush = brush;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.setBrush( m_brush );
    }

    virtual QStringList
    javascript() override
    {
        return QStringList()
               << QString( "p.setBrush(%1);" )
                  .arg( m_brush.color().name( QColor::HexArgb ) );
    }

private:

    QBrush m_brush { "black" };
};
}


class VGComposer;

/// container for vector graphics with enough APIs to rasterize it/convert it to PDF/EPS
class VGList
{
public:

    /// make an empty list
    VGList();

    /// copy constructor
    VGList( const VGList & other ) = default;

    /// assignment operator
    VGList &
    operator= ( const VGList & other ) = default;

    ~VGList();

    /// get a list of entries (ie. list of shared pointers to entries)
    const std::vector < IVGListEntry::SharedPtr > &
    entries() const { return m_entries; }

    /// get a list of entries (ie. list of shared pointers to entries)
    /// with read/write access
//    std::vector < IVGListEntry::SharedPtr > &
//    rwEntries() { return m_entries; }

private:

    /// VGComposer has write access to the list of entries
    friend class VGComposer;

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

    /// constructor that starts with an empty VGList
    VGComposer() {}

    /// constructor that starts with the supplied VGList
    VGComposer( const VGList & vgList) {
        m_vgList = vgList;
    }

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

    /// append another list
    void appendList( const VGList & vglist)
    {
        m_vgList.m_entries.insert( m_vgList.m_entries.end(),
                                   vglist.m_entries.begin(),
                                   vglist.m_entries.end());
    }

    /// clear all entries
    void
    clear()
    {
        m_vgList.m_entries.resize( 0 );
    }

private:

    VGList m_vgList;
};
}
}
}

Q_DECLARE_METATYPE( Carta::Lib::VectorGraphics::VGList )
