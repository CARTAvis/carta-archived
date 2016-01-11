/**
 * Generates image statistics.
 */

#pragma once

#include <QList>
#include <QString>
#include "CartaLib/StatInfo.h"
#include "casacore/images/Images/ImageInterface.h"

class StatisticsCASAImage {

public:

    /**
     * Returns a map of (key,value) pairs of image statistics for the image passed in.
     * @param image - a pointer to an image.
     * @return - a map of (key,value) pairs representing the image's statistics.
     */
    static QList<Carta::Lib::StatInfo> getStats( const casa::ImageInterface<casa::Float>* image );
private:
    static bool _beamCompare( const casa::GaussianBeam &a, const casa::GaussianBeam &b );

    static std::vector<QString> _beamAsStringVector( const casa::GaussianBeam &beam );

    static void _computeStats( const casa::ImageInterface<casa::Float>* image,
            QList<Carta::Lib::StatInfo>& stats );

    static void _getStatsPlanar( const casa::ImageInterface<casa::Float>* image,
                QList<Carta::Lib::StatInfo>& stats, int zIndex, int hIndex  );

    static void _insertRaDec( const casa::CoordinateSystem& cs, casa::Vector<casa::Int>& shapeVector,
            QList<Carta::Lib::StatInfo> & stats );

    static void _insertRestoringBeam( const casa::ImageInterface<casa::Float>* image,
            QList<Carta::Lib::StatInfo>& stats );

    static void _insertShape( const casa::Vector<casa::Int>& shapeVector,
            QList<Carta::Lib::StatInfo>& stats );

    static void _insertSpectral( const casa::CoordinateSystem& cs,
            casa::Vector<casa::Int>& shapeVector, QList<Carta::Lib::StatInfo>& stats );


    static std::vector<QString> _medianRestoringBeamAsStr( std::vector<casa::GaussianBeam> beams);
    StatisticsCASAImage();

    virtual ~StatisticsCASAImage();

};
