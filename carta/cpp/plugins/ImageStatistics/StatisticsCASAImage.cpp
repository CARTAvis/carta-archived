#include "StatisticsCASAImage.h"

#include "StatisticsCASA.h"
#include "casacore/measures/Measures/MDirection.h"
#include "casacore/coordinates/Coordinates/DirectionCoordinate.h"
#include "casacore/coordinates/Coordinates/SpectralCoordinate.h"
#include "casacore/images/Images/GaussianBeam.h"

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
        QMap<QString,QString>& stats ){
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


QMap<QString,QString>
StatisticsCASAImage::getStats( const casa::ImageInterface<casa::Float>* image ){
    QMap<QString,QString> stats;
    if ( image ){
        stats.insert( StatisticsCASA::STAT_ID, image->name(true).c_str() );

        _computeStats( image, stats );
    }
    return stats;
}


void StatisticsCASAImage::_insertRaDec( const casa::CoordinateSystem& cs,
        casa::Vector<casa::Int>& shapeVector,
        QMap<QString,QString> & stats ){
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

            QString raKey = QString(directionType.c_str()) + " Right Ascension";
            QString decKey = QString(directionType.c_str()) + " Declination";
            QString raRange = raStr[0] + ", " + raStr[1];
            QString decRange = decStr[0] + ", " + decStr[1];
            stats.insert( raKey, raRange);
            stats.insert( decKey, decRange );
        }
        else {
            QString key = "Direction Type";
            stats.insert( key, directionType.c_str() );
        }
    }
}

void StatisticsCASAImage::_insertRestoringBeam( const casa::ImageInterface<casa::Float>* image,
        QMap<QString,QString>& stats ){
    casa::ImageInfo imageInfo = image->imageInfo();
   std::vector<casa::GaussianBeam> restoringBeams;
   QString key;
   if ( imageInfo.hasBeam( ) ) {
       if ( imageInfo.hasMultipleBeams( ) ) {
           for ( size_t i=0; i < imageInfo.getBeamSet().nchan( ); ++i ){
               restoringBeams.push_back( imageInfo.restoringBeam(i,0) );
           }
           key = "Median Restoring Beam";
       }
       else {
           restoringBeams.push_back(imageInfo.restoringBeam());
           key = "Restoring Beam";
       }
       std::vector<QString> beamVector = _medianRestoringBeamAsStr( restoringBeams );
       if ( beamVector.size() == 3 ){
           QString beamValue = beamVector[0]+", "+beamVector[1]+", "+beamVector[2];
           stats.insert( key, beamValue );
       }

   }
}

void StatisticsCASAImage::_insertShape( const casa::Vector<casa::Int>& shapeVector,
        QMap<QString,QString>& stats ){
    QString shapeStr( "[");
    int dimCount = shapeVector.nelements();
    for ( int i = 0; i < dimCount; i++ ){
        shapeStr = shapeStr + QString::number(shapeVector[i]);
        if ( i < dimCount - 1 ){
            shapeStr = shapeStr + ", ";
        }
    }
    shapeStr = shapeStr + "]";
    stats.insert( "Shape", shapeStr );
}

void StatisticsCASAImage::_insertSpectral( const casa::CoordinateSystem& cs,
        casa::Vector<casa::Int>& shapeVector, QMap<QString,QString>& stats ){
    if ( cs.hasSpectralAxis( ) && shapeVector[cs.spectralAxisNumber( )] > 1 ) {
        //has_spectral_axis = true;
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
            QString key = "Frequency Range";
            QString freqRange = QString::number( frequencies.front() ) + ", " +
                    QString::number( frequencies.back() ) + " " + freqUnits;
            stats.insert( key, freqRange );
        }

        if ( velocities.size( ) > 0 ) {
            QString key = "Velocity Range";
            QString veloUnits = "km/s";
            QString velRange = QString::number( velocities.front() ) + ", " +
                    QString::number( velocities.back() ) + " "+veloUnits;
            stats.insert( key, velRange );
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
