/**
 * The purpose of this class is to provide functionality related to physical units.
 *
 * For example, choosing the best prefix and scale factor for a given unit.
 *
 **/

#ifndef UNITANALYZER_H
#define UNITANALYZER_H

#include <QString>
#include <memory>
#include <set>

class AnalyzedUnit
{
public:
    typedef AnalyzedUnit Me;
    typedef std::shared_ptr<Me> SharedPtr;

    // constructor
    AnalyzedUnit( const QString & unit);
    // destructor
    virtual ~AnalyzedUnit();

    /// original unit supplied to analyzer
    QString origUnit();
    /// how we usually display the unit (plain text)
    QString displayUnit();
    /// how we usually display the unit (html version)
    QString displayUnitHtml();
    /// for the given scaling, return the scaled unit (plain text)
    QString scaledUnit();
    /// for the given scaling, return the scaled unit (html version)
    QString scaledUnitHtml();
    /// return the scaling factor after applying scaling
    double scaleFactor();
    /// compute the best scale for the given range
    void computeScale( double min, double max);
    /// compute the best scale for the given value
    void computeScale( double avg);
protected: // override functions
    /// original unit supplied to analyzer
    virtual QString doOrigUnit();
    /// how we usually display the unit (plain text)
    virtual QString doDisplayUnit();
    /// how we usually display the unit (html version)
    virtual QString doDisplayUnitHtml();
    /// for the given scaling, return the scaled unit (plain text)
    virtual QString doScaledUnit();
    /// for the given scaling, return the scaled unit (html version)
    virtual QString doScaledUnitHtml();
    /// return the scaling factor after applying scaling
    virtual double doScaleFactor();
    /// compute the best scale for the given range
    virtual void doComputeScale( double min, double max);
    /// compute the best scale for the given value
    virtual void doComputeScale( double avg);

protected: // data members
    QString m_originalUnit, m_displayUnit, m_displayUnitHtml;
    QString m_scaledUnit, m_scaledUnitHtml;
    double m_scale;
};

// singleton UnitAnalyzer
class UnitAnalyzer
{
public:
    static AnalyzedUnit::SharedPtr analyze( const QString & unit);
protected:
    UnitAnalyzer();
    static UnitAnalyzer * m_instance;
    static UnitAnalyzer * instance();

    bool isInvisible( const QString & unit);
    bool isPrefixable( const QString & unit);
    QString aliasToProper( const QString & unit);

    std::set < QString > m_prefixableUnits;
    AnalyzedUnit::SharedPtr doAnalysis( const QString & unit);
};

namespace UnitAlgorithms {

QString formatAngle( double rad);

}

#endif // UNITANALYZER_H
