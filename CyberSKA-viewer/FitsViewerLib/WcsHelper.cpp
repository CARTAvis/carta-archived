#include "WcsHelper.h"

#include <QStringList>
#include <QTime>
#include <images/Images/ImageOpener.h>
#include <images/Images/FITSImage.h>
#include <casa/Exceptions/Error.h>
#include <images/Images/ImageExpr.h>
#include <images/Images/ImageExprParse.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Quanta.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <measures/Measures/MCDirection.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MCEpoch.h>
#include <casa/Quanta/MVAngle.h>
#include <coordinates/Coordinates/CoordinateUtil.h>

#include "common.h"



WcsHelperAst::WcsHelperAst() {
    dbg(1) << "XXXX1 constructing WcsHelperAst";
}

WcsHelperAst::~WcsHelperAst() {
    dbg(1) << "XXXX2 destructing WcsHelperAst";
}

/// get the current wcs, empty string if unknown
QString WcsHelperAst::getWCS()
{
    if( ! m_wcsHero) return "";
    auto sys = m_wcsHero-> getCurentSkyCS();
    return m_wcsHero-> skycs2string( sys);
}

/// set the wcs (follow up with getWCS to see what was actually set)
void WcsHelperAst::setWCS( QString wcs) {
    if( ! m_wcsHero) return;

    dbg(1) << "Setting WCS to " << wcs << "------------------------------";

    WcsHero::Hero::SKYCS skycs = WcsHero::Hero::string2skycs( wcs);
    m_wcsHero-> setSkyCS( skycs);
}


QString WcsHelperAst::formatValue( double val) {
    if( m_valueUnit.isEmpty())
        return QString("%1").arg(val);
    else
        return QString("%1 %2").arg(val).arg( m_valueUnit);
}

QStringList
WcsHelperAst::formatCursor(double x, double y, double frame , bool withLabels)
{
    QStringList res;
    if( ! m_wcsHero) {
        return res;
    }

    WcsHero::Hero & hero = * m_wcsHero;

    std::vector<double> coords { x + 1, y + 1, frame + 1 };
    auto fmt = hero.formatPixelCursor( coords, true);
    if( withLabels) {
        for( auto & inf : hero.axesInfos()) {
            QString val = fmt.takeFirst();
            res.append( QString( "%1:%2%3").arg(inf.htmlSymbol).arg(val).arg(inf.htmlUnit));
        }
    }
    else {
        res = fmt;
    }
    return res;
}

std::vector<double> WcsHelperAst::toWorld(double x, double y, double frame)
{
    std::vector<double> res(3, 0.0);
    if( ! m_wcsHero) {
        return res;
    }

    WcsHero::Hero & hero = * m_wcsHero;
    std::vector<double> coords { x + 1, y + 1, frame + 1 };
    res = hero.fits2world( coords);
    return res;
}

WcsHelperAst::FormattedCoordinate
WcsHelperAst::formatCoordinate(double x, double y)
{
    FormattedCoordinate res;

    if( ! m_wcsHero) {
        return res;
    }

    WcsHero::Hero & hero = * m_wcsHero;
    std::vector<double> coords { x + 1, y + 1 };
    QStringList lst = hero.formatPixelCursor( coords);
    if( lst.length() < 2) return res;

    res.type = getWCS();
    res.raLabel = hero.axesInfos()[0].label;
    res.decLabel = hero.axesInfos()[1].label;
    res.raValue = lst[0];
    res.decValue = lst[1];

    return res;
}

std::vector<WcsHelperAst::LabelAndUnit> WcsHelperAst::getLabels()
{
    std::vector<WcsHelperAst::LabelAndUnit> res;

    if( ! m_wcsHero) {
        return res;
    }

    WcsHero::Hero & hero = * m_wcsHero;
    for( auto & axInf : hero.axesInfos()) {
        LabelAndUnit lu;
        lu.labelTxt = axInf.label;
        lu.labelHtml = axInf.htmlLabel;
        lu.unitTxt = axInf.unit;
        lu.unitHtml = axInf.htmlUnit;
        res.push_back( lu);
    }
    return res;
}

bool WcsHelperAst::openFile( const QString & qfname)
{
    m_valueUnit = "";
    m_totalFluxDensityUnit = "";

    m_wcsHero.reset( WcsHero::Hero::constructFromFitsFile( qfname));
    if( ! m_wcsHero) {
        return false;
    }

    m_valueUnit = m_wcsHero-> bunit();

    if( m_totalFluxDensityUnit.toLower().endsWith("/beam"))
        m_totalFluxDensityUnit.chop( QString("/beam").length());
    else
        m_totalFluxDensityUnit = m_valueUnit + ".beam";

    dbg(1) << "totalFluxUnit = " << totalFluxDensityUnit() << "\n";

    return true;

}

QString WcsHelperAst::unit2nice(const QString & pu)
{
    QString u = pu.simplified();
    QString uu = u.toLower();
    if( uu == "kelvin")
        return "K";

    return u;
}

QString WcsHelperAst::csystem2html(const QString & cs)
{
    QString c = cs.toLower();

    if( c == "j2000") return "J2000";
    if( c == "b1950") return "B1950";
    if( c == "icrs") return "ICRS";
    if( c == "galactic") return "Gal.";
    if( c == "ecliptic") return "Ecl.";
    return cs;
}

QString WcsHelperAst::valueUnit()
{
    return m_valueUnit;
}

QString WcsHelperAst::totalFluxDensityUnit()
{
    return m_totalFluxDensityUnit;
}

WcsHero::Hero::SharedPtr WcsHelperAst::hero()
{
    dbg(1) << "XXXX4 = " << m_wcsHero.get();
    if( ! m_wcsHero) {
        LTHROW( "BAD things will happen now.");
        dbg(1) << "BAD things will happen now.";

    }
    return m_wcsHero;
}


// ===========================================================================
// old code based on casacore
// ===========================================================================


WcsHelperCasaCore::WcsHelperCasaCore() {
    casa::FITSImage::registerOpenFunction ();
    casa::UnitMap::addFITS ();
    casa::UnitMap::putUser ( "Kelvin", casa::UnitVal(1.0,"K"));

    m_cs = 0;
}

WcsHelperCasaCore::~WcsHelperCasaCore() {
    if( m_cs) delete m_cs;
}

/// get the current wcs, empty string if unknown
QString WcsHelperCasaCore::getWCS() {

    if( ! m_cs) return "";

    int dcn = m_cs->directionCoordinateNumber();
    if( dcn >= 0) {
        DirectionCoordinate & dc = const_cast<DirectionCoordinate &>(
                    m_cs-> directionCoordinate( dcn));
        MDirection::Types newmdt;
        dc.getReferenceConversion( newmdt);
        return MDirection::showType( newmdt).c_str();
    } else {
        return "";
    }
}

/// set the wcs (follow up with getWCS to see what was actually set)
void WcsHelperCasaCore::setWCS( QString wcs)
{
    dbg(1) << "casa.setWCS to " << wcs << "\n";

    //        if( ! dc_) return ;

    if( ! m_cs) return;

    casa::String err;
    CoordinateUtil::setDirectionConversion( err, * m_cs, wcs.toStdString());
    return;



}


QString WcsHelperCasaCore::formatValue( double val) {
    if( m_valueUnit.isEmpty())
        return QString("%1").arg(val);
    else
        return QString("%1 %2").arg(val).arg( m_valueUnit);
}

QStringList
WcsHelperCasaCore::formatCursor(double x, double y, double frame , bool withLabels)
{
    QStringList res;
    if( m_cs == 0) {
        return res;
    }

    casa::Vector<casa::Double> world, pixel( m_cs-> nPixelAxes (), 0);
    pixel(0) = x;
    pixel(1) = y;
    pixel(2) = frame;
    //        res << QString("cs world axis %1 ").arg(cs->nWorldAxes ());
    //        res << QString("cs pixel axis %1 ").arg(cs->nWorldAxes ());
    if( m_cs-> toWorld ( world, pixel)) {
        for( size_t i = 0 ; i < pixel.size () ; i ++ ) {
            std::string s;
            int ci, cci;
            m_cs-> findWorldAxis( ci, cci, i);
            if( withLabels) {
                std::string label = CoordinateUtil::axisLabel( m_cs-> coordinate(ci), cci);
                label = csystemLabel2html( label.c_str()).toStdString();
                s += label + ": ";
            }
            if( m_cs-> coordinate( ci).type() == Coordinate::DIRECTION ) {
                const DirectionCoordinate & dc = dynamic_cast<const DirectionCoordinate &>( m_cs-> coordinate(ci));
                MDirection::Types dtype = dc.directionType( true);

                casa::String units;

                // just for formatting purposes we create another dummy direction coordinate
                // otherwise casa will format coordinates based on the original type...
                // i.e. if original type was GALACTIC, and the current type is J2000, the
                // coordinates would still be formatted as a decimal degree.
                try {
                    Matrix<Double> xform(2,2);                                    // 1
                    xform = 0.0; xform.diagonal() = 1.0;                          // 2
                    DirectionCoordinate dcc(dtype,                  // 3
                                            Projection(Projection::CAR),          // 4
                                            135*C::pi/180.0, 60*C::pi/180.0,      // 5
                                            -1*C::pi/180.0, 1*C::pi/180,          // 6
                                            xform,                                // 7
                                            128, 128);                            // 8
                    s += dcc.format( units, Coordinate::DEFAULT, world(i), cci, true, true);
                } catch(...) {}


            } else {
                casa::String units;
                s += m_cs-> format( units, Coordinate::DEFAULT, world(i), i);
                if( ! units.empty()) s += " " + units;
            }

            res << s.c_str ();
        }
        //            res << CoordinateUtil::formatCoordinate( pixel, * cs_).c_str();
    } else {
        for( size_t i = 0 ; i < pixel.size () ; i ++ ) {
            res << QString("%1: %2 %3")
                   .arg( m_cs->worldAxisNames ()(i).c_str())
                   .arg( pixel(i))
                   .arg( m_cs->worldAxisUnits ()(i).c_str());
        }
    }

    return res;
}

std::vector<double> WcsHelperCasaCore::toWorld(double x, double y, double frame)
{
    std::vector<double> res(3, 0.0);
    if( m_cs == 0) {
        return res;
    }

    casa::Vector<casa::Double> world, pixel( m_cs-> nPixelAxes (), 0);
    pixel(0) = x;
    pixel(1) = y;
    pixel(2) = frame;
    //        res << QString("cs world axis %1 ").arg(cs->nWorldAxes ());
    //        res << QString("cs pixel axis %1 ").arg(cs->nWorldAxes ());
    if( m_cs-> toWorld ( world, pixel)) {
        if( world.size() > 0) res[0] = world(0);
        if( world.size() > 1) res[1] = world(1);
        if( world.size() > 2) res[2] = world(2);
    }

    return res;
}

WcsHelperCasaCore::FormattedCoordinate
WcsHelperCasaCore::formatCoordinate(double x, double y)
{
    FormattedCoordinate res;

    if( m_cs == 0) {
        return res;
    }

    res.type = getWCS();

    casa::Vector<casa::Double> world, pixel( m_cs-> nPixelAxes (), 0);
    pixel(0) = x;
    pixel(1) = y;
    if( m_cs-> toWorld ( world, pixel)) {
        for( size_t i = 0 ; i < pixel.size () ; i ++ ) {
            int ci, cci;
            std::string value;

            m_cs-> findWorldAxis( ci, cci, i);

            if( m_cs-> coordinate( ci).type() != Coordinate::DIRECTION ) continue;

            const DirectionCoordinate & dc = dynamic_cast<const DirectionCoordinate &>( m_cs-> coordinate(ci));


            std::string label = CoordinateUtil::axisLabel( m_cs-> coordinate(ci), cci).c_str();

            MDirection::Types dtype = dc.directionType( true);
            casa::String units;

            // just for formatting purposes we create another dummy direction coordinate
            // otherwise casa will format coordinates based on the original type...
            // i.e. if original type was GALACTIC, and the current type is J2000, the
            // coordinates would still be formatted as a decimal degree.
            try {
                Matrix<Double> xform(2,2);                                    // 1
                xform = 0.0; xform.diagonal() = 1.0;                          // 2
                DirectionCoordinate dcc(dtype,                  // 3
                                        Projection(Projection::CAR),          // 4
                                        135*C::pi/180.0, 60*C::pi/180.0,      // 5
                                        -1*C::pi/180.0, 1*C::pi/180,          // 6
                                        xform,                                // 7
                                        128, 128);                            // 8
                value = dcc.format( units, Coordinate::DEFAULT, world(i), cci, true, true);
            } catch(...) {

            }

            if( cci == 0) {
                res.raLabel = label.c_str();
                res.raValue = value.c_str();
            } else {
                res.decLabel = label.c_str();
                res.decValue = value.c_str();
            }
        }
    }

    // prettify the labels/system
    res.type = csystem2html( res.type);
    res.raLabel = csystemLabel2html( res.raLabel);
    res.decLabel = csystemLabel2html( res.decLabel);

    return res;
}


QStringList
WcsHelperCasaCore::formatNonDirection( double x, double y, int frame )
{
    QStringList res;

    if( m_cs == 0) {
        return res;
    }

    casa::Vector<casa::Double> world, pixel( m_cs-> nPixelAxes (), 0);
    pixel(0) = x;
    pixel(1) = y;
    pixel(2) = frame;
    if( m_cs-> toWorld ( world, pixel)) {
        for( size_t i = 0 ; i < pixel.size () ; i ++ ) {
            std::string s;
            int ci, cci;
            m_cs-> findWorldAxis( ci, cci, i);
            if( m_cs-> coordinate( ci).type() == Coordinate::DIRECTION ) continue;
            s += CoordinateUtil::axisLabel( m_cs-> coordinate(ci), cci);
            casa::String units;
            s += m_cs-> format( units, Coordinate::DEFAULT, world(i), i);
            if( ! units.empty()) s += " " + units;
            res << s.c_str ();
        }
    } else {
        for( size_t i = 0 ; i < pixel.size () ; i ++ ) {
            res << QString("%1: %2 %3")
                   .arg( m_cs->worldAxisNames ()(i).c_str())
                   .arg( pixel(i))
                   .arg( m_cs->worldAxisUnits ()(i).c_str());
        }
    }
    return res;
}

std::vector<WcsHelperCasaCore::LabelAndUnit> WcsHelperCasaCore::getLabels()
{
    std::vector<WcsHelperCasaCore::LabelAndUnit> res;

    if( m_cs == 0) {
        return res;
    }

    casa::Vector<casa::Double> world, pixel( m_cs-> nPixelAxes (), 0);
    if( m_cs-> toWorld ( world, pixel)) {
        for( size_t i = 0 ; i < pixel.size () ; i ++ ) {
            LabelAndUnit lu;
            std::string s;
            int ci, cci;
            m_cs-> findWorldAxis( ci, cci, i);
                lu.labelTxt = CoordinateUtil::axisLabel( m_cs-> coordinate(ci), cci).c_str();
                lu.labelHtml = csystemLabel2html( lu.labelTxt);
            if( m_cs-> coordinate( ci).type() == Coordinate::DIRECTION ) {
                const DirectionCoordinate & dc = dynamic_cast<const DirectionCoordinate &>( m_cs-> coordinate(ci));
                MDirection::Types dtype = dc.directionType( true);

                casa::String units;

                // just for formatting purposes we create another dummy direction coordinate
                // otherwise casa will format coordinates based on the original type...
                // i.e. if original type was GALACTIC, and the current type is J2000, the
                // coordinates would still be formatted as a decimal degree.
                try {
                    Matrix<Double> xform(2,2);                                    // 1
                    xform = 0.0; xform.diagonal() = 1.0;                          // 2
                    DirectionCoordinate dcc(dtype,                  // 3
                                            Projection(Projection::CAR),          // 4
                                            135*C::pi/180.0, 60*C::pi/180.0,      // 5
                                            -1*C::pi/180.0, 1*C::pi/180,          // 6
                                            xform,                                // 7
                                            128, 128);                            // 8
                    s += dcc.format( units, Coordinate::DEFAULT, world(i), cci, true, true);
                    lu.unitTxt = units.c_str();
                } catch(...) {}
            } else {
                casa::String units;
                s += m_cs-> format( units, Coordinate::DEFAULT, world(i), i);
                if( ! units.empty()) s += " " + units;
                lu.unitTxt = units.c_str();
            }
            lu.unitHtml = unit2nice( lu.unitTxt);

            res.push_back( lu);
        }
    } else {
        for( size_t i = 0 ; i < pixel.size () ; i ++ ) {
            LabelAndUnit lu;
            lu.labelTxt = ('X' + i);
            lu.labelHtml = lu.labelTxt;
            res.push_back( lu);
        }
    }

    return res;
}

bool WcsHelperCasaCore::openFile( const QString & qfname)
{
    if( ! m_cs) delete m_cs;
    m_cs = 0;
    //        if( ! dc_) delete dc_;
    //        dc_ = 0;
    m_valueUnit = "";
    //        wcs_ = "";

    std::string fname = qfname.toStdString ();
    //        std::ostream & dbg(1) = std::cerr;

    dbg(1) << "Opening (casa)image " << fname << "\n";

    QTime time; time.start();
    casa::ImageInterface<casa::Float> * img = 0;
    try {
        img = new casa::FITSImage( fname);
    } catch (...) {
    }

    if( ! img) {
        dbg(1) << "Casa failed to open the image...\n"
               << "We will not have WCS functionality.\n";
        return false;
    }

    dbg(1) << "\t-casa loaded image successfuly in " << time.elapsed()/1000.0 << " seconds\n";
    // make a copy of the image's coordinate system (for cursor formatting)
    m_cs = new casa::CoordinateSystem( img->coordinates ());

    // make our own copy of the direction coordinate (if there is one)
    int dcn = m_cs->directionCoordinateNumber();
    dbg(1) << "\t-direction coordinate index = " << dcn << "\n";
    if( dcn >= 0) {
        const DirectionCoordinate & dc = m_cs-> directionCoordinate( dcn);
        dbg(1) << "\t-direction type = " << casa::MDirection::showType(dc.directionType()) << "\n";
        dbg(1) << "\t-projection = " << dc.projection().name() << "\n";
        setWCS( MDirection::showType( dc.directionType()).c_str());
    }
    m_valueUnit = img->units ().getName ().c_str ();
    // now do some basic units substitution
    m_valueUnit = unit2nice( m_valueUnit);
    // try to figure out the total flux density unit
    // TODO: find a more formal approach for this
    m_totalFluxDensityUnit = m_valueUnit;
    if( m_totalFluxDensityUnit.toLower().endsWith("/beam"))
        m_totalFluxDensityUnit.chop( QString("/beam").length());
    else
        m_totalFluxDensityUnit = m_valueUnit + ".beam";

    delete img;

    dbg(1) << "totalFluxUnit = " << totalFluxDensityUnit() << "\n";

    return true;

}

QString WcsHelperCasaCore::unit2nice(const QString & pu)
{
    QString u = pu.simplified();
    QString uu = u.toLower();
    if( uu == "kelvin")
        return "K";

    return u;
}

QString WcsHelperCasaCore::csystem2html(const QString & cs)
{
    QString c = cs.toLower();

    if( c == "j2000") return "J2000";
    if( c == "b1950") return "B1950";
    if( c == "icrs") return "ICRS";
    if( c == "galactic") return "Gal.";
    if( c == "ecliptic") return "Ecl.";
    return cs;
}

QString WcsHelperCasaCore::csystemLabel2html(const QString & label)
{
    QString l = label.toLower();
    if( l == "j2000 right ascension") return "&alpha;";
    if( l == "j2000 declination") return "&delta;";
    if( l == "b1950 declination") return "&delta;";
    if( l == "b1950 right ascension") return "&alpha;";
    if( l == "icrs declination") return "&delta;";
    if( l == "icrs right ascension") return "&alpha;";
    if( l == "galactic longitude") return "l";
    if( l == "galactic latitude") return "b";
    if( l == "ecliptic longitude") return "&lambda;";
    if( l == "ecliptic latitude") return "&beta;";
    if( l.startsWith("topo frequency")) return "Freq";
    if( l.startsWith("lsrk frequency")) return "Freq";
    if( l.startsWith("frequenc")) return "Freq";

    return label;
}

QString WcsHelperCasaCore::valueUnit()
{
    return m_valueUnit;
}

QString WcsHelperCasaCore::totalFluxDensityUnit()
{
    return m_totalFluxDensityUnit;
}
