#include "AstGridPlotter.h"
#include <iostream>
#include "grfdriver.h"

#include <string.h>
#include <locale.h>
extern "C" {
#include <ast.h>
};


namespace WcsPlotterPluginNS
{

QThread AstGridPlotter::astThread;


AstGridPlotter::AstGridPlotter()
{
//    impl_ = new Impl;
    m_carLin = false;
}

AstGridPlotter::~AstGridPlotter()
{
//     delete impl_;
}


struct AstGuard {
    AstGuard() { astBegin; }

    ~AstGuard() { astEnd; }
};

QString
AstGridPlotter::getError()
{
    return m_errorString;
}

void AstGridPlotter::startPlotSlot(){
    bool plotSuccess = plot();

        if( ! plotSuccess) {
            qWarning() << "Grid rendering error"; // << sgp.getError()
        }

    emit plotResultSignal();
}


/// This part of code will be removed in the future
/*
AstFrameSet * AstGridPlotter::_make2dFrame( AstFrameSet* wcsinfo ){
    AstFrameSet * result = nullptr;
    bool celestialPlane = Carta::Lib::AxisDisplayInfo::isCelestialPlane( m_axisDisplayInfos );
    if ( !celestialPlane ){
        result = _make2dFrameCelestialExclude( wcsinfo );
    }
    else {
        result = wcsinfo;
    }
    return result;
}

AstFrameSet * AstGridPlotter::_make2dFrameSet( AstFrameSet *fs,
        int opaxis, int owaxis, int zpaxis, int zwaxis,
        int* paxisvals,
        int* naxis, int axisCount,
        bool celestialFirst){

    //Returned frame set.
    AstFrameSet *result = nullptr;

    // Begin an AST object context, so that we do not need to annul AST
    //objects individually before leaving this function.
    astBegin;

    // Check the supplied FrameSet has axisCount inputs and axisCount outputs.
    if( astGetI( fs, "Nin" ) != axisCount || astGetI( fs, "Nout" ) != axisCount ){
        m_errorString = "Bad FrameSet supplied";
    }
    else {
        // Check the supplied axis indices are in the range 1 to axisCount.
        if ( opaxis < 1 || opaxis > axisCount || owaxis < 1 || owaxis > axisCount ||
                zpaxis < 1 || zpaxis > axisCount || zwaxis < 1 || zwaxis > axisCount ){
            m_errorString = "Bad axis index supplied";
        }
        else {
            // Mapping from 3-d world to 3-d pixel
            // Get the Mapping from world coordinates to pixel coordinates, and
            //extract the 1-d Mapping for the non-celestial axis.
            AstMapping* map1 = static_cast<AstMapping*> astGetMapping( fs, AST__CURRENT, AST__BASE );
            // Non-celestial Mapping from 1-d world to 1-d pixel
            AstMapping *map2 = nullptr;
            astMapSplit( map1, 1, &zwaxis, &zpaxis, &map2 );
            if( !map2 || astGetI( map2, "Nin" ) != 1 || astGetI( map2, "Nout" ) != 1 ) {
                m_errorString = "The non-celestial axis cannot be split from the celestial axes";
            }
            else {
                // Invert map2 to it goes from the non-celestial pixel axis to the
                // non-celestial world axis.
                astInvert( map2 );

                // Allocate memory to hold a set of 3-d pixel positions that sample the
                // path across the sky of the required 2-d slice. The samples are one
                // pixel apart.
                // Number of samples along the slice.
                int nsamp = naxis[ opaxis - 1 ];
                // Work space holding 3-d pixel positions
                double *work1 = static_cast<double*>( astMalloc( axisCount*nsamp*sizeof( double ) ) );
                if( work1 ) {

                    // Fill the above array with the pixel positions. The values for the
                    // non-celestial pixel axis are set to 1. The values for the celestial
                    // pixel axis that is being removed are set to "paxisval". The values for
                    // the retained celestial pixel axis are set to [1,2,3,...,naxisn].
                    for( int i = 0; i < nsamp; i++ ) {
                        for ( int j = 0; j < axisCount; j++ ){
                            int workIndex = j * nsamp + i;

                            if ( j== opaxis -1 ){
                                work1[ workIndex ] = i + 1;
                            }
                            else if ( j== zpaxis - 1) {
                                work1[ workIndex ] = 1;
                            }
                            else {
                                if (paxisvals[j] >= 0 ){
                                    work1[ workIndex ] = paxisvals[j];
                                }
                                else {
                                    work1[workIndex] = 0;
                                }
                            }
                        }
                    }

                    // Allocate memory to hold the world positions corresponding to the
                    //above pixel positions.
                    nsamp = naxis[ opaxis - 1 ];
                    // Work space holding 3-d world positions
                    double* work2 = static_cast<double*>( astMalloc( axisCount*nsamp*sizeof( double ) ) );
                    if( work2 ) {

                        // Transform the pixel positions into world coordinates.
                        //astSetI( fs, "Report", 1 );
                        astTranN( fs, nsamp, axisCount, nsamp, work1, 1, axisCount, nsamp, work2 );
                        //astSetI( fs, "Report", 0 );

                        // 1-d Mapping from pixel to retained celestial axis.
                        // Create a LutMap that transforms 1-d pixel axis value (on the pixel axis
                        // that is being retained) into the corresponding value on the retained
                        // celestial axis.
                        AstLutMap* lutmap = astLutMap( nsamp, work2 + (owaxis-1)*nsamp, 1.0, 1.0, " " );

                        // We can only proceed if the retained celestial axis varies monotonically
                        // along the slice. This will be the case if the above Mapping has a defined
                        // inverse transformation (i.e. we can transform a value on the retained
                        // world axis into the corresponding value on the retaied pixel axis,
                        // given the point is on the specified slice in pixel coords).
                        if( !astGetI( lutmap, "TranInverse" ) ) {
                            char attname[12];    //Attribute name
                            sprintf( attname, "Label(%d)", owaxis );
                            m_errorString = QString("The change in %1 along the slice is non-monotonic.").arg(astGetC( fs, attname ) );

                        }
                        else {

                            // Create a 2-d frame to represent pixel coordinates in the returned
                            // FrameSet. This is done by picking the required axis from the existing
                            // 3-d pixel Frame. Create a new FramneSet holding just this one Frame.
                            int axes[2]; // 1-based indices of axes to be picked
                            if ( celestialFirst ){
                                axes[ 0 ] = opaxis;
                                axes[ 1 ] = zpaxis;
                            }
                            else {
                                axes[ 0 ] = zpaxis;
                                axes[ 1 ] = opaxis;
                            }

                            //Returned FrameSet
                            result = astFrameSet( astPickAxes( astGetFrame( fs, AST__BASE ), 2, axes,
                                    NULL ), " " );

                            // 2-d Mapping from pixel to world
                            AstCmpMap *map3 = nullptr;
                            //Create a Mapping that transforms 2-d pixel coords into 2-d world
                            //coords. This is a parallel compound Mapping, containing the mapping
                            //for the non-celestial axis, and the lutmap created above, in parallel.
                            if ( celestialFirst ){
                                map3 = astCmpMap( lutmap, map2, 0, " " );
                            } else {
                                map3 = astCmpMap( map2, lutmap, 0, " " );
                            }

                            // 2-d Mapping from pixel to world - reordered
                            AstCmpMap *map4 = nullptr;
                            // If necessary add in a PermMap that swaps the retained world axes so
                            // that they are in the same order they had in the supplied FrameSet.
                            if( ( zpaxis > opaxis && zwaxis < owaxis ) ||
                                    ( zpaxis < opaxis && zwaxis > owaxis ) ) {
                                axes[ 0 ] = 2;
                                axes[ 1 ] = 1;
                                map4 = astCmpMap( map3, astPermMap( 2, axes, 2, axes, NULL, " " ),
                                        1, " " );
                            } else {
                                map4 = map3;
                            }

                            // Create a 2-d frame to represent world coordinates in the returned
                            //FrameSet. This is done by picking the required axis from the existing
                            //3-d world Frame. Add this Frame into the returned FrameSet using the
                            //above mapping to connect it to the pixel Frame that is already in the
                            //returned FrameSet.
                            if ( celestialFirst ){
                                axes[ 0 ] = owaxis;
                                axes[ 1 ] = zwaxis;
                            } else {
                                axes[ 0 ] = zwaxis;
                                axes[ 1 ] = owaxis;
                            }
                            astAddFrame( result, AST__BASE, map4, astPickAxes( fs, 2, axes, NULL ) );

                            // Export the pointer to the new FrameSet so that it is not annulled by
                            //the following call to astEnd.
                            astExport( result );

                            //Free work space.
                            work2 = static_cast<double*>(astFree( work2 ));
                        }
                        // Free work space.
                        work1 = static_cast<double*>(astFree( work1 ));
                    }
                }
            }
        }
    }

    //End the AST Object context. This annuls all AST objects created since
    //the earlier astBegin call, except for those that have been exported from
    //the context.
    astEnd;

    //Return the ponter to the new FrameSet.
    return result;
}

AstFrameSet* AstGridPlotter::_make2dFrameCelestialExclude( AstFrameSet* wcsinfo ) {
    AstFrameSet* newFrame = wcsinfo;
    bool actualPerm = Carta::Lib::AxisDisplayInfo::isPermuted( m_axisDisplayInfos );
    if ( actualPerm ){
       int axesCount = astGetI( wcsinfo, "NAxes" );
       int naxis[axesCount];
       int paxisvals[axesCount];
       int displayAxes[2];
       int displayAxesIndex = 0;
       for ( int i = 0; i < axesCount; i++ ){
           naxis[i] = m_axisDisplayInfos[i].getFrameCount();
           paxisvals[i] = m_axisDisplayInfos[i].getFrame();
           //Display axis
           if ( paxisvals[i] < 0 ){
               displayAxes[displayAxesIndex] = i+1;
               displayAxesIndex++;
           }
       }

       int firstPermuteIndex = m_axisDisplayInfos[displayAxes[0] - 1 ].getPermuteIndex();
       int secondPermuteIndex = m_axisDisplayInfos[displayAxes[1] - 1 ].getPermuteIndex();

       int zwaxis = displayAxes[1];
       int zpaxis = displayAxes[1];

       //Assume we will be plotting the world axis horizontally (first) and the non-celestial
       //axis vertically(second).
       int owaxis = displayAxes[0];
       int opaxis = displayAxes[0];

       bool celestialFirst = true;
       Carta::Lib::AxisInfo::KnownType axisType = m_axisDisplayInfos[displayAxes[0] - 1].getAxisType();
       //Axis index 0 is celestial.
       if ( axisType == Carta::Lib::AxisInfo::KnownType::DIRECTION_LON ||
               axisType == Carta::Lib::AxisInfo::KnownType::DIRECTION_LAT ){
           //We want to plot the celestial (world) axis on the vertical axis (second) and
           //the non-celestial axis first;
           if ( secondPermuteIndex < firstPermuteIndex ){
               celestialFirst = false;
           }
       }
       //Axis index 1 is celestial
       else {
           zwaxis = displayAxes[0];
           zpaxis = displayAxes[0];
           owaxis = displayAxes[1];
           opaxis = displayAxes[1];
           //We want to plot the celestial (world) axis on the vertical axis (second) and
           //the non-celestial axis first;
           if ( firstPermuteIndex < secondPermuteIndex ){
               celestialFirst = false;
           }
       }

        //First pixel and world axes to retain
        newFrame = _make2dFrameSet( wcsinfo, opaxis, owaxis, zpaxis, zwaxis,
                //Array containing -1 if axis is retained, otherwise index of axis.
                paxisvals,
                //Array containing image shape plus axis count.
                naxis, m_axisDisplayInfos.size(),
                celestialFirst );
    }
    return newFrame;
}
*/

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
    grfGlobals()-> lineShadowPenIndex = -1;//m_shadowPenIndex;
    // assign VG composer
    grfGlobals()-> vgComposer = m_vgc;
    // pre-cache some things
    grfGlobals()-> prepare();
    
    // Temporarily override numeric locale, otherwise AST will fail to 
    // parse floating point numbers in the FITS header if the user's 
    // locale uses a comma as a decimal separator. Back up the old 
    // locale so that we can switch back afterwards and minimise impact
    // on the rest of the application.
    
    std::string oldLocale = setlocale(LC_NUMERIC, "C");

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
    if ( ! astOK ) {
        qDebug() << "astPutCards() failed";
        m_errorString = "astPutCards() failed, check logs.";
        return false;
    }

    if ( m_carLin ) {
        astSet( fitschan, "CarLin=1" );
    }
    else {
        astSet( fitschan, "CarLin=0" );
    }

    // try to get WCS out of the fits data
    AstFrameSet * wcsinfo = static_cast < AstFrameSet * > ( astRead( fitschan ) );
    if ( ! astOK ) {
        m_errorString = "astRead() failed, check logs.";
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

    AstFrameSet* newFrame = wcsinfo; //_make2dFrame( wcsinfo );
    if ( newFrame == nullptr ){
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

    AstPlot* plot = astPlot( newFrame, gbox, pbox, "Grid=1" );
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

    //Set the plot options
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

    // set system options
    /*if ( ! m_system.isEmpty() ) {
        qDebug() << "System="<<m_system;
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
    }*/

    // TODO: This part of code will move to '_setDisplayLabelOptionforAst()' in AstWcsGridRendeerService.cpp
    // set Label
    for(int ii = 0; ii < 2; ii = ii + 1)
    {
        QString target = QString("System(%1)").arg(ii+1);
        const char* CAxisSystem = astGetC( plot, target.toStdString().c_str() );
        QString SAxisSystem(CAxisSystem);
        bool isequatorial = 0;
        if(SAxisSystem == "FK5")
        {
            isequatorial = 1;
            SAxisSystem = ("J2000");
        }
        else if(SAxisSystem == "FK4")
        {
            isequatorial = 1;
            SAxisSystem = ("B1950");
        }
        else if(SAxisSystem == "ICRS")
        {
            isequatorial = 1;
            SAxisSystem = ("ICRS");
        }
        else
        {
            isequatorial = 0;
        }

        if(isequatorial)
        {
            target = QString("Label(%1)").arg(ii+1);
            const char* oldLabel = astGetC( plot, target.toStdString().c_str() );
            QString CapLabel = QString(oldLabel);
            if(CapLabel == "Right ascension")
            {
                CapLabel = "Right Ascension";
            }

            if(CapLabel == "Ecliptic longitude")
            {
                CapLabel = "Ecliptic Longitude";
            }

            if(CapLabel == "Ecliptic latitude")
            {
                CapLabel = "Ecliptic Latitude";
            }

            QString newLabel = QString("%1=%2 %3").arg(target).arg(SAxisSystem).arg(CapLabel);
            astSet( plot, newLabel.toStdString().c_str() );
        }
        else
        {
            target = QString("Label(%1)").arg(ii+1);
            const char* oldLabel = astGetC( plot, target.toStdString().c_str() );
            QString CapLabel = QString(oldLabel);

            if(CapLabel == "Galactic longitude")
            {
                CapLabel = "Galactic Longitude";
            }

            if(CapLabel == "Galactic latitude")
            {
                CapLabel = "Galactic Latitude";
            }

            if(CapLabel == "Ecliptic longitude")
            {
                CapLabel = "Ecliptic Longitude";
            }

            if(CapLabel == "Ecliptic latitude")
            {
                CapLabel = "Ecliptic Latitude";
            }

            QString newLabel = QString("%1=%3").arg(target).arg(CapLabel);
            astSet( plot, newLabel.toStdString().c_str() );
        }

        if(!isequatorial && SAxisSystem == "Cartesian")
        {
            target = QString("Label(%1)").arg(ii+1);
            const char* oldLabel = astGetC( plot, target.toStdString().c_str() );

            // Capitalization
            QString CapLabel = QString(oldLabel).toLower();
            CapLabel.replace(0, 1, QString(oldLabel[0]).toUpper() );
            QString newLabel = QString("%1=%3").arg(target).arg(CapLabel);
            astSet( plot, newLabel.toStdString().c_str() );
        }

    }


    // call the actual plotting
    astGrid( plot );

    if ( ! astOK ) {
        qWarning() << "AST error occurred probably in astGrid()" << astStatus;
        astClearStatus;
    }

    plot = (AstPlot *) astAnnul( plot );
    wcsinfo = (AstFrameSet *) astAnnul( wcsinfo );
    fitschan = (AstFitsChan *) astAnnul( fitschan );
    
    // Restore previous numeric locale
    
    setlocale(LC_NUMERIC, oldLocale.c_str());

    return true;
} // plot

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

/*void
AstGridPlotter::setSystem( const QString & system )
{
    m_system = system;
}*/

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




}
