/***
 * List of possible layer region types that can be applied to an image.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class RegionTypes : public Carta::State::CartaObject {

public:

    /**
     * Return the default region type.
     * @return - the default region type.
     */
    QString getDefault() const;

    /**
     * Returns the official region type corresponding to the passed in name (which may be
     * case insensitive).
     * @param name - a QString identifying a region type.
     * @return - the official name of the region type or an empty string if there is no
     * 		such type.
     */
    QString getActualType( const QString& name ) const;

    /**
     * Returns the model type corresponding to the passed in user type.
     * @param userType - a region type used by the UI.
     * @return - the region type used by the model.
     */
    QString getModelType( const QString& userType ) const;


    /**
     * Returns a list of available layer composition modes.
     * @return a QStringList containing the names of available layer composition modes.
     */
    QStringList getRegionTypes() const;

    virtual ~RegionTypes();

    const static QString CLASS_NAME;
    const static QString ELLIPSE;
    const static QString POINT;
    const static QString POLYGON;
    const static QString RECTANGLE;

private:
    void _initializeDefaultState();
    void _insertType( const QString& regionType, int index );

    static bool m_registered;
    static const QString TYPES;

    RegionTypes( const QString& path, const QString& id );

    class Factory;


	RegionTypes( const RegionTypes& other);
	RegionTypes& operator=( const RegionTypes & other );
};

}
}
