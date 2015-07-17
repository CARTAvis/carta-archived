/**
 *
 **/

#pragma once

#include <QPolygonF>
#include <QDebug>
#include <vector>

namespace Carta
{
namespace Lib
{
class Contour
{
public:

    /// returns the level for this contour
    double
    level() const
    {
        return m_level;
    }

    /// returns the list of polylines
    const std::vector < QPolygonF > &
    polylines() const
    {
        return m_polylines;
    }

    /// constructor
    Contour( double level, std::vector < QPolygonF > & polylines )
    {
        m_level = level;
        m_polylines = polylines;

//        m_constructed++;
//        qDebug() << "xContour" << m_constructed << "vs" << m_deleted;
    }

//    Contour( const Contour & other )
//    {
//        m_level = other.m_level;
//        m_polylines = other.m_polylines;

//        m_constructed++;
//        qDebug() << "xContour" << m_constructed << "vs" << m_deleted;
//    }

//    ~Contour()
//    {
//        m_deleted++;
//        qDebug() << "xContour" << m_constructed << "vs" << m_deleted;
//    }

//    Contour &
//    operator= ( const Contour & ) = delete;

private:

    double m_level = 0.0;
    std::vector < QPolygonF > m_polylines;

//    static int m_constructed;
//    static int m_deleted;
};

/// data type that holds generated contours
class ContourSet
{
public:

    const std::vector < Contour > &
    contours() const
    {
        return m_contours;
    }

    void
    add( const Contour & contour )
    {
        m_contours.push_back( contour );
    }

private:

    std::vector < Contour > m_contours;
};
}
}
