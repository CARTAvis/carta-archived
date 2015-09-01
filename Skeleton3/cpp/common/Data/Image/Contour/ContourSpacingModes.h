/***
 * List of all available methods for specifying the spacing between contour levels.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>

namespace Carta {

namespace Data {

class ContourSpacingModes : public Carta::State::CartaObject {

public:
    /**
     * Translates a non case sensitive specification for the spacing between contour levels into one
     * that is case sensitive.
     * @param method - a specification for the spacing between contour levels
     *  that may not have the proper capitalization.
     * @return - a recognized specification for contour spacing.
     */
    QString getSpacingMethod( const QString& method );

    /**
     * Returns the default specification for spacing between contour levels.
     * @return - the default spacing specification for contour levels.
     */
    QString getModeDefault() const;

    virtual ~ContourSpacingModes();


    const static QString CLASS_NAME;
    const static QString MODE_LINEAR;
    const static QString MODE_LOGARITHM;
    const static QString MODE_PERCENTILE;

private:

    void _initializeDefaultState();

    std::vector<QString> m_spacingModes;

    const static QString SPACING_MODE;
    static bool m_registered;

    ContourSpacingModes( const QString& path, const QString& id );

    class Factory;


	ContourSpacingModes( const ContourSpacingModes& other);
	ContourSpacingModes& operator=( const ContourSpacingModes& other );
};

}
}
