// WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING
//
// This entire class is very unfriendly to multithreading because it uses
// ASTlib's plotting, which I could only get to work using global variables.
//
// WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING


#ifndef SKYGRIDPLOTTER_H
#define SKYGRIDPLOTTER_H

#include <QSize>
#include <QString>
#include <QRectF>
#include <QStringList>
//#include "WcsHero.h"

class QImage;

class SkyGridPlotter
{

public:

//    enum System { Galactic, Ecliptic, FK4, FK5, ICRS, Default };
//    typedef WcsHero::Hero::SKYCS System;

    SkyGridPlotter();
    ~SkyGridPlotter();

    /// feed the plotter the raw FITS header (concatenated 80-char strings)
    /// returns whether the header has enough information about
    /// about sky coordinates
    bool setFitsHeader( const QString & hdr);

    /// set whether to use the old CAR interpretation (i.e. CAR is linear)
    void setCarLin( bool flag);

    /// set which system to plot
//    void setSystem( System system);
    void setSystem( const QString & system = QString());

    /// sets the output image (does not take ownership)
    void setOutputImage( QImage * img);

    /// sets the output rectangle in image coordinates for the grid (not labels)
    void setOutputRect( const QRectF & rect);

    /// sets which part of the FITS image to draw coordinates for
    void setInputRect(const QRectF & rect);

    /// set various options for grid drawing
    /// this is a temporary method, and it is dependent on AST lib
    /// \todo Replace this with implementation independent API
    void setPlotOption(const QString & option);

    /// perform the actual plot on the image
    /// returns success/failure
    bool plot();

    /// return the error message
    QString getError();

    /// static function to set text and line colors
    static void setLineColor( QString color);
    static void setTextColor( QString color);

protected:

    bool carLin_;
    QString errorString_;
    QString fitsHdr_;
    QStringList plotOptions_;
//    System system_;
    QString system_;
    QRectF orect_, irect_;
    QImage * img_;

//    /// implementation specific (AST)
//    class Impl;
//    Impl * impl_;

};

#endif // SKYGRIDPLOTTER_H
