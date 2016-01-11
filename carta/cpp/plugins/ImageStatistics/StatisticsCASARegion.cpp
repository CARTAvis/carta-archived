#include "StatisticsCASARegion.h"
#include "StatisticsCASA.h"
#include "RegionRecordFactory.h"
#include "imageanalysis/ImageAnalysis/ImageStatsCalculator.h"

#include <QDebug>

StatisticsCASARegion::StatisticsCASARegion() {
}


void
StatisticsCASARegion::_insertScalar( const casa::Record& result, const casa::String& key,
        Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats ){
    if ( result.isDefined( key ) ){
        casa::Vector<double> valArray = result.asArrayDouble(key);
        if ( valArray.nelements() > 0 ){
            Carta::Lib::StatInfo info( statType );
            info.setValue( QString::number( valArray[0] ) );
            stats.append( info );
        }
    }
}

void
StatisticsCASARegion::_insertString( const casa::Record& result, const casa::String& key,
        Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats ){
    if ( result.isDefined( key ) ){
        casa::String valStr = result.asString(key);
        Carta::Lib::StatInfo info( statType );
        info.setValue( valStr.c_str() );
        stats.append( info );
    }
}

QString StatisticsCASARegion::_vectorToString( const casa::Vector<int>& valArray ){
    int elementCount = valArray.nelements();
    QString val("[");
    if ( elementCount > 0 ){
        for ( int i = 0; i < elementCount; i++ ){
            val = val + QString::number(valArray[i]);
            if ( i < elementCount - 1 ){
                val = val + ", ";
            }
        }
    }
    val = val + "]";
    return val;
}

void
StatisticsCASARegion::_insertList( const casa::Record& result, const casa::String& key,
        Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats ){
    if ( result.isDefined( key) ){
        casa::Vector<int> valArray = result.asArrayInt( key );
        QString val = _vectorToString( valArray );
        if ( !val.isEmpty()){
            Carta::Lib::StatInfo info( statType );
            info.setValue( val );
            stats.append( info );
        }
    }
}



QList<Carta::Lib::StatInfo>
StatisticsCASARegion::getStats( const casa::ImageInterface<casa::Float>* image,
        Carta::Lib::RegionInfo& regionInfo ){
    QList<Carta::Lib::StatInfo> stats;
    //For now hard-code the region.
    std::vector<std::pair<double,double> > corners = regionInfo.getCorners();
    const casa::CoordinateSystem& cSys = image->coordinates();
    casa::Record regionRecord = RegionRecordFactory:: getRegionRecord( Carta::Lib::RegionInfo::RegionType::Polygon,
          cSys, corners );
    _getStatsFromCalculator( image, regionRecord, stats );
    return stats;
}

void StatisticsCASARegion::_getStatsFromCalculator( const casa::ImageInterface<casa::Float>* image,
       const casa::Record& region, QList<Carta::Lib::StatInfo>& stats ){
    std::shared_ptr<const casa::ImageInterface<casa::Float> > imagePtr( image->cloneII() );
    ImageStatsCalculator calc( imagePtr, &region, "", false);
    calc.setVerbose(True);
    calc.setList(False);
    Record result = calc.calculate();
    const casa::String blcKey( "blc");
    const casa::String trcKey( "trc");
    _insertScalar( result, "npts", Carta::Lib::StatInfo::StatType::FrameCount, stats );
    _insertScalar( result, "sum", Carta::Lib::StatInfo::StatType::Sum, stats );
    _insertScalar( result, "sumsq", Carta::Lib::StatInfo::StatType::SumSq, stats );
    _insertScalar( result, "min", Carta::Lib::StatInfo::StatType::Min, stats );
    _insertScalar( result, "max", Carta::Lib::StatInfo::StatType::Max, stats );
    _insertScalar( result, "mean", Carta::Lib::StatInfo::StatType::Mean, stats );
    _insertScalar( result, "sigma", Carta::Lib::StatInfo::StatType::Sigma, stats );
    _insertScalar( result, "rms", Carta::Lib::StatInfo::StatType::RMS, stats );
    _insertScalar( result, "flux", Carta::Lib::StatInfo::StatType::FluxDensity, stats );
    _insertList( result, blcKey, Carta::Lib::StatInfo::StatType::Blc, stats );
    _insertList( result, trcKey, Carta::Lib::StatInfo::StatType::Trc, stats );
    _insertList( result, "minpos", Carta::Lib::StatInfo::StatType::MinPos, stats );
    _insertList( result, "maxpos", Carta::Lib::StatInfo::StatType::MaxPos, stats );
    _insertString( result, "blcf", Carta::Lib::StatInfo::StatType::Blcf, stats );
    _insertString( result, "trcf", Carta::Lib::StatInfo::StatType::Trcf, stats );
    _insertString( result, "minposf", Carta::Lib::StatInfo::StatType::MinPosf, stats );
    _insertString( result, "maxposf", Carta::Lib::StatInfo::StatType::MaxPosf, stats );

    //Put in an identifier.
    if ( result.isDefined( blcKey ) && result.isDefined( trcKey ) ){
        casa::Vector<int> blcArray = result.asArrayInt( blcKey );
        QString blcVal = _vectorToString( blcArray );
        casa::Vector<int> trcArray = result.asArrayInt( trcKey );
        QString trcVal = _vectorToString( trcArray );
        QString idVal = blcVal + " x " + trcVal;
        Carta::Lib::StatInfo info( Carta::Lib::StatInfo::StatType::Name );
        info.setValue( idVal );
        info.setImageStat( false );
        stats.append( info );
    }
}



StatisticsCASARegion::~StatisticsCASARegion() {

}
