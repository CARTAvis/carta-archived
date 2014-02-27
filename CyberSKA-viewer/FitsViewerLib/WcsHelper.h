/**
 * Purpose of WcsHelper is to format/convert coordinates/values
 *
 * This is an obsolete class and will be removed soon in favor of WcsHero
 */

#ifndef WCS_HELPER_H
#define WCS_HELPER_H

#include <QString>
#include <vector>
#include <memory>
#include "../CoordinatesLib/WcsHero.h"

class WcsHelperAst {

public:

    struct FormattedCoordinate {
        QString raLabel, raValue, decLabel, decValue, type;
    };

    struct LabelAndUnit {
        QString labelTxt, unitTxt;
        QString labelHtml, unitHtml;
    };

//    // supported coordinate systems
//    enum CoordinateSystemType {
//        J2000, B1950, ICRS, GALACTIC, ECLIPTIC
//    };


    WcsHelperAst();

    ~WcsHelperAst();

    /// get the current wcs, empty string if unknown
    QString getWCS();

    /// set the wcs (follow up with getWCS to see what was actually set)
    void setWCS( QString wcs);

    /// format the given value with proper units
    QString formatValue( double val);

    /// convert from pixel to world
    std::vector<double> toWorld( double x, double y, double frame);

    /// formats FITS coordinates (all values)
    QStringList formatCursor( double x, double y, double frame, bool withLabels = true);

    /// format FITS coordinates in one of the known coordinate systems
    FormattedCoordinate formatCoordinate( double x, double y);

    /// get labels and units for all dimensons
    std::vector< LabelAndUnit > getLabels();

    bool openFile( const QString & qfname);

    /// basic unit replacement
    QString unit2nice( const QString & u);
    QString csystem2html( const QString & cs);

    /// get value unit
    QString valueUnit();

    /// get the total flux unit
    QString totalFluxDensityUnit();

    /// get a hero ref
    WcsHero::Hero::SharedPtr hero();

private:

    std::shared_ptr< WcsHero::Hero > m_wcsHero;
    QString m_valueUnit, m_totalFluxDensityUnit;

};

// forward declarations
namespace casa {
class CoordinateSystem;
};


class WcsHelperCasaCore {

public:

    struct FormattedCoordinate {
        QString raLabel, raValue, decLabel, decValue, type;
    };

    struct LabelAndUnit {
        QString labelTxt, unitTxt;
        QString labelHtml, unitHtml;
    };

    // supported coordinate systems
    enum CoordinateSystemType {
        J2000, B1950, ICRS, GALACTIC, ECLIPTIC
    };


    WcsHelperCasaCore();

    ~WcsHelperCasaCore();

    /// get the current wcs, empty string if unknown
    QString getWCS();

    /// set the wcs (follow up with getWCS to see what was actually set)
    void setWCS( QString wcs);

    /// format the given value with proper units
    QString formatValue( double val);

    /// formats image coordinates (all values)
    QStringList formatCursor( double x, double y, double frame, bool withLabels = true);

    /// convert from pixel to world
    std::vector<double> toWorld( double x, double y, double frame);

    /// format the coordinate in one of the known coordinate systems
    FormattedCoordinate formatCoordinate( double x, double y);

    /// format all other 'things' other than directional coordinates, if any
    /// i.e. stokes, frequency, etc
    QStringList formatNonDirection( double x, double y, int frame);

    /// get labels and units for all dimensons
    std::vector< LabelAndUnit > getLabels();

    bool openFile( const QString & qfname);

    /// basic unit replacement
    QString unit2nice( const QString & u);
    QString csystem2html( const QString & cs);
    QString csystemLabel2html( const QString & s);

    /// get value unit
    QString valueUnit();

    /// get the total flux unit
    QString totalFluxDensityUnit();

private:

    casa::CoordinateSystem * m_cs;
    QString m_valueUnit, m_totalFluxDensityUnit;

};

typedef WcsHelperAst WcsHelper;

#endif // WCS_HELPER_H
