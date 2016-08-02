/***
 * List of possible line styles such as solid or dotted.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class LineStyles : public Carta::State::CartaObject {

public:

    /**
     * Return the official name of the line style based on a name that may be a case
     * insensitive match.
     * @param styleStr - a case insensitive name for a line style.
     * @return - the official name for the line style or an empty string if the passed
     *      in line style was not recognized.
     */
    QString getActualLineStyle( const QString& styleStr ) const;

    /**
     * Return the default line style.
     * @return - the default line style.
     */
    QString getDefault() const;

    /**
     * Return the default line style for a secondary curve (such as a fit).
     * @return - the default line style for a secondary curve.
     */
    QString getDefaultSecondary() const;

    /**
     * Returns a list of available line styles.
     * @return - a QStringList containing the names of available line styles.
     */
    QStringList getLineStyles() const;


    virtual ~LineStyles();

    const static QString CLASS_NAME;
    static const QString LINE_STYLE_SOLID;
    static const QString LINE_STYLE_DASH;
    static const QString LINE_STYLE_DOT;
    static const QString LINE_STYLE_DASHDOT;
    static const QString LINE_STYLE_DASHDOTDOT;

private:

    static const QString LINE_STYLES;

    void _initValue( int * index, const QString& name );
    void _initializeDefaultState();

    static bool m_registered;

    LineStyles( const QString& path, const QString& id );

    class Factory;


	LineStyles( const LineStyles& other);
	LineStyles& operator=( const LineStyles & other );
};

}
}
