#include "StatisticsCASARegion.h"
#include "StatisticsCASA.h"
#include "RegionRecordFactory.h"
#include "imageanalysis/ImageAnalysis/ImageStatsCalculator.h"

#include <QDebug>

StatisticsCASARegion::StatisticsCASARegion() {
}


void
StatisticsCASARegion::_insertScalar( const casa::Record& result, const casa::String& key,
        const QString& label, QMap<QString,QString>& stats ){
    if ( result.isDefined( key ) ){
        casa::Vector<double> valArray = result.asArrayDouble(key);
        if ( valArray.nelements() > 0 ){
            stats.insert( label, QString::number( valArray[0] ) );
        }
    }
}

void
StatisticsCASARegion::_insertString( const casa::Record& result, const casa::String& key,
        const QString& label, QMap<QString,QString>& stats ){
    if ( result.isDefined( key ) ){
        casa::String valStr = result.asString(key);
        stats.insert( label, valStr.c_str() );
    }
}

void
StatisticsCASARegion::_insertList( const casa::Record& result, const casa::String& key,
        const QString& label, QMap<QString,QString>& stats ){
    if ( result.isDefined( key) ){
        casa::Vector<int> valArray = result.asArrayInt( key );
        int elementCount = valArray.nelements();
        if ( elementCount > 0 ){
            QString val("[");
            for ( int i = 0; i < elementCount; i++ ){
                val = val + QString::number(valArray[i]);
                if ( i < elementCount - 1 ){
                    val = val + ", ";
                }
            }
            val = val + "]";
            stats.insert( label, val );
        }
    }
}


QMap<QString,QString>
StatisticsCASARegion::getStats( const casa::ImageInterface<casa::Float>* image,
        Carta::Lib::RegionInfo& regionInfo ){
    QMap<QString,QString> stats;
    //For now hard-code the region.
    std::vector<std::pair<double,double> > corners = regionInfo.getCorners();
    qDebug() << "Corner count="<<corners.size();
    const casa::CoordinateSystem& cSys = image->coordinates();
    casa::Record region = RegionRecordFactory:: getRegionRecord( Carta::Lib::RegionInfo::RegionType::Polygon,
          cSys, corners );
    qDebug() << "Got region";
    cout << "region="<<region<<endl;
    qDebug() << "Making stat calc";
    std::shared_ptr<const casa::ImageInterface<casa::Float> > imagePtr( image->cloneII() );
    ImageStatsCalculator calc( imagePtr, &region, "", false);
    calc.setVerbose(True);
    calc.setList(False);
    qDebug() << "Result of calculation";
    Record result = calc.calculate();
    cout << "Stats="<<result<<endl;
    _insertScalar( result, "npts", "Npts", stats );
    _insertScalar( result, "sum", "Sum", stats );
    _insertScalar( result, "sumsq", "SumSq", stats );
    _insertScalar( result, "min", "Min", stats );
    _insertScalar( result, "max", "Max", stats );
    _insertScalar( result, "mean", "Mean", stats );
    _insertScalar( result, "sigma", "Sigma", stats );
    _insertScalar( result, "rms", "RMS", stats );
    _insertScalar( result, "flux", "Flux", stats );
    qDebug() << "Inserting blc";
    _insertList( result, "blc", "Blc", stats );
    qDebug() << "Inserting trc";
    _insertList( result, "trc", "Trc", stats );
    _insertList( result, "minpos", "Min Pos", stats );
    _insertList( result, "maxpos", "Max Pos", stats );
    _insertString( result, "blcf", "Blcf", stats );
    _insertString( result, "trcf", "Trcf", stats );
    _insertString( result, "minposf", "Min Pos f", stats );
    _insertString( result, "maxposf", "Max Pos f", stats );

    //Put in an identifier.
    //const QString BRACKETS_OPEN( "[");
    //const QString BRACKETS_CLOSE( "]");
    //QString trcStr = BRACKETS_OPEN + stats["trcf"] + BRACKETS_CLOSE;
    //QString blcStr = BRACKETS_OPEN + stats["blcf"] + BRACKETS_CLOSE;
    QString idVal = stats["Blc"] + " x " + stats["Trc"];
    //QString idVal("Hi");
    qDebug() << "Inserting key="<<StatisticsCASA::STAT_ID<<" val="<<idVal;
    stats.insert( StatisticsCASA::STAT_ID, idVal );
    return stats;
}



StatisticsCASARegion::~StatisticsCASARegion() {

}
