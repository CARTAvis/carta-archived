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
    static QList<Carta::Lib::StatInfo> getStats( const casacore::ImageInterface<casacore::Float>* image );
private:
    static bool _beamCompare( const casacore::GaussianBeam &a, const casacore::GaussianBeam &b );

    static std::vector<QString> _beamAsStringVector( const casacore::GaussianBeam &beam );

    static void _computeStats( const casacore::ImageInterface<casacore::Float>* image,
            QList<Carta::Lib::StatInfo>& stats );

    static void _getStatsPlanar( const casacore::ImageInterface<casacore::Float>* image,
                QList<Carta::Lib::StatInfo>& stats, int zIndex, int hIndex  );

    static void _insertRaDec( const casacore::CoordinateSystem& cs, casacore::Vector<casacore::Int>& shapeVector,
            QList<Carta::Lib::StatInfo> & stats );

    static void _insertRestoringBeam( const casacore::ImageInterface<casacore::Float>* image,
            QList<Carta::Lib::StatInfo>& stats );

    static void _insertShape( const casacore::Vector<casacore::Int>& shapeVector,
            QList<Carta::Lib::StatInfo>& stats );

    static void _insertSpectral( const casacore::CoordinateSystem& cs,
            casacore::Vector<casacore::Int>& shapeVector, QList<Carta::Lib::StatInfo>& stats );


    static std::vector<QString> _medianRestoringBeamAsStr( std::vector<casacore::GaussianBeam> beams);
    StatisticsCASAImage();

    virtual ~StatisticsCASAImage();

};
