/**
 *
 **/

#include "IRegion.h"

#include <QFile>
#include <QJsonDocument>

namespace Carta
{
namespace Lib
{
namespace Regions
{



bool
RegionSet::isPointInside( const std::vector < RegionPoint > & pts ) const
{
    return root()-> isPointInside( pts );


    // let's convert the points to QPointF
//    std::vector< QPointF > qpts( pts.size());
//    for( size_t i = 0 ; i < pts.size() ; i ++ ) {
//        qpts[i] = QPointF( pts[i][0], pts[i][1]);
//    }

//    return root()-> isPointInsideCS( qpts );
}



/// return an instance from json
RegionBase *
fromJson( QJsonObject json, RegionBase * parent )
{
    RegionBase * r = nullptr;
    QString type = json[RegionBase::REGION_TYPE].toString();
    try {
        if ( type == Circle::TypeName ) {
            r = new Circle( parent );
        }
        else if ( type == Union::TypeName ) {
            r = new Union( parent );
        }
        else if ( type == Polygon::TypeName ) {
            r = new Polygon( parent );
        }
        else {
            qCritical() << "Unknown json region type" << type;
        }
    }
    catch ( ... ) {
        qCritical() << "Could not parse json region" << json;
    }
    if ( r && r-> initFromJson( json ) ) {
        return r;
    }
    if ( r ) {
        delete r;
    }
    return nullptr;
} // fromJson

void
test1( QString inputFname, QString outputFname )
{
    QFile fp( inputFname );
    if ( ! fp.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Could not open " << inputFname;
        return;
    }
    QByteArray contents = fp.readAll();
    QJsonParseError jerr;
    QJsonDocument jdoc = QJsonDocument::fromJson( contents, & jerr );
    if ( jerr.error != QJsonParseError::NoError ) {
        qWarning() << "Json parse error@" << jerr.offset << ":" << jerr.errorString();
        return;
    }

    Carta::Lib::Regions::RegionBase * bb = Carta::Lib::Regions::fromJson( jdoc.object() );

    // get the outline box
    QRectF bigRect = bb->outlineBox();

    // setup the output image
    QSize outputSize;
    int maxSize = 100;
    if ( bigRect.width() > bigRect.height() ) {
        outputSize = QSize( maxSize, maxSize * bigRect.height() / bigRect.width() );
    }
    else {
        outputSize = QSize( maxSize * bigRect.width() / bigRect.height(), maxSize );
    }
    QImage qimg( outputSize, QImage::Format_ARGB32 );
    qimg.fill( 0 );

    // save image to output
    qimg.save( outputFname );
} // test1
}
}
}

/*static int
apiTest()
{
    Carta::Lib::Regions::Circle * c1 = new Carta::Lib::Regions::Circle( { 5, 5 }, 3 );

    Carta::Lib::Regions::Circle * c2 = new Carta::Lib::Regions::Circle( { 7, 1.5 }, 3.123 );

    qDebug() << "c1 bb=" << c1->outlineBox();
    qDebug() << "c2 bb=" << c2->outlineBox();

    Carta::Lib::Regions::Union * u1 = new Carta::Lib::Regions::Union();
    u1->addChild( c1 );
    u1->addChild( c2 );
    c2->setLineColor( "blue" );

//    c1->setFillColor( "black");
    qDebug() << "u1 bb=" << u1->outlineBox();

    auto json = u1->toJson();
    QJsonDocument jdoc( json );
    QString s = jdoc.toJson( QJsonDocument::Indented );
    qDebug() << "JSON=====================";
    qDebug() << s;
    qDebug() << "=========================";

    // now let's try reverse
    Carta::Lib::Regions::RegionBase * bb = Carta::Lib::Regions::fromJson( json );
    if ( ! bb ) {
        qDebug() << "Ooops";
    }
    else {
        auto json = bb->toJson();
        QJsonDocument jdoc( json );
        QString s = jdoc.toJson( QJsonDocument::Indented );
        qDebug() << "JSON=====================";
        qDebug() << s;
        qDebug() << "=========================";
    }

    return 0;
} */// apiTest

//static int foo = apiTest();
