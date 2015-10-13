/***
 * List of all available methods for generating contour levels.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>

namespace Carta {

namespace Data {

class ContourGenerateModes : public Carta::State::CartaObject {

public:

    /**
     * Translates a non case sensitive method of generating contours into one
     * that is case sensitive.
     * @param method - a method of generating contours that may not have the proper capitalization.
     * @return - a recognized method of generating contours.
     */
    QString getGenerateMethod( const QString& method );

    /**
     * Return the default method for generating contours.
     * @return  the default contour generating method.
     */
    QString getModeDefault() const;

    virtual ~ContourGenerateModes();


    const static QString CLASS_NAME;
    const static QString MODE_RANGE;
    const static QString MODE_MINIMUM;
    const static QString MODE_PERCENTILE;

private:

    void _initializeDefaultState();

    std::vector<QString> m_limitModes;

    const static QString LIMIT_MODE;

    static bool m_registered;

    ContourGenerateModes( const QString& path, const QString& id );

    class Factory;

	ContourGenerateModes( const ContourGenerateModes& other);
	ContourGenerateModes& operator=( const ContourGenerateModes& other );
};

}
}
