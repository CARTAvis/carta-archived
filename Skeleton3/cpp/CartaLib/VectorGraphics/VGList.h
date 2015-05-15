/**
 *
 **/

#include "../CartaLib.h"
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
/// our QPainter with an API that should be reproducible in HTML5
class BetterQPainter
{
public:

    class FontInfo
    {
    public:
        QString name() { return "some font"; }
    private:
        QString m_name;
        friend class BetterQPainter;
    };

    BetterQPainter( QPainter & qPainter )
        : m_qPainter( qPainter )
    {
        // setup default fonts
        m_fonts.push_back( QFont( "Helvetica", 10)); // <-- default
        m_fonts.push_back( QFont( "Monospace", 10));
        m_fonts.push_back( QFont( "Courier", 10));
        m_fonts.push_back( QFont( "Purisa", 10));

        // default pen
        m_qPainter.setPen( QPen( QColor( "red" ), 1 ) );
        // default font
        m_qPainter.setFont( m_fonts.front());
    }

    /// get info about the existing fonts
    std::vector<FontInfo> fontInfos() {
        std::vector<FontInfo> infos;
        for( QFont & font : m_fonts) {
            FontInfo finfo;
            finfo.m_name = QFontInfo( font).family();
            infos.push_back( finfo);
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

    void setFontIndex( int fontIndex) {
        qDebug() << "fontIndex" << fontIndex;
        CARTA_ASSERT( fontIndex >= 0 && fontIndex < int(m_fonts.size()));
        fontIndex = Carta::Lib::clamp<int>( fontIndex, 0, int(m_fonts.size() - 1));
        QFont font = m_fonts[fontIndex];
        font.setPointSizeF( m_qPainter.font().pointSizeF());
        m_qPainter.setFont( font);
    }

    void setFontSize( int size) {
        QFont f = m_qPainter.font();
        f.setPointSizeF( size);
        m_qPainter.setFont( f);
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
    std::vector<QFont> m_fonts;
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

    DrawPolyline( const QPolygonF & poly)
    {
        m_poly = poly;
    }

    virtual void
    cplusplus( BetterQPainter & painter ) override
    {
        painter.drawPolyline( m_poly);
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
        /// \todo implement escaping
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
