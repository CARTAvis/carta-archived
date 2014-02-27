/***
 * The original idea behind this class was to implement the non-gui controller type
 * functionality for the entire application.
 *
 * The GUI was supposed to be implemented by either a subclass (yuck) or another
 * controller (currently called FvController). The reality is that most of new
 * functionality just went to the FvController...
 *
 */

// TODO: total redesign of FvContoller and FitsViewerServer is needed


#ifndef FITSVIEWERSERVER_H
#define FITSVIEWERSERVER_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QSize>
#include <QDir>
#include <QFileInfoList>
#include <QStringList>
#include <QList>

#include "common.h"
#include "ColormapFunction.h"
#include "CachedImageReader.h"
#include "WcsHelper.h"

namespace RaiLib { namespace Cache {
class RaiCacherService;
}};



class FitsViewerServer : public QObject
{
Q_OBJECT
public:

    explicit FitsViewerServer(QObject *parent = 0);
    ~FitsViewerServer();
    bool error();
    QString errorString();

signals:

    void cachedColormapChanged();

public slots:

public:

    QStringList getFileList( const QString & dirName);
    bool loadFitsFile( const FitsFileLocation & );
    bool resize( const QSize & size);
    bool loadFrame( int frame);
    const QImage & renderCurrentFrame();
//    const QImage & renderFrame( int frame);
    const QSize size() const { return renderBuffer_.size (); }

    // set panning
    void setDrawOrigin( QPointF);

    // set scaling
    void setDrawWidth( double width);

    // suggest centering parameters
    void getCentered( QPointF & origin, double & width);

    // set histogram parameters
    void setHistogram( double min, double max);

    // set colormap function
    void setColormap( ColormapFunction cmap);

    // get value at given image coordinates
    double getValue( int x, int y, int z);

    // return a reference to the parser
    inline FitsParser & parser()
    {
        if( m_cireader.isNull ()) {
            dbg(1) << "void parser is getting used\n";
            return m_invalidParser;
        } else {
            return m_cireader->parser ();
        }
    }

    // return a reference to the cached colormap
    const CachedRgbFunction & getCachedColormap() const;

    // return pointer to cached image reader
    inline QSharedPointer< CachedImageReader > cir() { return m_cireader; }

    const FitsParser::HeaderInfo &  getHeaderInfo();
    RaiLib::HistogramInfo getCurrentFrameInfo();
    RaiLib::HistogramInfo getFrameInfo( int frame);
    int currentFrame() const { return currentFrame_; }
    const FitsFileLocation & getFitsLocation() const { return fLocation_; }
    const QPointF & getDrawOrigin() const { return drawOrigin_; }
    double getDrawWidth() const { return drawWidth_; }
    QSize getRenderSize() const { return renderBuffer_.size(); }



    /*
    // returns a profile along X axis (z is a frame)
    const QList<double> & getProfileX( int x, int y, int z);
    // returns a profile along Y axis (z is a frame)
    const QList<double> & getProfileY( int x, int y, int z);
    // returns a profile along Z axis (z is a frame)
    const QList<double> & getProfileZ( int x, int y, int z);
    */

    /// formats a value with units
    QString formatValue( double val);
    // format the cursor
    QStringList formatCursor( double x, double y, double frame, bool withLabels = true);
    /// get the current wcs, empty string if unknown
    QString getWCS();
    /// set the wcs (follow up with getWCS to see what was actually set)
    void setWCS( const QString & s);
    // format coordinates for the given cursor
    QString formatCoordinate( double x, double y);

    /// queues up a file for caching
    void queueForCacher( const FitsFileLocation & floc, bool now = false);

    /// return the value unit
    QString valueUnit();

    /// return the total flux density unit
    QString totalFluxDensityUnit();

    /// get the current wcs hero pointer
    WcsHero::Hero::SharedPtr wcsHero();


protected:

    bool loadIfNeeded();

    FitsParser m_invalidParser;
    // cache controller
    QSharedPointer< RaiLib::Cache::Controller > m_cc;
    // cached image reader for the current file
    QSharedPointer< CachedImageReader > m_cireader;
    // cacher service
    QSharedPointer< RaiLib::Cache::RaiCacherService > m_cacherService;
    QImage fullFrameImage_, renderBuffer_;
    bool needsReload_;

    // attributes affecting the rendering
    FitsFileLocation fLocation_;
    int currentFrame_;
    QPointF drawOrigin_;
    double drawWidth_;
    double histMin_, histMax_;
    ColormapFunction cmap_;
    HistogramColormapFunctor hcMap_;
    CachedRgbFunction m_cachedHcMap;

    /// TODO: deprecate
    WcsHelper * m_wcsHelper;

    /// wcshero instance for formatting coordinate related stuff
    std::shared_ptr< WcsHero::Hero > m_wcsHero;

    Q_DISABLE_COPY( FitsViewerServer)

};

#endif // FITSVIEWERSERVER_H
