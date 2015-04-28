#include "AstGridPlotter.h"
#include <iostream>
#include "grfdriver.h"

extern "C" {
#include <ast.h>
};

namespace WcsPlotterPluginNS
{

AstGridPlotterQImage::AstGridPlotterQImage()
{
//    impl_ = new Impl;
    m_carLin = false;
    m_img = nullptr;
}

AstGridPlotterQImage::~AstGridPlotterQImage()
{
//     delete impl_;
}

bool AstGridPlotterQImage::setFitsHeader(const QString & hdr)
{
    m_fitsHeader = hdr;

    return true;
}

void AstGridPlotterQImage::setCarLin(bool flag)
{
    m_carLin = flag;
}

void AstGridPlotterQImage::setSystem( const QString & system)
{
    m_system = system;
}

void AstGridPlotterQImage::setOutputImage(QImage *img)
{
    m_img = img;
}

void AstGridPlotterQImage::setOutputVGComposer(AstGridPlotterQImage::VGComposer * vgc)
{
    m_vgc = vgc;
}

void AstGridPlotterQImage::setOutputRect(const QRectF &rect)
{
    m_orect = rect;
}

void AstGridPlotterQImage::setInputRect(const QRectF & rect)
{
    m_irect = rect;
}

void AstGridPlotterQImage::setPlotOption(const QString & option)
{
    m_plotOptions.append( option);
}

struct AstGuard {
    AstGuard() { astBegin; }
    ~AstGuard() { astEnd; }
};

bool AstGridPlotterQImage::plot()
{
    astClearStatus;
    AstGuard astGuard;

//    if( ! m_img ) {
//        m_errorString = "No image set";
//        return false;
//    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
    AstFitsChan * fitschan = astFitsChan( NULL, NULL, "" );
#pragma GCC diagnostic pop

    if( ! fitschan) {
        m_errorString = "astFitsChan returned null :(";
        return false;
    }

    std::string stdstr = m_fitsHeader.toStdString();
    astPutCards( fitschan, stdstr.c_str());
    if( m_carLin)
        astSet( fitschan, "CarLin=1");
    else
        astSet( fitschan, "CarLin=0");

    AstFrameSet * wcsinfo = static_cast<AstFrameSet*>( astRead( fitschan ));

    if ( !astOK ) {
        m_errorString = "Some AST LIB error, check logs.";
        return false;
    } else if ( wcsinfo == AST__NULL ) {
        m_errorString = "No WCS found";
        return false;
    } else if ( strcmp( astGetC( wcsinfo, "Class" ), "FrameSet" ) ) {
        m_errorString = "check FITS header (astlib)";
        return false;
    }

    float gbox[] = { float(m_orect.left()), float(m_orect.bottom()),
                     float(m_orect.right()), float(m_orect.top()) };
    // convert from casa coordinates to fits (add 1)
    double pbox[] = { m_irect.left()+1, m_irect.bottom()+1,
                      m_irect.right()+1, m_irect.top()+1 };

    grfSetImage( m_img);
    grfdriverSetVGComposer( m_vgc);

    AstPlot * plot = astPlot( wcsinfo, gbox, pbox, "Grid=1" );
    if( plot == 0 || ! astOK) {
        m_errorString = "astPlot() failed";
        return false;
    }

    double minDim = std::min( m_orect.width(), m_orect.height());
    double desiredGapInPix = 2;
    double desiredGapInPerc = desiredGapInPix / minDim;

    astSetD( plot, "NumLabGap", desiredGapInPerc);
    desiredGapInPix = 3;
    desiredGapInPerc = desiredGapInPix / minDim;
    astSetD( plot, "TextLabGap(1)", desiredGapInPerc);
    desiredGapInPix = 5;
    desiredGapInPerc = desiredGapInPix / minDim;
    astSetD( plot, "TextLabGap(2)", desiredGapInPerc);

    // set system options
    if( ! m_system.isEmpty()) {
        std::string sys = QString("System=%1").arg(m_system).toStdString();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        astSet( plot, sys.c_str());
#pragma GCC diagnostic pop
        astClear( plot, "Epoch,Equinox");
    }

    for( int i = 0 ; i < m_plotOptions.length() ; i ++ ) {
        std::string stdstr = m_plotOptions[i].toStdString();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        astSet( plot, stdstr.c_str());
#pragma GCC diagnostic pop
    }

    astGrid( plot );

    grfSetImage( nullptr);
    grfdriverSetVGComposer( nullptr);

    plot = (AstPlot *) astAnnul( plot);
    wcsinfo = (AstFrameSet *) astAnnul( wcsinfo);
    fitschan = (AstFitsChan *) astAnnul( fitschan);

    return true;
}

QString AstGridPlotterQImage::getError()
{
    return m_errorString;
}

void AstGridPlotterQImage::setLineColor(QString color)
{
    grfSetLineColor( color);
}

void AstGridPlotterQImage::setTextColor(QString color)
{
    grfSetTextColor( color);
}

}
