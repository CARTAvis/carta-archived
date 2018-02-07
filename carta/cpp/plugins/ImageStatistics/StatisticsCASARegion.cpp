#include "StatisticsCASARegion.h"
#include "StatisticsCASA.h"
#include "RegionRecordFactory.h"
#include "imageanalysis/ImageAnalysis/ImageStatsCalculator.h"

#include <QDebug>

StatisticsCASARegion::StatisticsCASARegion() {
}


QList<Carta::Lib::StatInfo>
StatisticsCASARegion::getStats(
        casacore::ImageInterface<casacore::Float>* image,
        std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo,
        const std::vector<int>& slice ){
    QList<Carta::Lib::StatInfo> stats;
    QString regionTypeStr;
    casacore::Record regionRecord = RegionRecordFactory:: getRegionRecord(
            image, regionInfo, slice, regionTypeStr );
    _getStatsFromCalculator( image, regionRecord, slice, stats, regionTypeStr );
    return stats;
}


void StatisticsCASARegion::_getStatsFromCalculator( casacore::ImageInterface<casacore::Float>* image,
       const casacore::Record& region, const std::vector<int>& slice,
       QList<Carta::Lib::StatInfo>& stats, const QString& regionType ){
    //If the region record is empty, there are not stats - just return.
    int fieldCount = region.nfields();
    if ( fieldCount == 0 ){
        return;
    }
    //std::shared_ptr<const casacore::ImageInterface<casacore::Float> > imagePtr( image->cloneII() );

    casacore::CoordinateSystem cs = image->coordinates();
    casacore::Vector<casacore::Int> displayAxes = cs.directionAxesNumbers();
    casacore::Quantum<casacore::Double> pix0( 0, "pix");
    casacore::IPosition shape = image->shape();
    int nAxes = shape.nelements();
    casacore::Vector<casacore::Quantum<casacore::Double> > blcq( nAxes, pix0);
    casacore::Vector<casacore::Quantum<casacore::Double> > trcq( nAxes, pix0);
    for ( int i = 0; i < nAxes; i++ ){
        if ( i == displayAxes[0] || i == displayAxes[1]){
            trcq[i].setValue( shape[i] - 1 );
        }
        else {
            // select the current spectral or stoke channel
            blcq[i].setValue( slice[i] );
            trcq[i].setValue( slice[i] );
        }
    }

    /**
     * We get Rectangle, Polygon, Ellipse or Point region first.
     * Rectangle and Point objects can select the range of spectral or stoke channel (frame),
     * by default we select the current channel. They are defined in the "RegionRecordFactory.cpp".
     * On the other hand, Polygon and Ellipse objects can not select the range of spectral or stoke channel.
     * So we need the second stage of the box region process,
     * which is used to select the range of spectral or stoke channel.
     */
    casacore::WCBox box( blcq, trcq, cs, casacore::Vector<casacore::Int>());
    casacore::ImageRegion* imgBox = new casacore::ImageRegion( box );
    std::shared_ptr<casacore::SubImage<casacore::Float> > boxImage( new casacore::SubImage<Float>(*image, *imgBox ) );

    casa::ImageStatsCalculator calc( boxImage, &region, "", true);
    calc.setList(false);
    Record result = calc.calculate();
    const casacore::String blcKey( "blc");
    const casacore::String trcKey( "trc");
    _insertScalar( result, "npts", Carta::Lib::StatInfo::StatType::FrameCount, stats );
    _insertScalar( result, "sum", Carta::Lib::StatInfo::StatType::Sum, stats );
    _insertScalar( result, "sumsq", Carta::Lib::StatInfo::StatType::SumSq, stats );
    _insertScalar( result, "min", Carta::Lib::StatInfo::StatType::Min, stats );
    _insertScalar( result, "max", Carta::Lib::StatInfo::StatType::Max, stats );
    _insertScalar( result, "mean", Carta::Lib::StatInfo::StatType::Mean, stats );
    _insertScalar( result, "sigma", Carta::Lib::StatInfo::StatType::Sigma, stats );
    _insertScalar( result, "rms", Carta::Lib::StatInfo::StatType::RMS, stats );
    _insertScalar( result, "flux", Carta::Lib::StatInfo::StatType::FluxDensity, stats );
    _insertList( result, blcKey, Carta::Lib::StatInfo::StatType::Blc, stats, slice );
    _insertList( result, trcKey, Carta::Lib::StatInfo::StatType::Trc, stats, slice );
    _insertList( result, "minpos", Carta::Lib::StatInfo::StatType::MinPos, stats, slice );
    _insertList( result, "maxpos", Carta::Lib::StatInfo::StatType::MaxPos, stats, slice );
    _insertString( result, "blcf", Carta::Lib::StatInfo::StatType::Blcf, stats );
    _insertString( result, "trcf", Carta::Lib::StatInfo::StatType::Trcf, stats );
    _insertString( result, "minposf", Carta::Lib::StatInfo::StatType::MinPosf, stats );
    _insertString( result, "maxposf", Carta::Lib::StatInfo::StatType::MaxPosf, stats );

    //Put in an identifier.
    if ( result.isDefined( blcKey ) && result.isDefined( trcKey ) ){
        casacore::Vector<int> blcArray = result.asArrayInt( blcKey );
        QString blcVal = _vectorToString( blcArray, slice );
        casacore::Vector<int> trcArray = result.asArrayInt( trcKey );
        QString trcVal = _vectorToString( trcArray, slice );
        QString idVal = regionType + ": ";
        // Note: It is meaningless to show the statistics for a "Point" region, this may need to correct in future.
        if ( blcVal != trcVal ){
            idVal = idVal + blcVal + " -> " + trcVal;
        }
        Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::Name );
        info.setValue( idVal );
        info.setImageStat( false );
        stats.append( info );
    }
}



void
StatisticsCASARegion::_insertList( const casacore::Record& result, const casacore::String& key,
        Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats,
        const std::vector<int>& slice ){
    if ( result.isDefined( key) ){
        casacore::Vector<int> valArray = result.asArrayInt( key );
        QString val = _vectorToString( valArray, slice );
        if ( !val.isEmpty()){
            Carta::Lib::StatInfo info( statType );
            info.setValue( val );
            stats.append( info );
        }
    }
}


void
StatisticsCASARegion::_insertScalar( const casacore::Record& result, const casacore::String& key,
        Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats ){
    if ( result.isDefined( key ) ){
        casacore::Vector<double> valArray = result.asArrayDouble(key);
        if ( valArray.nelements() > 0 ){
            Carta::Lib::StatInfo info( statType );
            info.setValue( QString::number( valArray[0] ) );
            stats.append( info );
        }
    }
}

void
StatisticsCASARegion::_insertString( const casacore::Record& result, const casacore::String& key,
        Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats ){
    if ( result.isDefined( key ) ){
        casacore::String valStr = result.asString(key);
        Carta::Lib::StatInfo info( statType );
        info.setValue( valStr.c_str() );
        stats.append( info );
    }
}

QString StatisticsCASARegion::_vectorToString( const casacore::Vector<int>& valArray,
        const std::vector<int>& slice ){
    int elementCount = valArray.nelements();
    QString val("[");
    for ( int i = 0; i < elementCount; i++ ){
        if ( slice[i] == -1 ) {
            val = val + QString::number(valArray[i]);
        } else {
            val = val + QString::number(slice[i]);
        }
        if ( i < elementCount - 1 ){
            val = val + ", ";
        }
    }
    val = val + "]";
    return val;
}


StatisticsCASARegion::~StatisticsCASARegion() {

}
