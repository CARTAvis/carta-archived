/***
 * List of all available Themes.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>

namespace Carta {

namespace Data {

class Themes : public Carta::State::CartaObject {

public:

    /**
     * Translates a non case sensitive theme into one
     * that is case sensitive.
     * @param theme - a theme that may not have the proper capitalization.
     * @return - a recognized theme or an empty string if the theme is not
     *      recognized.
     */
    QString getTheme( const QString& theme ) const;

    /**
     * Returns a list of available themes.
     * @return a QStringList containing the names of available themes.
     */
    QStringList getThemes() const;

    /**
     * Returns the index of the theme or -1 if no such theme exists.
     * @param name - an identifier for a theme.
     * @return the index of the identified theme or -1 if no such theme exists.
     */
    int getIndex( const QString& name) const;

    /**
     * Returns the default theme.
     * @return an identifier for the default theme.
     */
    QString getDefaultTheme() const;

    virtual ~Themes();


    const static QString CLASS_NAME;
    const static QString THEMES;
private:

    void _initializeDefaultState();
    void _initializeCallbacks();

    QStringList m_themes;


    static bool m_registered;

    Themes( const QString& path, const QString& id );

    class Factory;


	Themes( const Themes& other);
	Themes& operator=( const Themes& other );
};

}
}
