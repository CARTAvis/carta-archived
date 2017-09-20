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

class ContourTypes : public Carta::State::CartaObject {

public:

    /**
     * Translates a non case sensitive type of generating contours into one
     * that is case sensitive.
     * @param type - a type of generating contours that may not have the proper capitalization.
     * @return - a recognized type of generating contours.
     */
    QString getContourType(const QString& type);

    /**
     * Return the default type for generating contours.
     * @return the default contour type.
     */
    QString getContourTypeDefault() const;

    virtual ~ContourTypes();

    const static QString CLASS_NAME;
    const static QString MODE_NOLINECOMBOPT;
    const static QString MODE_LINECOMBOPT;
    const static QString MODE_GAUSSBLUR3;
    const static QString MODE_GAUSSBLUR5;
    const static QString MODE_BOXBLUR3;
    const static QString MODE_BOXBLUR5;

private:

    void _initializeDefaultState();

    std::vector<QString> m_typeModes;

    const static QString TYPE_MODE;

    static bool m_registered;

    ContourTypes( const QString& path, const QString& id );

    class Factory;

    ContourTypes( const ContourTypes& other);
    ContourTypes& operator=( const ContourTypes& other );
};

}
}
