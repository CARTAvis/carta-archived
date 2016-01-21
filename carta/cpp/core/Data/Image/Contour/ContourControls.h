/***
 * Entry point for clients wishing to change contour settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Contour.h"
#include <set>

namespace Carta {

namespace Data {

class DataContours;
class GeneratorState;
class IPercentIntensityMap;

class ContourControls : public QObject, public Carta::State::CartaObject{

    friend class Controller;

    Q_OBJECT

public:

    /**
     * Delete the contour set with the indicated name.
     * @param contourSetName - a unique identifier for a contour set.
     * @return an error message if the contour set could not be deleted; an
     *      empty string otherwise.
     */
    QString deleteContourSet( const QString& contourSetName );

    /**
     * Generate a set of contours with the given name.
     * @param contourSetName - the name of the contour set to generate.
     * @return - an error message if there was a problem generating the contour
     *  set; an empty string otherwise.
     */
    QString generateContourSet( const QString& contourSetName );

    /**
     * Select a specific contour set.
     * @param name - a name for a contour set.
     */
    void selectContourSet( const QString& name );

    /**
     * Set the transparency level of the contours within the specified set.
     * @param contourName - an identifier for a contour set.
     * @param levels - the contours levels within the set whose transparency should be changed.
     * @param transparency - the transparency between 0 and 255.
     * @return an error message if the transparency could not be set; otherwise, an empty string.
     */
    QString setAlpha( const QString& contourName, std::vector<double>& levels, int transparency );

    /**
     * Set the color of the contours within the specified set.
     * @param contourName - an identifier for a contour set.
     * @param levels - the contours levels within the set affected by the change.
     * @param red - the amount of red between 0 and 255.
     * @param green - the amount of green between 0 and 255.
     * @param blue - the amount of blue between 0 and 255.
     * @return a list of errors if the color could not be set; otherwise, an empty list.
     */
    QStringList setColor( const QString& contourName, std::vector<double>& levels,
            int red, int green, int blue );

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
     * Update the contour levels within the given contour set.
     * @param contourName - the name of a contour set.
     * @param levels - an updated list of contour levels.
     * @return an error message if the contour levels could not be updated; otherwise,
     *      an empty string.
     */
    QString setLevels( const QString& contourName, std::vector<double>& levels );

    /**
     * Set the draw style for the contour levels in the given set.
     * @param contourName - the name of a contour set.
     * @param levels - a list of contour levels whose line style should change.
     * @param lineStyle - an identifier for the draw style to use.
     * @return an error message if the line style could not be set; otherwise, an
     *      empty string.
     */
    QString setLineStyle( const QString& contourName, std::vector<double>& levels,
            const QString& lineStyle );

    /**
     * Set the object capable of mapping intensities to percentiles and vice/versa.
     * @param mapper - the object capable of mapping between percentiles and intensities.
     */
    void setPercentIntensityMap( IPercentIntensityMap* mapper );

    /**
     * Set the interval used to generate contour levels between a min and a max.
     * @param interval - a fixed interval of spacing to use between contour levels.
     * @return - an error message if there is a problem setting the spacing interval between
     *      contour levels; an empty string otherwise.
     */
    QString setSpacingInterval( double interval );

    /**
     * Set the width of the lines used to draw the contours in the identified set.
     * @param contourName - an identifier for a contour set.
     * @param levels - the levels within the contour set that are affected.
     * @param thickness - the width of the contour lines to be drawn.
     * @return an error message if there was a problem setting the width of the contour
     *      lines; otherwise, an empty string.
     */
    QString setThickness( const QString& contourName, std::vector<double>& levels,
            double thickness );

    /**
     * Set the visibility of the contour levels in the set.
     * @param contourName - an identifier for a contour set.
     * @param levels - a list of contour levels whose visibility will be affected.
     * @param visible - true if the levels should be visible; false otherwise.
     */
    QString setVisibility( const QString& contourName, std::vector<double>& levels, bool visible );
    virtual ~ContourControls();
    const static QString CLASS_NAME;

signals:
    void drawContoursChanged();

private:
    const static QString CONTOUR_SETS;
    const static QString CONTOUR_SET_NAME;
    const static QString LEVEL_LIST;
    const static QString LEVEL_SEPARATOR;

    void _addContourSet( const std::vector<double>& levels, const QString& contourSetName );

    QString _generateRange( const QString& contourSetName);
    QString _generateMinimum( const QString& contourSetName );
    QString _generatePercentile( const QString& contourSetName );

    DataContours* _getContour( const QString& setName );
    std::vector<double> _getLevels( double minLevel, double maxLevel ) const;
    std::vector<double> _getLevelsMinMax(double max, QString& error ) const;

    void _initializeDefaultState();
    void _initializeCallbacks();

    bool _isDuplicate( const QString& contourSetName ) const;

    void _setDrawContours( std::shared_ptr<DataContours> contours );

    void _updateContourSetState();

    static bool m_registered;
    //Used
    IPercentIntensityMap* m_percentIntensityMap;


    ContourControls( const QString& path, const QString& id );

    class Factory;

    std::set<shared_ptr<DataContours> > m_dataContours;

    std::shared_ptr<DataContours> m_drawContours;
    std::shared_ptr<GeneratorState> m_generatorState;

    Carta::State::StateInterface m_stateData;

	ContourControls( const ContourControls& other);
	ContourControls& operator=( const ContourControls& other );
};
}
}
