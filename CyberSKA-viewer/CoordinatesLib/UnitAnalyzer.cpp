#include "UnitAnalyzer.h"
#include "FitsViewerLib/common.h"
#include <cmath>

UnitAnalyzer * UnitAnalyzer::m_instance = 0;

class InvisibleUnit : public AnalyzedUnit {
public:
    InvisibleUnit( const QString & unit) : AnalyzedUnit(unit) {
        m_scale = 1.0;
        m_displayUnit = "";
        m_displayUnitHtml = "";
        m_scaledUnit = "";
        m_scaledUnitHtml = "";
    }
};

class DegreeUnit : public AnalyzedUnit {
public:
    DegreeUnit()
        : AnalyzedUnit( "deg")
    {
        m_scale = 1.0;
        m_displayUnit = "";
        m_displayUnitHtml = "";
        m_scaledUnit = "";
        m_scaledUnitHtml = "";
    }
};


class PrefixableUnit : public AnalyzedUnit {
public:
    PrefixableUnit( const QString & unit)
        : AnalyzedUnit(unit)
    {
    }

    // AnalyzedUnit interface
protected:
    virtual void doComputeScale(double min, double max) CPP11_OVERRIDE
    {
        return doComputeScale( std::max( fabs(min), fabs(max)));
    }
    virtual void doComputeScale(double avg) CPP11_OVERRIDE
    {
        avg = fabs( avg);
        double nDigitsD = std::log10( avg);
        int nDigits = std::round( nDigitsD);
        struct { QString prefix; double scale; } ps = { "", 1.0};
        if( ! std::isfinite( avg)) ps = { "", 0 };
        else if( nDigits <  -21-1) ps = { "y", -24 };
        else if( nDigits <  -18-1) ps = { "z", -21 };
        else if( nDigits <  -15-1) ps = { "a", -18 };
        else if( nDigits <  -12-1) ps = { "f", -15 };
        else if( nDigits <   -9-1) ps = { "p", -12 };
        else if( nDigits <   -6-1) ps = { "n",  -9 };
        else if( nDigits <   -3-1) ps = { "u",  -6 };
        else if( nDigits <    0-1) ps = { "m",  -3 };
        else if( nDigits <    3) ps = { "",   0 };
        else if( nDigits <    6) ps = { "k",  3 };
        else if( nDigits <    9) ps = { "M",  6 };
        else if( nDigits <   12) ps = { "G",  9 };
        else if( nDigits <   15) ps = { "T", 12 };
        else if( nDigits <   18) ps = { "P", 15 };
        else if( nDigits <   21) ps = { "E", 18 };
        else if( nDigits <   24) ps = { "Z", 21 };
        else                     ps = { "Y", 24 };

        dbg(1) << "avg=" << avg << " nd=" << nDigits << " pf=" << ps.prefix << " sc=" << ps.scale;

        m_scaledUnit = ps.prefix + m_displayUnit;
        m_scaledUnitHtml = ps.prefix + m_displayUnitHtml;
        m_scale = std::pow( 10, - ps.scale);
    }
};

bool UnitAnalyzer::isInvisible(const QString &unit)
{
    if( unit.startsWith( "hh:mm:ss")) return true;
    if( unit.startsWith( "ddd:mm:ss")) return true;
    return false;

}

bool UnitAnalyzer::isPrefixable(const QString & unit)
{
    return m_prefixableUnits.find( unit) != m_prefixableUnits.end();
}

QString UnitAnalyzer::aliasToProper(const QString & p_unit)
{
    QString unit = p_unit.trimmed();
    QString lunit = unit.toLower();
    if( lunit == "kelvin") return "K";
    if( lunit == "meter") return "m";
    if( lunit == "metre") return "m";
    if( lunit == "sec") return "s";
    if( lunit == "jan") return "Jy";

    // if unknown alias, return the unit itself
    return unit;
}

AnalyzedUnit::SharedPtr UnitAnalyzer::doAnalysis(const QString & p_unit)
{
    // first convert a known alias to a proper unit
    QString unit = aliasToProper( p_unit);

    // if the unit is invisible, return an invisible implementation
    if( isInvisible(unit)) {
        return std::make_shared<InvisibleUnit>(unit);
    }

    // if the unit is prefixable, return prefixable implementation
    if( isPrefixable( unit)) {
        return std::make_shared<PrefixableUnit>( unit);
    }

    // special cases for some known units
    if( unit == "degrees") {
        return std::make_shared<InvisibleUnit>( unit);
    }

    // otherwise make a verbatim unit
    return std::make_shared<AnalyzedUnit>( unit);
}

AnalyzedUnit::SharedPtr UnitAnalyzer::analyze(const QString &unit)
{
    return instance()-> doAnalysis( unit);
}

UnitAnalyzer::UnitAnalyzer()
{
    m_prefixableUnits.insert( "m");
    m_prefixableUnits.insert( "g");
    m_prefixableUnits.insert( "s");
    m_prefixableUnits.insert( "rad");
    m_prefixableUnits.insert( "K");
    m_prefixableUnits.insert( "A");
    m_prefixableUnits.insert( "mol");
    m_prefixableUnits.insert( "cd");

    m_prefixableUnits.insert( "sr");
    m_prefixableUnits.insert( "Hz");
    m_prefixableUnits.insert( "N");
    m_prefixableUnits.insert( "J");
    m_prefixableUnits.insert( "W");
    m_prefixableUnits.insert( "C");
    m_prefixableUnits.insert( "V");
    m_prefixableUnits.insert( "Pa");
    m_prefixableUnits.insert( "Ohm");
    m_prefixableUnits.insert( "S");
    m_prefixableUnits.insert( "F");
    m_prefixableUnits.insert( "Wb");
    m_prefixableUnits.insert( "T");
    m_prefixableUnits.insert( "H");
    m_prefixableUnits.insert( "lm");
    m_prefixableUnits.insert( "lx");

}

UnitAnalyzer *UnitAnalyzer::instance()
{
    if( ! m_instance) {
        m_instance = new UnitAnalyzer();
    }
    return m_instance;
}

AnalyzedUnit::AnalyzedUnit(const QString &unit)
{
    m_originalUnit = unit;
    m_displayUnit = unit;
    m_displayUnitHtml = unit;
    m_scaledUnit = unit;
    m_scaledUnitHtml = unit;
    m_scale = 1.0;
}

AnalyzedUnit::~AnalyzedUnit()
{

}

QString AnalyzedUnit::origUnit()
{
    return doOrigUnit();
}

QString AnalyzedUnit::displayUnit()
{
    return doDisplayUnit();
}

QString AnalyzedUnit::displayUnitHtml()
{
    return doDisplayUnitHtml();
}

QString AnalyzedUnit::scaledUnit()
{
    return doScaledUnit();
}

QString AnalyzedUnit::scaledUnitHtml()
{
    return doScaledUnitHtml();
}

double AnalyzedUnit::scaleFactor()
{
    return doScaleFactor();
}

void AnalyzedUnit::computeScale(double min, double max)
{
    doComputeScale( min, max);
}

void AnalyzedUnit::computeScale(double avg)
{
    doComputeScale( avg);
}

QString AnalyzedUnit::doOrigUnit()
{
    return m_originalUnit;
}

QString AnalyzedUnit::doDisplayUnit()
{
    return m_displayUnit;
}

QString AnalyzedUnit::doDisplayUnitHtml()
{
    return m_displayUnitHtml;
}

QString AnalyzedUnit::doScaledUnit()
{
    return m_scaledUnit;
}

QString AnalyzedUnit::doScaledUnitHtml()
{
    return m_scaledUnitHtml;
}

double AnalyzedUnit::doScaleFactor()
{
    return m_scale;
}

void AnalyzedUnit::doComputeScale(double /*min*/, double /*max*/)
{
    // default implementation does nothing at all
}

void AnalyzedUnit::doComputeScale(double /*avg*/)
{
    // default implementation does nothing at all
}

namespace UnitAlgorithms {

QString formatAngle(double rad)
{
    double deg = 180 * rad / M_PI;
    int arcmin = floor( deg * 60);
    deg -= arcmin / 60.0;
    double arcsec = deg * 60 * 60;
    QString res = "";
    if( arcmin > 0) res = QString::number(arcmin) + "'";
    res += QString::number(arcsec) + "\"";
    return res;
}

} // namespace UnitAlgorithms

