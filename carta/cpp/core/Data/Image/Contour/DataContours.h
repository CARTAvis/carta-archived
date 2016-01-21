/***
 * Maintains a set of contours and directs rendering them.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Contour.h"
#include <set>

namespace Carta {
namespace Lib {
    class IContourGeneratorService;
}
}

namespace Carta {

namespace Data {

class GeneratorState;

class DataContours : public QObject, public Carta::State::CartaObject {

friend class ContourControls;
friend class ControllerData;

Q_OBJECT

public:

    /**
     * Return the number of levels in this contour set.
     * @return a count of the levels in the contour set.
     */
    int getLevelCount() const;

    /**
     * Return a list of levels in this contour set.
     * @return - the list of contour levels.
     */
    std::vector<double> getLevels() const;

    /**
     * Return the name of the contour set.
     * @return - the name of the contour set.
     */
    QString getName() const;



    /**
     * Returns the set of pens used to draw contours.
     * @return a vector of contour pens, one for each contour.
     */
    std::vector<QPen> getPens() const;

    /**
     * Returns true if contours should be drawn; false otherwise.
     * @return true if contours should be drawn; false otherwise.
     */
    bool isContourDraw() const;

    bool operator<( const DataContours& other ) const;

    /**
     * Reset the contour set state.
     * @param state - a string representation of the new state of this
     *      contour set.
     */
    virtual void resetState( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Set the transparency level of the contour lines.
     * @param levels - the contour lines whose transparency will be changed.
     * @param transparency - an integer between 0 and 255.
     * @return an error message if the transparency could not be set; otherwise, and empty string.
     */
    QString setAlpha( std::vector<double>& levels, int transparency );

    /**
     * Set the color of the contour lines.
     * @param levels - the contour lines whose transparency will be changed.
     * @param red - an integer between 0 and 255 indicating the amount of red.
     * @param green - an integer between 0 and 255 indicating the amount of green.
     * @param blue - an integer between 0 and 255 indicating the amount of blue.
     * @return a list of errors if the color could not be set; otherwise, an empty list.
     */
    QStringList setColor( std::vector<double>& levels, int red, int green, int blue );

    /**
     * Set the contours to be drawn.
     * @param contours - a set of contours to draw.
     */
    void setContours( std::set<Contour>& contours );

    /**
     * Update the list of contour levels.
     * @param levels - an updated list of contour levels.
     * @return true if any of the contour levels where changed; false otherwise.
     */
    bool setLevels( std::vector<double>& levels );

    /**
     * Set the drawing style for the given list of contour levels.
     * @param levels - a list of contour levels.
     * @param lineStyle - the draw style to use for the contour levels.
     * @return an error message if the draw style could not be set; an empty string
     *      otherwise.
     */
    QString setLineStyle( std::vector<double>& levels, const QString& lineStyle );

    /**
     * Set the width of the lines used to draw contours.
     * @param levels - a list of contour levels affected by the change.
     * @param thickness - the width of the lines.
     * @return an error message if the draw width could not be set; an empty string otherwise.
     */
    QString setThickness( std::vector<double>& levels, double thickness );

    /**
     * Set whether or not the given contour levels should be visible.
     * @param levels - a list of contour levels whose visibility should be changed.
     * @param visible - true if the contour levels should be visible; false otherwise.
     */
    QString setVisibility( std::vector<double>& levels, bool visible );

    /**
     * Specify a unique name for the contour set.
     * @param name - a unique name for the contour set.
     * @return an error message if the name could not be set; otherwise,
     *      an empty string.
     */
    QString setName( const QString& name );

    virtual ~DataContours();

    const static QString CLASS_NAME;
    const static QString CONTOURS;
    const static QString CONTOUR_DRAW;
    const static QString SET_NAME;


private:
    std::set<Contour> _getContours();
    Contour* _getContour(double level);
    std::shared_ptr<GeneratorState> _getGenerator() const;
    Carta::State::StateInterface _getState() const;
    void _initializeDefaultState();
    void _initializeCallbacks();

    void _updateContourState( );
    void _updateGeneratorState( const std::shared_ptr<GeneratorState>& other );

    std::set<Contour> m_contours;
    std::shared_ptr<GeneratorState> m_generatorState;

    static bool m_registered;

    DataContours( const QString& path, const QString& id );

    class Factory;



	DataContours( const DataContours& other);
	DataContours& operator=( const DataContours& other );
};
}
}
