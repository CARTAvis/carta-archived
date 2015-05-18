#include "AstGridPlotter.h"
#include <iostream>
#include "grfdriver.h"

extern "C" {
#include <ast.h>
};

namespace WcsPlotterPluginNS
{
AstGridPlotter::AstGridPlotter()
{
//    impl_ = new Impl;
    m_carLin = false;
}

AstGridPlotter::~AstGridPlotter()
{
//     delete impl_;
}

bool
AstGridPlotter::setFitsHeader( const QString & hdr )
{
    m_fitsHeader = hdr;

    return true;
}

void
AstGridPlotter::setCarLin( bool flag )
{
    m_carLin = flag;
}

void
AstGridPlotter::setSystem( const QString & system )
{
    m_system = system;
}

void
AstGridPlotter::setOutputVGComposer( AstGridPlotter::VGComposer * vgc )
{
    m_vgc = vgc;
}

void
AstGridPlotter::setOutputRect( const QRectF & rect )
{
    m_orect = rect;
}

void
AstGridPlotter::setInputRect( const QRectF & rect )
{
    m_irect = rect;
}

void
AstGridPlotter::setPlotOption( const QString & option )
{
    m_plotOptions.append( option );
}

//void AstGridPlotterQImage::setLineThickness(double t)
//{
//    m_lineThickness = t;
//}

struct AstGuard {
    AstGuard() { astBegin; }

    ~AstGuard() { astEnd; }
};

bool
AstGridPlotter::plot()
{
    astClearStatus;
    AstGuard astGuard;

    // copy over colors
    grfGlobals()-> colors = colors();

    // make sure we have at least 1 color
    grfGlobals()-> colors.push_back( QColor( "blue" ) );

//    // copy over fonts
//    grfGlobals()-> fonts = fonts();
//    // make sure we have at least 1 font
//    grfGlobals()-> fonts.push_back( QFont( "Helvetica", 10));
    // shadows are separate
    grfGlobals()-> lineShadowPen = m_shadowPen;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
    AstFitsChan * fitschan = astFitsChan( NULL, NULL, "" );
#pragma GCC diagnostic pop

    if ( ! fitschan ) {
        m_errorString = "astFitsChan returned null :(";
        return false;
    }

    std::string stdstr = m_fitsHeader.toStdString();
    astPutCards( fitschan, stdstr.c_str() );
    if ( m_carLin ) {
        astSet( fitschan, "CarLin=1" );
    }
    else {
        astSet( fitschan, "CarLin=0" );
    }

    AstFrameSet * wcsinfo = static_cast < AstFrameSet * > ( astRead( fitschan ) );

    if ( ! astOK ) {
        m_errorString = "Some AST LIB error, check logs.";
        return false;
    }
    else if ( wcsinfo == AST__NULL ) {
        m_errorString = "No WCS found";
        return false;
    }
    else if ( strcmp( astGetC( wcsinfo, "Class" ), "FrameSet" ) ) {
        m_errorString = "check FITS header (astlib)";
        return false;
    }

    float gbox[] = {
        float ( m_orect.left() ), float ( m_orect.bottom() ),
        float ( m_orect.right() ), float ( m_orect.top() )
    };

    // convert from casa coordinates to fits (add 1)
    double pbox[] = {
        m_irect.left() + 1, m_irect.bottom() + 1,
        m_irect.right() + 1, m_irect.top() + 1
    };

    grfdriverSetVGComposer( m_vgc );

    AstPlot * plot = astPlot( wcsinfo, gbox, pbox, "Grid=1" );
    if ( plot == 0 || ! astOK ) {
        m_errorString = "astPlot() failed";
        return false;
    }

    double minDim = std::min( m_orect.width(), m_orect.height() );
    double desiredGapInPix = 5;
    double desiredGapInPerc = desiredGapInPix / minDim;

    astSetD( plot, "NumLabGap", desiredGapInPerc );
    desiredGapInPix = 3;
    desiredGapInPerc = desiredGapInPix / minDim;
    astSetD( plot, "TextLabGap(1)", desiredGapInPerc );
    desiredGapInPix = 10;
    desiredGapInPerc = desiredGapInPix / minDim;
    astSetD( plot, "TextLabGap(2)", desiredGapInPerc );

    // set system options
    if ( ! m_system.isEmpty() ) {
        std::string sys = QString( "System=%1" ).arg( m_system ).toStdString();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        astSet( plot, sys.c_str() );
#pragma GCC diagnostic pop
        astClear( plot, "Epoch,Equinox" );
    }

    for ( int i = 0 ; i < m_plotOptions.length() ; i++ ) {
        std::string stdstr = m_plotOptions[i].toStdString();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        astSet( plot, stdstr.c_str() );
#pragma GCC diagnostic pop
    }

    if ( true ) {
        double g1 = astGetD( plot, "Gap(1)" );
        double g2 = astGetD( plot, "Gap(2)" );
        astSetD( plot, "Gap(1)", g1 * m_densityModifier );
        astSetD( plot, "Gap(2)", g2 * m_densityModifier );
    }

    {
        fflush(stdout);
        printf("====================== astshow begin ======================");
        astShow( plot);
        printf("======================= astshow end =======================");
        fflush(stdout);

    }

    if ( false ) {
        const char * labelling = astGetC( plot, "Labelling" );
        qDebug() << "labelling= " << labelling << ( ! ! labelling );
        if( ! labelling) {
            qDebug() << "Dave1!";
        }
    }

    astGrid( plot );

    if ( false ) {
        const char * labelling = astGetC( plot, "Labelling" );
        qDebug() << "labelling== " << labelling << ( ! ! labelling );
        if( ! labelling) {
            qDebug() << "Dave2!";
        }
    }

    if ( ! astOK ) {
        qWarning() << "AST error occurred probably in astGrid()" << astStatus;
        astClearStatus;
    }

    plot = (AstPlot *) astAnnul( plot );
    wcsinfo = (AstFrameSet *) astAnnul( wcsinfo );
    fitschan = (AstFitsChan *) astAnnul( fitschan );

    return true;
} // plot

QString
AstGridPlotter::getError()
{
    return m_errorString;
}
}
