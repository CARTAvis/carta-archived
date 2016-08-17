/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/Nullable.h"
#include "CartaLib/Regions/ICoordSystem.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QRectF>

namespace Carta
{
namespace Lib
{
namespace Regions
{
static const QColor DEFAULT_LINE_COLOR = QColor( "pink" );
static const QColor DEFAULT_FILL_COLOR = QColor( "blue" );

/// for now we use 2D points for all region related operations, but this could
/// change in the future to add support fo 3D or even higher dimensional regions
typedef QPointF RegionPoint;
typedef std::vector< RegionPoint > RegionPointV;

class RegionBase;

RegionBase *
fromJson( QJsonObject json, RegionBase * parent = nullptr );

class RegionBase;

class RegionSet
{
public:

    /// returns the root region
    RegionBase *
    root() { return m_root; }

    /// sets the root
    void
    setRoot( RegionBase * region )
    {
        m_root = region;
    }

    /// test for intersection, the point must be specified as an array of points, one
    /// for each coordinate system (query this by nCoordSystems() & coordSystemConverter())
    bool
    isPointInside( const std::vector < RegionPoint > & pts );

    /// set the input coordinate system. Affects
    ///  isPointInside()
//    void
//    setInputCS( const CompositeCoordinateSystem & cs )
//    {
//        /// here we setup converters from cs to all our coordinate systems
//        Q_UNUSED( cs);
//    }

    /// returns how many coordinate systems are used in this region set
    int
    nCoordSystems() const
    {
        return m_coordSystems.size();
    }

    /// returns the coordinate system converter (i)
    ICoordSystemConverter &
    coordSystemConverter( int i )
    {
        CARTA_ASSERT( i >= 0 && i <= nCoordSystems() );
        return * m_coordSystems[i];
    }

    /// add a coordinate system converter (and return it's index)
    int
    addCoordSystemConverter( ICoordSystemConverter::SharedPtr cvt )
    {
        m_coordSystems.push_back( cvt );
        return m_coordSystems.size() - 1;
    }

private:

    /// the root region
    RegionBase * m_root = nullptr;

    /// list of coordinate systems in the set
    std::vector < ICoordSystemConverter::SharedPtr > m_coordSystems;
};

class RegionBase
{
    CLASS_BOILERPLATE( RegionBase );

public:

    /// textual name (for serialization/deserialization purposes)
    /// static constexpr auto TypeName = "None";

    virtual QString
    typeName() = 0;

    enum class RenderType
    {
        GUI, Print
    };

    //    virtual QString
    //    name() const { return Name; }

    RegionBase( RegionBase * parent = nullptr )
    {
        m_parent = nullptr;
        if ( parent ) { parent-> addChild( this ); }
    }

    virtual bool
    canHaveChildren() const { return false; }

    /// adds region as a child of this region
    void
    addChild( RegionBase * region )
    {
        if ( ! canHaveChildren() ) {
            qCritical() << "Cannot add kids to this region.";
            return;
        }
        m_kids.push_back( region );
        region-> setParent( this );

//        region-> m_parent = this;
    }

    /// return a list of all children
    const std::vector < RegionBase * > &
    children()
    {
        return m_kids;
    }

    /// returns the coordinate system of this region
    int
    coordSystem() const
    {
        return m_coordinateSystemID;
    }

    /// set the coordinate system of this region
    void
    setCoordSystem( int cs )
    {
        m_coordinateSystemID = cs;
    }

    /// initializes the object from json
    /// this should be called immediately after the object is created
    virtual bool
    initFromJson( QJsonObject json )
    {
        // get the line color
        QJsonValue lineColor = json["lineColor"];
        if ( lineColor.isString() && QColor::isValidColor( lineColor.toString() ) ) {
            m_lineColor = QColor( lineColor.toString() );
        }

        // get the fill color
        QJsonValue fillColor = json["fillColor"];
        if ( fillColor.isString() && QColor::isValidColor( fillColor.toString() ) ) {
            m_fillColor = QColor( fillColor.toString() );
        }
        else if ( fillColor.isBool() && fillColor.toBool() == false ) {
            m_fillColor = QColor( 0, 0, 0, 0 );
        }

        if ( json.contains( "kids" ) ) {
            QJsonValue jval = json["kids"];
            if ( ! jval.isArray() ) { return false; }
            QJsonArray jarr = jval.toArray();
            for ( const auto & js : jarr ) {
                auto kid = fromJson( js.toObject() );
                if ( ! kid ) {
                    return false;
                }
                addChild( kid );
            }
        }

        return true;
    } // initFromJson

    /// tests whether the point is inside this region, assuming the point is already
    /// in the in the same coordinate system as the region.
    ///
    /// for example circle would test distance from center, subtraction would test
    /// inside one but not the other, etc.
    ///
    /// default implementation returns false
    virtual bool
    isPointInside( const RegionPointV & /*p*/ )
    {
        return false;
    }

    /// tests whether the point is inside this region, but the point is specified
    /// using an array of points representing different coordinate systems
    ///
    /// default implementation returns false
    virtual bool
    isPointInsideCS( const RegionPointV & pts )
    {
        CARTA_ASSERT( int ( pts.size() ) > m_coordinateSystemID );
        return false;
    }

    /// tests whether the point is inside this region as if it was a union
    /// this is useful to implement interactive events, like testing if mouse click
    /// belongs to the region
    ///
    /// default implementation:
    ///   for regions with no kids it calls isPointInside()
    ///   for regions with kids, it calls the kids isPointInsideUnion() and returns
    ///   true if the point is inside any of the kids (hence the union)
    virtual bool
    isPointInsideUnion( const RegionPointV & pts )
    {
        if ( canHaveChildren() ) {
            // for group regions we delegate to kids
            for ( auto & kid : children() ) {
                if ( kid-> isPointInsideUnion( pts ) ) { return true; }
            }
            return false;
        }
        else {
            // for simple regions we do direct shape test
            return isPointInside( pts );
        }
    }

    /// returns an outline box for this region, i.e. the smallest rectangle that is
    /// big enough to render all these regions
    /// default behavior is to return a union of all childrens' outline boxes
    virtual QRectF
    outlineBox()
    {
        if ( m_kids.size() == 0 ) {
            return QRectF();
        }
        auto rect = m_kids[0]->outlineBox();
        for ( size_t i = 1 ; i < m_kids.size() ; ++i ) {
            rect = rect.united( m_kids[i]-> outlineBox() );
        }
        return rect;
    }

    /// returns a VG list of this region
    virtual VectorGraphics::VGList
    vgList()
    {
        VectorGraphics::VGComposer comp;
        for ( auto kid : m_kids ) {
            comp.appendList( kid-> vgList() );
        }
        return comp.vgList();
    }

    /// returns a list of rectangles that contain points in the region, non-overlapping
    /// you will need to scan all points in these rectangles to get a list of all
    /// points inside regions. This should be significantly faster than doing the same
    /// for the ouline box
    virtual std::vector < QRectF >
    boundingRects()
    {
        // default implementation is to return the outlineBox
        return { outlineBox() };
    }

    RegionBase *
    parent() { return m_parent; }

    /// set line color
    void
    setLineColor( const QColor & color )
    {
        m_lineColor = color;
    }

    /// get line color
    /// our own if defined
    /// otherwise parent's color (if parent defined)
    /// otherwise default color
    const QColor &
    getLineColor()
    {
        if ( m_lineColor.isSet() ) {
            return m_lineColor.val();
        }
        if ( m_parent ) {
            return m_parent-> getLineColor();
        }
        return DEFAULT_LINE_COLOR;
    }

    /// do we have our own line color
    bool
    hasOwnLineColor() const
    {
        return m_lineColor.isSet();
    }

    void
    setFillColor( QColor color )
    {
        m_fillColor = color;
    }

    QColor
    getFillColor()
    {
        if ( m_fillColor.isSet() ) {
            return m_fillColor.val();
        }
        if ( parent() ) {
            return parent()-> getFillColor();
        }
        return DEFAULT_FILL_COLOR;
    }

    bool
    hasOwnFillColor() const
    {
        return m_fillColor.isSet();
    }

    /// serialize to json
    virtual QJsonObject
    toJson()
    {
        QJsonObject json;

        // output line color if it's set, or this is root
        if ( m_lineColor.isSet() || parent() == nullptr ) {
            json["lineColor"] = getLineColor().name();
        }
        if ( m_fillColor.isSet() || parent() == nullptr ) {
            json["fillColor"] = getFillColor().name();
        }

        if ( m_kids.size() > 0 ) {
            QJsonArray arr;

            for ( auto kid : m_kids ) {
                arr.append( kid-> toJson() );
            }
            json["kids"] = arr;
        }

        return json;
    } // do_toJson

    /// return the coordinate system index
    int
    csId() const
    {
        return m_coordinateSystemID;
    }

    /// default behavior of the destructor is to delete all kids
    virtual
    ~RegionBase()
    {
        for ( auto & kid : m_kids ) {
            delete kid;
        }
        m_kids.resize( 0 );
    }

    static constexpr auto REGION_TYPE = "type";

protected:

    void
    setParent( RegionBase * parent )
    {
        if ( CARTA_RUNTIME_CHECKS && parent && m_parent != nullptr ) {
            qCritical() << "Setting parent to a new parent?!?!?";
        }
        m_parent = parent;
    }

private:

    RegionBase * m_parent = nullptr;
    Nullable < QColor > m_lineColor;
    Nullable < QColor > m_fillColor;

    std::vector < RegionBase * > m_kids;

    int m_coordinateSystemID = 0;

};

class Circle : public RegionBase
{
public:

    static constexpr auto TypeName = "circle";
    virtual QString
    typeName() override { return TypeName; }

    Circle( RegionBase * parent = nullptr ) : RegionBase( parent ) { }

    Circle( const RegionPoint & center, double radius )
    {
        m_center = center;
        m_radius = radius;
    }

    virtual bool
    isPointInside( const RegionPointV & pts ) override
    {
        const auto & p = pts[csId()];
        auto d = p - m_center;
        auto lsq = QPointF::dotProduct( d, d );
        return lsq < m_radius * m_radius;
    }

    virtual bool
    isPointInsideUnion( const RegionPointV & pts ) override
    {
        return isPointInside( pts );
    }

    virtual QRectF
    outlineBox() override
    {
        return QRectF( m_center.x() - m_radius, m_center.y() - m_radius,
                       m_radius * 2, m_radius * 2 );
    }

    virtual VectorGraphics::VGList
    vgList() override
    {
        VectorGraphics::VGComposer composer;

//        if ( hasOwnLineColor() ) {
        composer.append < VectorGraphics::Entries::SetPenColor > ( getLineColor() );
//        }
//        if ( hasOwnFillColor() ) {
        composer.append < VectorGraphics::Entries::SetBrush > ( getFillColor() );

//        }
        QRectF rect( m_center.x() - m_radius, m_center.y() - m_radius,
                     2 * m_radius, 2 * m_radius );
        composer.append < VectorGraphics::Entries::DrawEllipse > ( rect );

        return composer.vgList();
    } // vgList

    virtual QJsonObject
    toJson() override
    {
        // get the common properties
        QJsonObject doc = RegionBase::toJson();

        // and add our own
        doc["centerx"] = m_center.x();
        doc["centery"] = m_center.y();
        doc["radius"] = m_radius;
        doc["type"] = TypeName;

        return doc;
    }

    virtual bool
    initFromJson( QJsonObject obj ) override
    {
        if ( ! RegionBase::initFromJson( obj ) ) {
            return false;
        }
        if ( ! obj["centerx"].isDouble() ) {
            return false;
        }
        if ( ! obj["centery"].isDouble() ) {
            return false;
        }
        if ( ! obj["radius"].isDouble() ) {
            return false;
        }
        m_center = QPointF( obj["centerx"].toDouble(), obj["centery"].toDouble() );
        m_radius = obj["radius"].toDouble();
        return true;
    }

    const RegionPoint &
    center() const { return m_center; }

    void
    setCenter( const RegionPoint & pt )
    {
        m_center = pt;
    }

    double
    radius() const { return m_radius; }

    void
    setRadius( double radius ) { m_radius = radius; }

private:

    RegionPoint m_center { 0, 0 };
    double m_radius = 1;
};

class Polygon : public RegionBase
{
public:

    static constexpr auto TypeName = "polygon";
    static constexpr auto POINTS = "pts";
    static constexpr auto POINT_X = "x";
    static constexpr auto POINT_Y = "y";
    virtual QString
    typeName() override { return TypeName; }

    Polygon( RegionBase * parent = nullptr ) : RegionBase( parent ) { }

    virtual bool
    isPointInside( const RegionPointV & pts ) override
    {
        const auto & p = pts[csId()];
        return m_qpolyf.containsPoint( p, Qt::WindingFill );
    }

    virtual bool
    isPointInsideUnion( const RegionPointV & pts ) override
    {
        return isPointInside( pts );
    }

    virtual QRectF
    outlineBox() override
    {
        return m_qpolyf.boundingRect();
    }

    virtual VectorGraphics::VGList
    vgList() override
    {
        VectorGraphics::VGComposer composer;

//        if ( hasOwnLineColor() ) {
        composer.append < VectorGraphics::Entries::SetPenColor > ( getLineColor() );

//        }
//        if ( hasOwnFillColor() ) {
        composer.append < VectorGraphics::Entries::SetBrush > ( getFillColor() );

//        }
        composer.append < VectorGraphics::Entries::DrawPolygon > ( m_qpolyf );

        return composer.vgList();
    } // vgList

    virtual QJsonObject
    toJson() override
    {
        // get the common properties
        QJsonObject doc = RegionBase::toJson();

        // and add our own
        doc[POINTS] = QJsonArray();
        QJsonArray pts;
        for ( auto & pt : m_qpolyf ) {
            QJsonObject o;
            o[POINT_X] = pt.x();
            o[POINT_Y] = pt.y();
            pts.push_back( o );
        }
        doc[POINTS] = pts;
        doc[RegionBase::REGION_TYPE] = TypeName;

        return doc;
    } // toJson

    virtual bool
    initFromJson( QJsonObject obj ) override
    {
        if ( ! RegionBase::initFromJson( obj ) ) { return false; }
        if ( ! obj[POINTS].isArray() ) { return false; }
        QJsonArray pts = obj[POINTS].toArray();
        for ( const auto & jv : pts ) {
            QJsonObject o = jv.toObject();
            if ( ! o["x"].isDouble() ) { return false; }
            if ( ! o["y"].isDouble() ) { return false; }
            double x = o["x"].toDouble();
            double y = o["y"].toDouble();
            m_qpolyf.append( QPointF( x, y ) );
        }
        return true;
    }

    const QPolygonF &
    qpolyf() const { return m_qpolyf; }

    void
    setqpolyf( const QPolygonF & poly ) { m_qpolyf = poly; }

private:

    QPolygonF m_qpolyf;
};

class Union : public RegionBase
{
public:

    static constexpr auto TypeName = "union";
    virtual QString
    typeName() override { return TypeName; }

    Union( RegionBase * parent = nullptr ) : RegionBase( parent ) { }

    virtual bool
    canHaveChildren() const override { return true; }

    virtual bool
    isPointInside( const RegionPointV & pts ) override
    {
        // return true if the point is inside any of the kids
        for ( auto & kid : children() ) {
            if ( kid-> isPointInside( pts ) ) { return true; }
        }

        // otherwise it's not inside
        return false;
    }

    virtual QJsonObject
    toJson() override
    {
        QJsonObject obj = RegionBase::toJson();
        obj["type"] = TypeName;
        return obj;
    }
};
}
}
}
