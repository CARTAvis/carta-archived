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



class DataContours : public QObject, public Carta::State::CartaObject {

friend class ContourControls;

Q_OBJECT

public:

    QString getName() const;

    /**
     * Returns the service capable of rendering contours.
     * @return the contour rendering service.
     */
    std::shared_ptr<Carta::Lib::IContourGeneratorService> _getRenderer();

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
     * Set the contours to be drawn.
     * @param contours - a set of contours to draw.
     */
    void setContours( std::set<Contour>& contours );

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
    Carta::State::StateInterface _getState() const;
    void _initializeDefaultState();

    void _initializeCallbacks();

    std::set<Contour> m_contours;


    static bool m_registered;

    DataContours( const QString& path, const QString& id );

    class Factory;

    std::shared_ptr<Carta::Lib::IContourGeneratorService> m_contourService;

	DataContours( const DataContours& other);
	DataContours& operator=( const DataContours& other );
};
}
}
