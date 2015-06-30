#include "ContourEditorController.h"
#include "common/Globals.h"
#include <functional>


namespace Impl
{
/// convert string to array of doubles
static
std::vector < double >
s2vd( QString s, QString sep = " " )
{
    QStringList lst = s.split( sep );
    std::vector < double > res;
    for ( auto v : lst ) {
        bool ok;
        double val = v.toDouble( & ok );
        if ( ! ok ) {
            return res;
        }
        res.push_back( val );
    }
    return res;
}
}

namespace Hacks
{
ContourEditorController::ContourEditorController( QObject * parent,
                                                  QString statePrefix
                                                  ) : QObject( parent )
{
    auto & globals = * Globals::instance();
    m_connector = globals.connector();
    m_statePrefix = statePrefix;
    if ( ! m_statePrefix.endsWith( '/' ) ) {
        m_statePrefix.append( '/' );
    }

    // create the contour calculation service and hook it up
    m_contourSvc.reset( new Carta::Core::DefaultContourGeneratorService( this ) );
    connect( m_contourSvc.get(), & Carta::Core::DefaultContourGeneratorService::done,
             this, & Me::contourServiceCB );

    const SS::FullPath prefix = SS::FullPath::fromQString( m_statePrefix );

#define SetupVar( vname, type, path, initValue ) \
    vname.reset( new type( prefix.with( path ) ) ); \
    vname-> set( initValue ); \
    connect( vname.get(), & type::valueChanged, this, & Me::stdVarCB )

    SetupVar( m_stringVar, SS::StringVar,
              "text", "");

#undef SetupVar

    stdVarCB();
}

ContourEditorController::JobId
ContourEditorController::startRendering( ContourEditorController::JobId jobId )
{
    if ( jobId < 0 ) {
        m_lastJobId++;
    }
    else {
        m_lastJobId = jobId;
    }

    (void) m_contourSvc-> start( m_lastJobId );

    return m_lastJobId;
}

void
ContourEditorController::setInput( NdArray::RawViewInterface::SharedPtr rawView )
{
    m_contourSvc-> setInput( rawView );
}

namespace VGE = Carta::Lib::VectorGraphics::Entries;

void
ContourEditorController::contourServiceCB(
    const Carta::Lib::IContourGeneratorService::Result & result,
    ContourEditorController::JobId jobId )
{
    if ( jobId != m_lastJobId ) {
        // old result, we don't need this anymore
        return;
    }

    // convert the raw contours into VG
    Carta::Lib::VectorGraphics::VGComposer vgc;
    const auto & contourSet = result.contours();
    for ( size_t k = 0 ; k < contourSet.size() ; ++k ) {
        const auto & con = contourSet[k].polylines();
        for ( size_t i = 0 ; i < con.size() ; ++i ) {
            const QPolygonF & poly = con[i];
            vgc.append < VGE::DrawPolyline > ( poly );
        }
    }
    emit done( vgc.vgList(), m_lastJobId );
} // contourServiceCB

void
ContourEditorController::stdVarCB()
{
    QString text = QByteArray::fromBase64( m_stringVar-> get().toLatin1() );
    qDebug() << "raw string var:" << m_stringVar-> get();
    qDebug() << "decoded string var:" << text;

    text.replace( ',',' ');
    text = text.simplified();
    m_contourSvc-> setLevels( Impl::s2vd( text));

    // let everyone know something was updated
    emit updated();
}
}
