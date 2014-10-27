#include "Viewer.h"
#include "Globals.h"
#include "IPlatform.h"
//#include "IConnector.h"
//#include "Algorithms/RawView2QImageConverter.h"
#include "State/ObjectManager.h"
#include "Data/ViewManager.h"

#include "misc.h"
#include "PluginManager.h"
#include "MainConfig.h"
#include "MyQApp.h"
#include "CmdLine.h"
#include "ScriptedCommandListener.h"
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QDebug>
#include <QCache>
#include <QCoreApplication>
#include <cmath>
#include <iostream>
#include <limits>






/*class TestView : public IView
{
public:
    TestView( const QString & viewName, QColor bgColor )
    {
        m_qimage = QImage( 100, 100, QImage::Format_RGB888 );
        m_qimage.fill( bgColor );

        m_viewName  = viewName;
        m_connector = nullptr;
        m_bgColor   = bgColor;
    }

    virtual void
    registration( IConnector * connector )
    {
        m_connector = connector;
    }

    virtual const QString &
    name() const
    {
        return m_viewName;
    }

    virtual QSize
    size()
    {
        return m_qimage.size();
    }

    virtual const QImage &
    getBuffer()
    {
        redrawBuffer();
        return m_qimage;
    }

    virtual void
    handleResizeRequest( const QSize & pSize )
    {
        QSize size( std::max( pSize.width(), 1), std::max( pSize.height(), 1));
        m_qimage = QImage( size, m_qimage.format() );
        m_connector-> refreshView( this );
    }

    virtual void
    handleMouseEvent( const QMouseEvent & ev )
    {
        m_lastMouse = QPointF( ev.x(), ev.y() );
        m_connector-> refreshView( this );

		m_connector-> setState( StateKey::MOUSE_X, m_viewName, QString::number(ev.x()));
		m_connector-> setState( StateKey::MOUSE_Y, m_viewName, QString::number(ev.y()));
	}

    virtual void
    handleKeyEvent( const QKeyEvent & event )
    { }

protected:
    QColor m_bgColor;

    void
    redrawBuffer()
    {
        QPointF center = m_qimage.rect().center();
        QPointF diff   = m_lastMouse - center;
        double angle   = atan2( diff.x(), diff.y() );
        angle *= - 180 / M_PI;

        m_qimage.fill( m_bgColor );
        {
            QPainter p( & m_qimage );
            p.setPen( Qt::NoPen );
            p.setBrush( QColor( 255, 255, 0, 128 ) );
            p.drawEllipse( QPoint( m_lastMouse.x(), m_lastMouse.y() ), 10, 10 );
            p.setPen( QColor( 255, 255, 255 ) );
            p.drawLine( 0, m_lastMouse.y(), m_qimage.width() - 1, m_lastMouse.y() );
            p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_qimage.height() - 1 );

            p.translate( m_qimage.rect().center() );
            p.rotate( angle );
            p.translate( - m_qimage.rect().center() );
            p.setFont( QFont( "Arial", 20 ) );
            p.setPen( QColor( "white" ) );
            p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName );
        }

        // execute the pre-render hook
        Globals::instance()-> pluginManager()
            -> prepare < PreRender > ( m_viewName, & m_qimage )
            .executeAll();
    } // redrawBuffer

    IConnector * m_connector;
    QImage m_qimage;
    QString m_viewName;
    int m_timerId;
    QPointF m_lastMouse;
};*/





/*class TestView2 : public IView
{
public:
    TestView2( const QString & viewName, QColor bgColor, QImage img )
    {
        m_defaultImage = img;
        m_qimage       = QImage( 100, 100, QImage::Format_RGB888 );
        m_qimage.fill( bgColor );

        m_viewName  = viewName;
        m_connector = nullptr;
        m_bgColor   = bgColor;
    }

    void
    setImage( const QImage & img )
    {
        m_defaultImage = img;
        m_connector-> refreshView( this );
    }

    virtual void
    registration( IConnector * connector )
    {
        m_connector = connector;
    }

    virtual const QString &
    name() const
    {
        return m_viewName;
    }

    virtual QSize
    size()
    {
        return m_qimage.size();
    }

    virtual const QImage &
    getBuffer()
    {
        redrawBuffer();
        return m_qimage;
    }

    virtual void
    handleResizeRequest( const QSize & pSize )
    {
        QSize size( std::max( pSize.width(), 1), std::max( pSize.height(), 1));
        m_qimage = QImage( size, m_qimage.format() );
        m_connector-> refreshView( this );
    }

    virtual void
    handleMouseEvent( const QMouseEvent & ev )
    {
        m_lastMouse = QPointF( ev.x(), ev.y() );
        m_connector-> refreshView( this );

		m_connector-> setState( StateKey::MOUSE_X, m_viewName, QString::number(ev.x()));
		m_connector-> setState( StateKey::MOUSE_Y, m_viewName, QString::number(ev.y()));
	}

    virtual void
    handleKeyEvent( const QKeyEvent & event )
    { }

protected:
    QColor m_bgColor;
    QImage m_defaultImage;

    void
    redrawBuffer()
    {
        QPointF center = m_qimage.rect().center();
        QPointF diff   = m_lastMouse - center;
        double angle   = atan2( diff.x(), diff.y() );
        angle *= - 180 / M_PI;

        m_qimage.fill( m_bgColor );
        {
            QPainter p( & m_qimage );
            p.drawImage( m_qimage.rect(), m_defaultImage );
            p.setPen( Qt::NoPen );
            p.setBrush( QColor( 255, 255, 0, 128 ) );
            p.drawEllipse( QPoint( m_lastMouse.x(), m_lastMouse.y() ), 10, 10 );
            p.setPen( QColor( 255, 255, 255 ) );
            p.drawLine( 0, m_lastMouse.y(), m_qimage.width() - 1, m_lastMouse.y() );
            p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_qimage.height() - 1 );

            p.translate( m_qimage.rect().center() );
            p.rotate( angle );
            p.translate( - m_qimage.rect().center() );
            p.setFont( QFont( "Arial", 20 ) );
            p.setPen( QColor( "white" ) );
            p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName );
        }

        // execute the pre-render hook
        Globals::instance()-> pluginManager()
            -> prepare < PreRender > ( m_viewName, & m_qimage )
            .executeAll();
    } // redrawBuffer

    IConnector * m_connector;
    QImage m_qimage;
    QString m_viewName;
    int m_timerId;
    QPointF m_lastMouse;
};
*/

//static TestView2 * testView2 = nullptr;

Viewer::Viewer() :
    QObject( nullptr ),
    m_viewManager( nullptr)
{
    int port = Globals::instance()->cmdLineInfo()-> scriptPort();
    qDebug() << "Port="<<port;
    if ( port < 0 ) {
        qDebug() << "Not listening to scripted commands.";
    }
    else {
        m_scl = new ScriptedCommandListener( port, this );
        qDebug() << "Listening to scripted commands on port " << port;
        connect( m_scl, & ScriptedCommandListener::command,
                 this, & Viewer::scriptedCommandCB );
    }
    /*qDebug() << "Viewer constructor";
    m_rawView2QImageConverter = std::make_shared<RawView2QImageConverter>();
    qDebug() << "Viewer made converter";
    m_rawView2QImageConverter-> setAutoClip( 0.95); //95%
    qDebug() << "Set auto clip";*/
}

void
Viewer::start()
{
    qDebug() << "Viewer::start() starting";

    auto & globals = * Globals::instance();

    // initialize plugin manager
    globals.setPluginManager( new PluginManager );
    auto pm = globals.pluginManager();

    // tell plugin manager where to find plugins
    pm-> setPluginSearchPaths( globals.mainConfig()->pluginDirectories() );

    // find and load plugins
    pm-> loadPlugins();

    qDebug() << "Loading plugins...";
    auto infoList = pm-> getInfoList();
    qDebug() << "List of plugins: [" << infoList.size() << "]";
    for ( const auto & entry : infoList ) {
        qDebug() << "  path:" << entry.json.name;
    }

    // tell all plugins that the core has initialized
    pm-> prepare < Initialize > ().executeAll();

#ifdef DONT_COMPILE

    // associate a callback for a command
    connector->addCommandCallback( "debug",
                                   [] ( const QString & cmd, const QString & params,
                                        const QString & sessionId ) -> QString {
                                       std::cerr << "lambda command cb:\n"
                                                 << " " << cmd << "\n"
                                                 << " " << params << "\n"
                                                 << " " << sessionId << "\n";
                                       return "1";
                                   }
                                   );

    // associate a callback for a command
    connector->addCommandCallback(
        "add",
        [] ( const QString & /*cmd*/, const QString & params,
             const QString & /*sessionId*/ ) -> QString {
            std::cerr << "add command:\n"
                      << params << "\n";
            QStringList lst = params.split( " " );
            double sum = 0;
            for ( auto & entry: lst ) {
                bool ok;
                sum += entry.toDouble( & ok );
                if ( ! ok ) {
                    sum = - 1;
                    break;
                }
            }
            return QString( "add(%1)=%2" ).arg( params ).arg( sum );
        }
        );

//    auto xyzCBid =
    connector-> addStateCallback( "/xyz", [] ( const QString & path, const QString & val ) {
                                      qDebug() << "lambda state cb:\n"
                                               << "  path: " << path << "\n"
                                               << "  val:  " << val;
                                  }
                                  );

//    connector->removeStateCallback(xyzCBid);

    static const QString varPrefix = "/myVars";
    static int pongCount           = 0;
    connector-> addStateCallback(
        varPrefix + "/ping",
        [ = ] ( const QString & path, const QString & val ) {
            std::cerr << "lcb: " << path << "=" << val << "\n";
            QString nv = QString::number( pongCount++ );
            connector-> setState( varPrefix + "/pong", nv );
        }
        );
    connector-> setState( "/xya", "hola" );
    connector-> setState( "/xyz", "8" );

#endif // dont compile

    // create some views to be rendered on the client side
    //m_connector-> registerView( new TestView( "view1", QColor( "blue" ) ) );
    //m_connector-> registerView( new TestView( "view2", QColor( "red" ) ) );



	// ask plugins to load the image
	qDebug() << "======== trying to load image ========";
	//QString fname = Globals::fname();
	QString fname;
	if( ! Globals::instance()-> platform()-> initialFileList().isEmpty()) {
		fname = Globals::instance()-> platform()-> initialFileList() [0];
	}

    // ask one of the plugins to load the image
	/*if ( fname.length() > 0 ){
	    qDebug() << "Trying to load astroImage...";
	    auto res2 =Globals::instance()-> pluginManager()-> prepare < LoadAstroImage > ( fname ).first();
        if ( ! res2.isNull() ) {
	        qDebug() << "Could not find any plugin to load astroImage";
	        m_image = res2.val();

	        qDebug() << "Pixel type = " << Image::pixelType2int( res2.val()-> pixelType() );
	        testView2 = new TestView2( "view3", QColor( "pink" ), QImage(10, 10, QImage::Format_ARGB32) );
	        m_connector-> registerView( testView2 );

	        // convert the loaded image into QImage
	        m_currentFrame = 0;
	        reloadFrame( true);
//        delete frameView;
	    }

	    if( 0) {
	        // some debugging info

            Image::ImageInterface::SharedPtr img = res2.val();

	        qDebug() << "Dimensions: " << QVector <int>::fromStdVector( img->dims() );
	        qDebug() << "Unit: " << img-> getPixelUnit().toStr();
	        // let's get a view (the whole image)
	        NdArray::RawViewInterface * rawView = img-> getDataSlice( SliceND({Slice1D().step(3)}));
	        if( ! rawView) {
	            qFatal( "Raw view is null");
	        }
	        qDebug() << "View dimensions:" << rawView->dims();
	        typedef std::vector<int> VI;
	        VI pos( img->dims().size(), 0);
	        const char * rawPtr = rawView->get( pos);
	        for( size_t i = 0 ; i < sizeof( double) ; i ++ ) {
	            qDebug() << static_cast<int>(rawPtr[i]);
	        }
	        NdArray::TypedView<float> fv(rawView, false);
	        pos = VI( img->dims().size(), 0);
	        qDebug() << "fview @" << pos << "=" << fv.get( pos);
	        pos[0] = 1;
	        qDebug() << "fview @" << pos << "=" << fv.get( pos);

	        {
	            // [3:7:2,5:6]
	            auto slice = SliceND().start(3).step(2).end(7)
                         .next().start(5).end(6);
	            NdArray::RawViewInterface * rawView = img-> getDataSlice( slice);
	            NdArray::TypedView<float> fv(rawView, true);
	            qDebug().nospace();
	            int64_t count = 0;
	            fv.forEach([&count]( const float & val) {
	                qDebug() << val << " ";
	                count ++;
	            });
	            qDebug() << "foreach processed " << count << "items";
	            qDebug().space();
	        }

	        if( false){
	            // [0:5,0:5,0,0]
	            auto slice = SliceND().start(0).end(5)
                         .next().start(0).end(5)
                         .next().index(0)
                         .next().index(0);
	            NdArray::RawViewInterface * rawView = img-> getDataSlice( slice);
	            NdArray::TypedView<float> fv(rawView, true);
	            qDebug().nospace();
	            int64_t count = 0;
	            fv.forEach([&count]( const float & val) {
	                qDebug() << val << " ";
	                count ++;
	            });
	            qDebug() << "foreach processed " << count << "items";
	            qDebug().space();
	        }
	    }
	}*/

    ObjectManager* objManager = ObjectManager::objectManager();
    QString vmId = objManager->createObject (ViewManager::CLASS_NAME);
    CartaObject* vmObj = objManager->getObject( vmId );
    m_viewManager.reset( dynamic_cast<ViewManager*>(vmObj));
    fname = "/scratch/Images/Orion.cont.image.fits";
    if ( fname.length() > 0 ){
        //reloadFrame( true);
        m_viewManager->loadFile( fname );
    }

	qDebug() << "Viewer has started...";
}




void
Viewer::scriptedCommandCB( QString command )
{
    command = command.simplified();
    qDebug() << "Scripted command received:" << command;

    QStringList args = command.split( ' ', QString::SkipEmptyParts );
    qDebug() << "args=" << args;
    qDebug() << "args.size=" << args.size();
    if ( args.size() == 2 && args[0].toLower() == "load" ) {
        qDebug() << "Trying to load" << args[1];
        auto loadImageHookHelper = Globals::instance()->pluginManager()->
                    prepare < LoadImage >(args[1], 0);
        Nullable <QImage> res = loadImageHookHelper.first();
        if ( res.isNull() ){
            qDebug() << "Could not find any plugin to load image";
        }
        else {
            qDebug() << "Image loaded: "<< res.val().size();
            //testView2->setImage( res.val() );
        }
    }
    else if ( args.size() == 1 && args[0].toLower() == "quit" ) {
        qDebug() << "Quitting...";
        MyQApp::exit();
        return;
    }
    else {
        qWarning() << "Sorry, unknown command";
    }
}



/*void Viewer::reloadFrame( bool forceClipRecompute)
{
    qDebug() << "realodFrame m_image=" << m_image.get();
    auto frameSlice = SliceND().next();
    for( size_t i = 2 ; i < m_image->dims().size() ; i ++) {
        frameSlice.next().index( i == 2 ? m_currentFrame : 0);
    }
    NdArray::RawViewInterface * frameView = m_image-> getDataSlice( frameSlice);
    m_rawView2QImageConverter-> setView( frameView);
    QImage qimg = m_rawView2QImageConverter-> go(
                      m_currentFrame, m_clipRecompute || forceClipRecompute);
    delete frameView;
    //testView2->setImage(qimg);

}*/

void Viewer::mouseCB(const QString & /*path*/, const QString & /*val*/)
{
/*    bool ok;
    double x = m_connector-> getState( StateKey::MOUSE_X, "").toDouble( & ok);
    if( !ok) {}
    double y = m_connector-> getState( StateKey::MOUSE_Y, "").toDouble( & ok);
    if( !ok) {}
    auto pixCoords = std::vector<double>(m_image->dims().size(), 0.0);
    pixCoords[0] = x;
    pixCoords[1] = y;
    if( pixCoords.size() > 2) {
        pixCoords[2] = m_currentFrame;
    }
    auto list = m_coordinateFormatter->formatFromPixelCoordinate( pixCoords);
    qDebug() << "Formatted coordinate:" << list;
    m_connector-> setState( StateKey::CURSOR, "", list.join("\n").toHtmlEscaped());
*/
} // scriptedCommandCB
