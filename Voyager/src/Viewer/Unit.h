#ifndef UNIT_H_
#define UNIT_H_

/**
 * Similar to the UnitAnalyzer class in the CyberSKA-viewer. Adds display capability
 * (html and scales) to casa unit class.
 */
class Unit {
public:
	Unit();
	virtual ~Unit();
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
private:
	//May use a casa unit class to get the full power of casa.
	Unit casaUnit;
};

#endif /* UNIT_H_ */
