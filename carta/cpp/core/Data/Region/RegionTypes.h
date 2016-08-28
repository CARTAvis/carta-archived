/***
 * List of possible layer region types that can be applied to an image.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>



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
     * Returns true if the name represents a valid region type; false, otherwise.
     * @param name - a QString identifying a region type.
     * @param actualName - the actual name of the transform in case of a case mismatch
     * @return true if the name represents a valid region type; false, otherwise.
     */
    bool isRegionType( const QString& name, QString& actualName ) const;

    /**
     * Returns a list of available layer composition modes.
     * @return a QStringList containing the names of available layer composition modes.
     */
    QStringList getRegionTypes() const;

    virtual ~RegionTypes();

    const static QString CLASS_NAME;
    const static QString ELLIPSE;
    const static QString POLYGON;

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
