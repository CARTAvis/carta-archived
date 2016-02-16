#include "StatisticsCASAImage.h"

#include "StatisticsCASA.h"
#include "casacore/measures/Measures/MDirection.h"
#include "casacore/coordinates/Coordinates/DirectionCoordinate.h"
#include "casacore/coordinates/Coordinates/SpectralCoordinate.h"
#include "casacore/components/ComponentModels/GaussianBeam.h"

#include <QDebug>

StatisticsCASAImage::StatisticsCASAImage() {

}


std::vector<QString> StatisticsCASAImage::_beamAsStringVector( const casa::GaussianBeam &beam ){
    std::vector<QString> result;
    if (! beam.isNull()) {
        char buf[512];
        sprintf( buf,"%.2f\"", beam.getMajor("arcsec"));
        result.push_back(QString(buf));
        sprintf( buf,"%.2f\"", beam.getMinor("arcsec") );
        result.push_back(QString(buf));
        sprintf( buf,"%.2f%c", beam.getPA("deg", casa::True), 0x00B0 );
        result.push_back(QString(buf));
    }
    return result;
}


bool StatisticsCASAImage::_beamCompare( const casa::GaussianBeam &a, const casa::GaussianBeam &b ) {
    return a.getArea("rad2") < b.getArea("rad2");
}


void StatisticsCASAImage::_computeStats(const casa::ImageInterface<casa::Float>* image,
        QList<Carta::Lib::StatInfo>& stats ){
    casa::Vector<casa::Int> shapeVector = image->shape().asVector();
    int dimCount = shapeVector.nelements();
    if (dimCount <= 0 ){
        return;
    }
    _insertShape( shapeVector, stats );

    const casa::CoordinateSystem cs = image->coordinates();
    _insertRaDec( cs, shapeVector, stats );

    _insertSpectral( cs, shapeVector, stats );

    _insertRestoringBeam( image, stats );
}


void StatisticsCASAImage::_getStatsPlanar( const casa::ImageInterface<casa::Float>* image,
        QList<Carta::Lib::StatInfo>& statsMap, int zIndex, int hIndex ) {

    try {
        int dim = image->shape().nelements();
        if ( dim <= 2 ){
            return;
        }

        const casa::CoordinateSystem cs = image->coordinates();

        int zAxis = -1;
        if ( cs.hasSpectralAxis() ){
            zAxis = cs.spectralAxisNumber();
        }
        else {
            int tabIndex = cs.findCoordinate( casa::Coordinate::TABULAR );
            if ( tabIndex >= 0 ){
                zAxis = tabIndex;
            }
        }


        int hAxis = -1;
        if ( cs.hasPolarizationCoordinate() ){
            hAxis = cs.polarizationAxisNumber();
        }

        //Get the axis names
        casa::Vector<casa::String> axesNames = cs.worldAxisNames();
        int nameCount = axesNames.size();
        casa::String zAxisName("");
        if ( zAxis >= 0 && zAxis < nameCount ){
            zAxisName = axesNames[zAxis];
        }
        casa::String hAxisName("");
        if ( hAxis >= 0 && hAxis < nameCount ){
            hAxisName = axesNames[hAxis ];
        }

        casa::String zUnit, zspKey, zspVal;

        casa::String unit =  image->units().getName();

        //Region Spectral Plane
        casa::Vector<casa::Double> tWrld;

        casa::String tStr;
        casa::Vector<casa::Double>tPix = cs.referencePixel();
        int tPixSize = tPix.nelements();
        if (zIndex > -1 && zAxis >= 0 && zAxis < tPixSize ) {
            tPix(zAxis) = zIndex;
            if ( cs.toWorld( tWrld, tPix)){
                casa::String zLabel = cs.format(tStr, casa::Coordinate::DEFAULT, tWrld(zAxis), zAxis,
                        true, true, -1, false);
                casa::String valueStr( zLabel + tStr );
                Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::Frequency );
                info.setValue( valueStr.c_str() );
                statsMap.append( info );
            }
        }

        //Region Stokes Plane
        if (hAxis > -1) {
            tPix(hAxis) = hIndex;
            if ( cs.toWorld( tWrld, tPix) ){
                casa::String hLabel = cs.format(tStr, casa::Coordinate::DEFAULT, tWrld(hAxis), hAxis,
                        true, true, -1, false );
                if ( hLabel != "" ){
                    Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::Stokes );
                    info.setValue( hLabel.c_str() );
                    statsMap.append( info );
                }
            }
        }

        //Frequency & Velocity
        casa::Int spInd = cs.findCoordinate(casa::Coordinate::SPECTRAL);
        casa::SpectralCoordinate spCoord;
        if ( spInd>=0 ) {
            spCoord=cs.spectralCoordinate(spInd);
            spCoord.setVelocity();
            casa::Double vel;
            casa::Double restFreq = spCoord.restFrequency();
            if (downcase(zAxisName).contains("freq")) {
                Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::Velocity );
                if (restFreq >0 && spCoord.pixelToVelocity(vel, zIndex)) {
                    info.setValue( QString::number( vel )+"km/s" );
                }
                else {
                    info.setValue( zspVal.c_str() );
                }
                statsMap.append( info );
            }
        }

        // strip out extra quotes, e.g. '"ELECTRONS"'
        std::string unitval(unit.c_str( ));
        std::string::size_type p = 0;
        while( (p = unitval.find('"',p)) != unitval.npos ) {
            unitval.erase(p, 1);
        }
        Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::BrightnessUnit );
        info.setValue( unitval.c_str() );
        statsMap.append( info );

        casa::Double beamArea = 0;
        casa::ImageInfo ii = image->imageInfo();
        casa::GaussianBeam beam = ii.restoringBeam(zIndex);
        std::string imageUnits = image->units().getName();
        std::transform( imageUnits.begin(), imageUnits.end(), imageUnits.begin(), ::toupper );

        casa::Int afterCoord = -1;
        casa::Int dC = cs.findCoordinate(casa::Coordinate::DIRECTION, afterCoord);
        if ( ! beam.isNull() && dC!=-1 && imageUnits.find("JY/BEAM") != std::string::npos ) {
            casa::DirectionCoordinate dCoord = cs.directionCoordinate(dC);
            casa::Vector<casa::String> units(2);
            units(0) = units(1) = "rad";
            dCoord.setWorldAxisUnits(units);
            casa::Vector<casa::Double> deltas = dCoord.increment();
            double divisor = qAbs( deltas(0)* deltas(1));
            beamArea = beam.getArea("rad2") / divisor;
        }

        if ( beamArea > 0 ){
            Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::BeamArea );
            info.setValue( QString::number( beamArea ) );
            statsMap.append( info );
        }
    }
    catch (const casa::AipsError& err) {
        std::string errMsg_ = err.getMesg();
        qDebug() << "Error: "<<errMsg_.c_str();
    }
}

QList<Carta::Lib::StatInfo>
StatisticsCASAImage::getStats( const casa::ImageInterface<casa::Float>* image ){
    QList<Carta::Lib::StatInfo> stats;
    if ( image ){
        Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::Name );
        info.setValue( image->name( true ).c_str() );
        info.setImageStat( true );
        stats.append( info );

        _computeStats( image, stats );

        //Planar statistics
        int zIndex = 0;
        int hIndex = 0;
        _getStatsPlanar( image, stats, zIndex, hIndex );
    }
    return stats;
}


void StatisticsCASAImage::_insertRaDec( const casa::CoordinateSystem& cs,
        casa::Vector<casa::Int>& shapeVector,
        QList<Carta::Lib::StatInfo> & stats ){
    if ( cs.hasDirectionCoordinate( ) ) {
        const casa::DirectionCoordinate &direction = cs.directionCoordinate( );
        casa::String directionType = casa::MDirection::showType(direction.directionType( ));
        casa::Vector<double> refval = direction.referenceValue( );
        if ( refval.size( ) == 2 ) {
            casa::Vector<int> direction_axes = cs.directionAxesNumbers( );
            casa::Vector<double> pix(direction_axes.size( ));
            for ( unsigned int x=0; x < pix.size( ); ++x ){
                pix[x] = 0;
            }
            casa::Vector<double> world (pix.size( ));
            direction.toWorld(world,pix);

            casa::String units;
            std::vector<QString> raStr;
            raStr.push_back(direction.format( units, casa::Coordinate::DEFAULT, world[0], 0, true, true ).c_str());
            std::vector<QString> decStr;
            decStr.push_back(direction.format( units, casa::Coordinate::DEFAULT, world[1], 1, true, true ).c_str());

            for ( unsigned int x=0; x < pix.size( ); ++x ){
                pix[x] = shapeVector[direction_axes[x]];
            }
            direction.toWorld(world,pix);
            raStr.push_back(direction.format( units, casa::Coordinate::DEFAULT, world[0], 0, true, true ).c_str());
            decStr.push_back(direction.format( units, casa::Coordinate::DEFAULT, world[1], 1, true, true ).c_str());

            QString raRange = raStr[0] + ", " + raStr[1];
            QString decRange = decStr[0] + ", " + decStr[1];
            Carta::Lib::StatInfo infoRA( Carta::Lib::StatInfo::StatType::RightAscensionRange );
            infoRA.setValue( raRange );
            stats.append( infoRA );
            Carta::Lib::StatInfo infoDEC( Carta::Lib::StatInfo::StatType::DeclinationRange );
            infoDEC.setValue( decRange );
            stats.append( infoDEC );
        }
        else {
            Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::DirectionType );
            info.setValue( directionType.c_str() );
            stats.append( info );
        }
    }
}

void StatisticsCASAImage::_insertRestoringBeam( const casa::ImageInterface<casa::Float>* image,
        QList<Carta::Lib::StatInfo>& stats ){
    casa::ImageInfo imageInfo = image->imageInfo();
   std::vector<casa::GaussianBeam> restoringBeams;
   Carta::Lib::StatInfo::StatType beamType = Carta::Lib::StatInfo::StatType::MedianRestoringBeam;
   if ( imageInfo.hasBeam( ) ) {
       if ( imageInfo.hasMultipleBeams( ) ) {
           for ( size_t i=0; i < imageInfo.getBeamSet().nchan( ); ++i ){
               restoringBeams.push_back( imageInfo.restoringBeam(i,0) );
           }
       }
       else {
           restoringBeams.push_back(imageInfo.restoringBeam());
           beamType = Carta::Lib::StatInfo::StatType::RestoringBeam;
       }
       std::vector<QString> beamVector = _medianRestoringBeamAsStr( restoringBeams );
       if ( beamVector.size() == 3 ){
           QString beamValue = beamVector[0]+", "+beamVector[1]+", "+beamVector[2];
           Carta::Lib::StatInfo info( beamType );
           info.setValue( beamValue );
           stats.append( info );
       }
   }
}

void StatisticsCASAImage::_insertShape( const casa::Vector<casa::Int>& shapeVector,
        QList<Carta::Lib::StatInfo>& stats ){
    QString shapeStr( "[");
    int dimCount = shapeVector.nelements();
    for ( int i = 0; i < dimCount; i++ ){
        shapeStr = shapeStr + QString::number(shapeVector[i]);
        if ( i < dimCount - 1 ){
            shapeStr = shapeStr + ", ";
        }
    }
    shapeStr = shapeStr + "]";
    Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::Shape );
    info.setValue( shapeStr );
    stats.append( info );
}

void StatisticsCASAImage::_insertSpectral( const casa::CoordinateSystem& cs,
        casa::Vector<casa::Int>& shapeVector, QList<Carta::Lib::StatInfo>& stats ){
    if ( cs.hasSpectralAxis( ) && shapeVector[cs.spectralAxisNumber( )] > 1 ) {
        casa::SpectralCoordinate spec = cs.spectralCoordinate( );
        casa::Vector<casa::String> specUnitVec = spec.worldAxisUnits( );
        if ( specUnitVec(0) == "Hz" ){
            specUnitVec(0) = "GHz";
        }
        spec.setWorldAxisUnits(specUnitVec);

        std::vector<double> frequencies(shapeVector[cs.spectralAxisNumber( )]);
        std::vector<double> velocities(frequencies.size( ));
        QString freqUnits = specUnitVec(0).c_str();
        spec.setVelocity( "km/s" );

        for ( int off=0; off < shapeVector[cs.spectralAxisNumber( )]; ++off ) {
            if ( spec.toWorld(frequencies[off],off) == false ) {
                frequencies.resize(0);
                velocities.resize(0);
                break;
            }

            //An exception if we try to convert pixels to
            //velocity and the rest frequency is zero.
            if ( spec.restFrequency() == 0 || spec.pixelToVelocity(velocities[off],off) == false ) {
                frequencies.resize(0);
                velocities.resize(0);
                break;
            }

        }

        if ( frequencies.size() > 0 ){
            QString freqRange = QString::number( frequencies.front() ) + ", " +
                    QString::number( frequencies.back() ) + " " + freqUnits;
            Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::FrequencyRange );
            info.setValue( freqRange );
            stats.append( info );
        }

        if ( velocities.size( ) > 0 ) {
            QString veloUnits = "km/s";
            QString velRange = QString::number( velocities.front() ) + ", " +
                    QString::number( velocities.back() ) + " "+veloUnits;
            Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::VelocityRange );
            info.setValue( velRange );
            stats.append( info );
        }
    }
}




std::vector<QString>
StatisticsCASAImage::_medianRestoringBeamAsStr( std::vector<casa::GaussianBeam> beams){
    std::vector<QString> result;
    if ( beams.size( ) == 1 ){
        result = _beamAsStringVector(beams[0]);
    }
    else if ( beams.size( ) > 1 ) {
        std::vector<casa::GaussianBeam> beamcopy(beams);
        size_t n = beamcopy.size( ) / 2;
        std::nth_element( beamcopy.begin( ), beamcopy.begin( )+n, beamcopy.end( ),
                StatisticsCASAImage::_beamCompare );
        result = _beamAsStringVector( beamcopy[n] );
    }
    return result;
}


StatisticsCASAImage::~StatisticsCASAImage() {

}
