#include "SkyGridPlotter.h"
#include <iostream>
#include "FitsViewerLib/FitsParser.h"
#include "FitsViewerLib/common.h"
#include "grfdriver.h"

extern "C" {
#include "ast.h"
};

//class SkyGridPlotter::Impl {
//    friend class SkyGridPlotter;
//public:

//    AstFitsChan *fitschan;
//    AstFrameSet *wcsinfo;
//    AstPlot * plot;

//    Impl() {
//        fitschan = 0;
//        wcsinfo = 0;
//        plot = 0;
//    }
//};

SkyGridPlotter::SkyGridPlotter()
{
//    impl_ = new Impl;
    carLin_ = false;
    img_ = 0;
}

SkyGridPlotter::~SkyGridPlotter()
{
//     delete impl_;
}

bool SkyGridPlotter::setFitsHeader(const QString & hdr)
{
    fitsHdr_ = hdr;

    return true;
}

void SkyGridPlotter::setCarLin(bool flag)
{
    carLin_ = flag;
}

void SkyGridPlotter::setSystem( const QString & system)
{
    system_ = system;
}

void SkyGridPlotter::setOutputImage(QImage *img)
{
    img_ = img;
}

void SkyGridPlotter::setOutputRect(const QRectF &rect)
{
    orect_ = rect;
}

void SkyGridPlotter::setInputRect(const QRectF & rect)
{
    irect_ = rect;
}

void SkyGridPlotter::setPlotOption(const QString & option)
{
    plotOptions_.append( option);
}

struct AstGuard {
    AstGuard() { astBegin; }
    ~AstGuard() { astEnd; }
};

bool SkyGridPlotter::plot()
{
    astClearStatus;
    AstGuard astGuard;

    if( ! img_ ) {
        errorString_ = "No image set";
        return false;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
    AstFitsChan * fitschan = astFitsChan( NULL, NULL, "" );
#pragma GCC diagnostic pop

    if( ! fitschan) {
        errorString_ = "astFitsChan returned null :(";
        return false;
    }

    std::string stdstr = fitsHdr_.toStdString();
    astPutCards( fitschan, stdstr.c_str());
    if( carLin_)
        astSet( fitschan, "CarLin=1");
    else
        astSet( fitschan, "CarLin=0");

    AstFrameSet * wcsinfo = static_cast<AstFrameSet*>( astRead( fitschan ));

    if ( !astOK ) {
        errorString_ = "Some AST LIB error, check logs.";
        return false;
    } else if ( wcsinfo == AST__NULL ) {
        errorString_ = "No WCS found";
        return false;
    } else if ( strcmp( astGetC( wcsinfo, "Class" ), "FrameSet" ) ) {
        errorString_ = "check FITS header (astlib)";
        return false;
    }

    float gbox[] = { float(orect_.left()), float(orect_.bottom()),
                     float(orect_.right()), float(orect_.top()) };

    double pbox[] = { irect_.left(), irect_.bottom(), irect_.right(), irect_.top() };

    grfSetImage( img_);

    AstPlot * plot = astPlot( wcsinfo, gbox, pbox, "Grid=1" );
    if( plot == 0 || ! astOK) {
        errorString_ = "astPlot() failed";
        return false;
    }

    double minDim = std::min( orect_.width(), orect_.height());
    double desiredGapInPix = 2;
    double desiredGapInPerc = desiredGapInPix / minDim;
    dbg(1) << "Setting gap to " << desiredGapInPerc << "\n";
    astSetD( plot, "NumLabGap", desiredGapInPerc);
    desiredGapInPix = 3;
    desiredGapInPerc = desiredGapInPix / minDim;
    astSetD( plot, "TextLabGap(1)", desiredGapInPerc);
    desiredGapInPix = 5;
    desiredGapInPerc = desiredGapInPix / minDim;
    astSetD( plot, "TextLabGap(2)", desiredGapInPerc);

    // set system options
    if( ! system_.isEmpty()) {
        std::string sys = QString("System=%1").arg(system_).toStdString();
        dbg(1) << "sys=" << sys;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        astSet( plot, sys.c_str());
#pragma GCC diagnostic pop
        astClear( plot, "Epoch,Equinox");
    }

//    if( system_ == FK5) {
//        astSet( plot, "System=FK5, Equinox=J2000");
//    } else if( system_ == FK4) {
//        astSet( plot, "System=FK4, Equinox=B1950");
//    } else if( system_ == ICRS) {
//        astSet( plot, "System=ICRS");
//    } else if( system_ == Galactic) {
//        astSet( plot, "System=Galactic");
//    } else if( system_ == Ecliptic) {
//        astSet( plot, "System=Ecliptic");
//    } else {
//        // leave it be
//    }



    dbg(1) << "plot options:\n";
    for( int i = 0 ; i < plotOptions_.length() ; i ++ ) {
        std::string stdstr = plotOptions_[i].toStdString();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        astSet( plot, stdstr.c_str());
#pragma GCC diagnostic pop
        dbg(1) << i << ": " << plotOptions_[i] << "\n";
    }

    astGrid( plot );

    grfSetImage(0);

    plot = (AstPlot *) astAnnul( plot);
    wcsinfo = (AstFrameSet *) astAnnul( wcsinfo);
    fitschan = (AstFitsChan *) astAnnul( fitschan);

    return true;
}

QString SkyGridPlotter::getError()
{
    return errorString_;
}

void SkyGridPlotter::setLineColor(QString color)
{
    grfSetLineColor( color);
}

void SkyGridPlotter::setTextColor(QString color)
{
    grfSetTextColor( color);
}
