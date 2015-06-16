/**
 *
 **/

#pragma once

#include "CartaLib/IImage.h"
#include "CartaLib/CartaLib.h"
#include <QObject>
#include <QPolygonF>

class IContourGeneratorService : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IContourGeneratorService );

public:

    typedef int64_t JobId;

    typedef std::vector < std::vector < QPolygonF > > Result;

    virtual void
    setLevels( const std::vector < double > & levels ) = 0;

    virtual void
    setInput( NdArray::RawViewInterface::SharedPtr rawView ) = 0;

    /// \brief start the job
    /// \param jobId what id to assign to job, if -1, it'll be auto-generated
    /// \return the jobId of the job
    ///
    virtual JobId
    start( JobId jobId = - 1 ) = 0;

//    IContourGeneratorService( QObject * parent = nullptr )
//        : QObject( parent ) { }

    virtual
    ~IContourGeneratorService() { }

signals:

    void done( const Result &, JobId );
};

