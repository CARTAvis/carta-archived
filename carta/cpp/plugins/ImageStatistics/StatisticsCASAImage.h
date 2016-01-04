/**
 * Generates image statistics.
 */

#pragma once

#include <QMap>
#include <QString>
#include "casacore/images/Images/ImageInterface.h"

class StatisticsCASAImage {

public:

    /**
     * Returns a map of (key,value) pairs of image statistics for the image passed in.
     * @param image - a pointer to an image.
     * @return - a map of (key,value) pairs representing the image's statistics.
     */
    static QMap<QString,QString> getStats( const casa::ImageInterface<casa::Float>* image );
private:
    static bool _beamCompare( const casa::GaussianBeam &a, const casa::GaussianBeam &b );

    static std::vector<QString> _beamAsStringVector( const casa::GaussianBeam &beam );

    static void _computeStats( const casa::ImageInterface<casa::Float>* image,
            QMap<QString,QString>& stats );

    static void _insertRaDec( const casa::CoordinateSystem& cs, casa::Vector<casa::Int>& shapeVector,
            QMap<QString,QString> & stats );

    static void _insertRestoringBeam( const casa::ImageInterface<casa::Float>* image,
            QMap<QString,QString>& stats );

    static void _insertShape( const casa::Vector<casa::Int>& shapeVector,
            QMap<QString,QString>& stats );

    static void _insertSpectral( const casa::CoordinateSystem& cs,
            casa::Vector<casa::Int>& shapeVector, QMap<QString,QString>& stats );


    static std::vector<QString> _medianRestoringBeamAsStr( std::vector<casa::GaussianBeam> beams);
    StatisticsCASAImage();

    virtual ~StatisticsCASAImage();

};
