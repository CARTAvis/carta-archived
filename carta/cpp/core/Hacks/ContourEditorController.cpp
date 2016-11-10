#include "ContourEditorController.h"
#include "core/Globals.h"
#include <functional>

/*
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
*/

namespace Carta
{

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

    QStringList initialText;
    initialText << "# comment out the 'level' lines to turn on the contours"
                << "## specify color"
                << "color blue"
                << "## set width"
                << "width 3"
                << "## and alpha"
                << "alpha 0.5"
                << "## set desired level(s)"
                << "#level 0 1"
                << "##change width and alpha"
                << "w 1"
                << "a 1"
                << "## add another contour"
                << "#level 0.5"
                << "## yet another color, this time hex"
                << "c #ff0000"
                << "#l -1";
    QString s = initialText.join("\n").toLatin1().toBase64();

    SetupVar( m_stringVar, SS::StringVar,
              "text", s);

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
ContourEditorController::setInput( Carta::Lib::NdArray::RawViewInterface::SharedPtr rawView )
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

    if( m_pens.size() != result.contours().size()) {
        qCritical() << "conour set entries:" << result.contours().size()
                    << "but pen entries:" << m_pens.size();
        return;
    }

    // convert the raw contours into VG
    Carta::Lib::VectorGraphics::VGComposer vgc;
    const auto & contourSet = result.contours();
    for ( size_t k = 0 ; k < contourSet.size() ; ++k ) {
        const auto & con = contourSet[k].polylines();
        vgc.append< VGE::SetPen >( m_pens[k]);
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

    std::vector<double> levels;
    m_pens.resize( 0);
    double width = 1.0;
    QColor color = QColor( "green");
    // make lines
    QStringList lines = text.split( "\n", QString::SkipEmptyParts);
    for( auto & line : lines) {
        line = line.simplified();
        bool lineError = false;

        QStringList e = line.split( " ");
        if( e[0] == "level" || e[0] == "l") {
            for( int i = 1 ; i < e.length() ; ++ i) {
                bool ok;
                double v = e[i].toDouble( & ok);
                if( ! ok) {
                    lineError = true;
                    continue;
                }
                levels.push_back( v);
                QPen pen( QColor( color), width);
                pen.setCosmetic( true);
                m_pens.push_back( pen);
            }
        }
        else if( e[0] == "color" || e[0] == "c") {
            if( e.length() != 2) {
                lineError = true;
            } else {
                color.setNamedColor( e[1]);
                if( ! color.isValid()) {
                    lineError = true;
                }
            }
        }
        else if( e[0] == "alpha" || e[0] == "a") {
            if( e.length() != 2) {
                lineError = true;
            } else {
                bool ok;
                double alpha = e[1].toDouble( & ok);
                if( ! ok || ! ( alpha >= 0 && alpha <= 1)) {
                    lineError = true;
                } else {
                    color.setAlphaF( alpha);
                }
            }
        }
        else if( e[0] == "width" || e[0] == "w") {
            if( e.length() != 2) {
                lineError = true;
            } else {
                bool ok;
                width = e[1].toDouble( & ok);
                if( ! ok || ! ( width >= 0 && width <= 100)) {
                    lineError = true;
                }
            }
        }
        else if( line.startsWith( "#")) {
            // comment, skip it
        }
        else {
            lineError = true;
        }

        if( lineError) {
            qWarning() << "Error parsing contour UI:" << line;
        }
    }
    m_contourSvc-> setLevels( levels);
    emit updated();

//    text.replace( ',',' ');
//    text = text.simplified();
//    m_contourSvc-> setLevels( Impl::s2vd( text));

//    // let everyone know something was updated
//    emit updated();
}
}

}
