/**
 * Purpose of IContourGeneratorService is to define an API that
 *
 * a) can be implemented by plugins to offer a new countour calculating algorithms
 * b) can be used elsewhere (eg. by plugins) to generate contours
 *
 **/

#pragma once

#include "CartaLib/IImage.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/ContourSet.h"
#include <QObject>
#include <QPolygonF>

namespace Carta
{
namespace Lib
{
/*
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
    }

private:

    double m_level = 0.0;
    std::vector < QPolygonF > m_polylines;
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

    void add( const Contour & contour) {
        m_contours.push_back( contour);
    }

private:

    std::vector < Contour > m_contours;
};
*/

///
/// Purpose of IContourGeneratorService is to define an API that:
///
/// a) can be implemented by plugins that want to offer a new countour calculating algorithms
/// b) can be used elsewhere (by core or plugins) to generate contours
///
/// The result of the algorithm is just a list of polylines per requested level.
/// It is up to the caller to use this (to generate output files, render it, etc)
class IContourGeneratorService
    : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IContourGeneratorService );

public:

    typedef int64_t JobId;

//    typedef std::vector < std::vector < QPolygonF > > Result;
    typedef ContourSet Result;

    /// request name for which to generate the contours
    virtual void
    setName( const QString & name ) = 0;

    /// request levels for which to generate the contours
    virtual void
    setLevels( const std::vector < double > & levels ) = 0;

    /// set the input on which to generate the contours
    virtual void
    setInput( NdArray::RawViewInterface::SharedPtr rawView ) = 0;

    /// \brief start the job
    /// \param jobId what id to assign to job, if -1, it'll be auto-generated (0,1,2,...)
    /// \return the jobId of the job
    virtual JobId
    start( JobId jobId = - 1 ) = 0;

    IContourGeneratorService( QObject * parent = nullptr )
        : QObject( parent ) { }

    virtual
    ~IContourGeneratorService() { }

signals:

    /// emitted when the job is done
    /// \param result contains the contours
    /// \param jobId which jobid does this result correspond to
    void
    done( const Result & result, JobId jobId );
};
}
}

#include "VectorGraphics/VGList.h"
namespace Carta
{
namespace Lib
{
class Contour2VG
{
public:

    VectorGraphics::VGList
    convert( const IContourGeneratorService::Result & contours );
};
}
}
