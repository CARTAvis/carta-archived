/***
 * Stores generator settings for a particular contour set.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"


namespace Carta {

namespace Data {

class ContourGenerateModes;
class ContourSpacingModes;

class GeneratorState {
    friend class ContourControls;
    friend class DataContours;

public:

    /**
     * Constructor.
     */
    GeneratorState();

    /**
     * Return the generator state as a string.
     * @return a string representation of the generator state.
     */
    QString getStateString( ) const;

    /**
     * Returns the name of the method used to generate contour levels.
     * @return - the name of the method used to generate contour levels.
     */
    QString getGenerateMethod() const;

    /**
     * Returns the maximum contour level.
     * @return - the maximum contour level.
     */
    double getRangeMax() const;

    /**
     * Returns the minimum contour level.
     * @return - the minimum contour level.
     */
    double getRangeMin() const;

    /**
     * Returns the spacing between contour levels.
     * @return - the spacing between contour levels.
     */
    double getSpacingInterval() const;

    /**
     * Returns how contour levels should be spaced.
     * @return - how contour levels should be spaced.
     */
    QString getSpacingMethod() const;

    /**
     * Returns the number of contour levels.
     * @return - the number of contour levels.
     */
    int getLevelCount() const;

    /**
     * Returns true if negative contour levels should be dashed; false otherwise.
     * @return - whether or not negative contour levels should be dashed.
     */
    bool isDashedNegative() const;

    /**
     * Set whether or not negative contours should be dashed.
     * @param useDash - true if negative contours should be dashed; false if they
     *      should be solid lines.
     */
    void setDashedNegative( bool useDash );

    /**
     * Set the method used to generate contour levels within the set.
     * @param method - an identifier for a method used to generate contour levels.
     * @return - an error message if there was a problem setting the method used to
     *      generate contour levels; an empty string otherwise.
     */
    QString setGenerateMethod( const QString& method );

    /**
     * Set the type of spacing to use between contour levels (linear, logarithmic, etc).
     * @param method - an identifier for the spacing to use between contour levels.
     * @return - an error message if there was a problem setting the spacing; an empty
     *  string otherwise.
     */
    QString setSpacing( const QString& method );

    /**
     * Set the number of contour levels in the set.
     * @param count - the number of contour levels.
     * @return - an error message if the count could not be set; an empty string otherwise.
     */
    QString setLevelCount( int count );

    /**
     * Set the largest contour level.
     * @param value - the intensity or percentage of the largest contour level.
     * @return - an error message if the largest contour level could not be set; an empty string
     *      otherwise.
     */
    QString setLevelMax( double value );

    /**
     * Set the smallest contour level.
     * @param value - the intensity or percentage of the smallest contour level.
     * @return - an error message if the smallest contour level could not be set; an empty string
     *      otherwise.
     */
    QString setLevelMin( double value );



    /**
     * Set the interval used to generate contour levels between a min and a max.
     * @param interval - a fixed interval of spacing to use between contour levels.
     * @return - an error message if there is a problem setting the spacing interval between
     *      contour levels; an empty string otherwise.
     */
    QString setSpacingInterval( double interval );

    virtual ~GeneratorState();


private:
    const static QString DASHED_NEGATIVE;
    const static QString GENERATE_MODE;
    const static QString INTERVAL;
    const static QString LEVEL_COUNT;
    const static QString LEVEL_COUNT_MAX;
    const static QString LEVEL_MIN;
    const static QString LEVEL_MAX;
    const static QString RANGE_MIN;
    const static QString RANGE_MAX;
    const static QString SPACING_MODE;
    const static QString SPACING_INTERVAL;
    const static int LEVEL_COUNT_MAX_VALUE;
    const static double ERROR_MARGIN;


    void _initializeDefaultState();
    void _initializeSingletons( );

    void _updateState( const std::shared_ptr<GeneratorState>& other );

    Carta::State::StateInterface m_state;
    static ContourGenerateModes* m_generateModes;
    static ContourSpacingModes* m_spacingModes;

	GeneratorState( const GeneratorState& other);
	GeneratorState& operator=( const GeneratorState& other );
};
}
}
