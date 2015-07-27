/***
 * Maintains a set of contours and directs rendering them.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"


namespace Carta {
namespace Lib {
    class IContourGeneratorService;
}
}

namespace Carta {

namespace Data {

class Contour;

class DataContours : public QObject, public Carta::State::CartaObject {

Q_OBJECT

public:

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

    virtual ~DataContours();

    const static QString CLASS_NAME;
    const static QString CONTOURS;
    const static QString CONTOUR_DRAW;

private:

    void _initializeDefaultState();

    std::vector<Contour*> m_contours;

    static bool m_registered;

    DataContours( const QString& path, const QString& id );

    class Factory;

    std::shared_ptr<Carta::Lib::IContourGeneratorService> m_contourService;

	DataContours( const DataContours& other);
	DataContours& operator=( const DataContours& other );
};
}
}
