/***
 * List of all available styles for drawing contour lines.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>

namespace Carta {

namespace Data {

class ContourStyles : public Carta::State::CartaObject {

public:

    /**
     * Translates a non case sensitive contour line style into one
     * that is case sensitive.
     * @param lineStyle - contour line style  that may not have the proper capitalization.
     * @return - a recognized contour line style.
     */
    QString getLineStyle (const QString& lineStyle );

    /**
     * Returns the default contour line style.
     * @return - the default contour line style.
     */
    QString getLineStyleDefault() const;

    virtual ~ContourStyles();


    const static QString CLASS_NAME;


private:

    void _initializeDefaultState();

    std::vector<QString> m_lineStyles;

    const static QString LINE_SOLID;
    const static QString LINE_DASHED;
    const static QString LINE_DOTTED;
    const static QString LINE_DASHED_DOTTED;
    const static QString LINE_STYLES;
    static bool m_registered;

    ContourStyles( const QString& path, const QString& id );

    class Factory;


	ContourStyles( const ContourStyles& other);
	ContourStyles& operator=( const ContourStyles& other );
};

}
}
