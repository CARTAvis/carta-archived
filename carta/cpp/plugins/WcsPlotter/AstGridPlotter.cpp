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

void AstGridPlotter::setAxisPermutation( std::vector<int>& perms ){
    m_axisPerms = perms;
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

AstFrame* AstGridPlotter::_permuteAxes( AstFrameSet* wcsinfo ) const {
    //AstFrameSet* wcsinfoCopy = (AstFrameSet*) astCopy( wcsinfo );
    AstFrame* newFrame = nullptr;

    int axesCount = astGetI( wcsinfo, "NAxes" );
    int permCount = m_axisPerms.size();
    if ( axesCount == permCount ){

        //Decide if we really need to do a permutation based on whether
        //any of the axes have changed from their nominal position.
        int perm[axesCount];
        bool actualPerm = false;
        for ( int i = 0; i < axesCount; i++ ){
            perm[i] = m_axisPerms[i];
            if ( perm[i] != (i+1) ){
                actualPerm = true;
            }
        }

        //Okay, there was an actual change is axis order.
        if ( actualPerm ){

            //astPermAxes( wcsinfo, perm );
            //newFrame = (AstFrameSet*) astConvert( wcsinfo, wcsinfoCopy, "" );
            AstMapping* map = nullptr;
            newFrame = (AstFrame*)(astPickAxes( wcsinfo, axesCount, perm, map ));
            if ( !astOK ){
                qWarning() << "AST error setting permuation axes "<<astStatus;
                astClearStatus;
            }
        }
    }
    return newFrame;
}

bool
AstGridPlotter::plot()
{

    // setup the graphics driver globals
    // =================================
    // copy over pens, making sure we have at least one pen
//    grfGlobals()-> pens = pens();
//    if( pens().empty()) {
//        grfGlobals()->pens.push_back( QPen( QColor( "green"), 1));
//    }
    // setup shadow pen
    grfGlobals()-> lineShadowPenIndex = m_shadowPenIndex;
    // assign VG composer
    grfGlobals()-> vgComposer = m_vgc;
    // pre-cache some things
    grfGlobals()-> prepare();

    // get rid of any ast errors from previous calls, just in case
    astClearStatus;

    // make sure we clean up resources no matter how we exit this method
    AstGuard astGuard;

    // ask AST to read in the FITS header
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

    // try to get WCS out of the fits data
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


    AstFrame* newFrame = _permuteAxes( wcsinfo );

    /*if ( newFrame != nullptr ){
       fflush(stdout);
       printf("====================== astshow frame begin ======================");
       astShow( newFrame );
       printf("======================= astshow frame end =======================");
       fflush(stdout);
    }*/

    float gbox[] = {
        float ( m_orect.left() ), float ( m_orect.bottom() ),
        float ( m_orect.right() ), float ( m_orect.top() )
    };

    // convert from casa coordinates to fits (add 1)
    double pbox[] = {
        m_irect.left() + 1, m_irect.bottom() + 1,
        m_irect.right() + 1, m_irect.top() + 1
    };
    //qDebug() << "pbox="<<pbox[0]<<","<<pbox[1]<<","<<pbox[2]<<","<<pbox[3];

    AstPlot* plot = nullptr;
    if ( newFrame ==nullptr ){
        plot = astPlot( wcsinfo, gbox, pbox, "Grid=1" );
    }
    else {
        //Seems like the new axes ranges are not being set correctly.
        //astSetD( newFrame, "Bottom(2)", 0 );
        //astSetD( newFrame, "Top(2)", 50 );
        plot = astPlot( newFrame, gbox, pbox, "Grid=1" );
    }
    if ( ! plot || ! astOK ) {
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
    if ( !astOK ){
        qWarning() << "AST error setting system"<<astStatus;
        astClearStatus;
    }

    for ( int i = 0 ; i < m_plotOptions.length() ; i++ ) {
        std::string stdstr = m_plotOptions[i].toStdString();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        astSet( plot, stdstr.c_str() );
#pragma GCC diagnostic pop
    }
    if ( !astOK ){
        qWarning() << "AST error setting plot options"<<astStatus;
        astClearStatus;
    }

    if ( true ) {
        double g1 = astGetD( plot, "Gap(1)" );
        double g2 = astGetD( plot, "Gap(2)" );
        astSetD( plot, "Gap(1)", g1 * m_densityModifier );
        astSetD( plot, "Gap(2)", g2 * m_densityModifier );
        if (!astOK ){
            qWarning() << "Ast error setting gap" << astStatus;
            astClearStatus;
        }
    }


    if ( false ) {
        const char * labelling = astGetC( plot, "Labelling" );
        qDebug() << "labelling= " << labelling << ( ! ! labelling );
        if( ! labelling) {
            qDebug() << "Dave1!";
        }
    }

    // call the actual plotting
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
