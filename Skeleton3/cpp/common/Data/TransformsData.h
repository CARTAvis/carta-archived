/***
 * List of possible data transforms that can be applied to the image.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>



namespace Carta {

namespace Data {

class TransformsData : public CartaObject {

public:

    /**
     * Returns true if the name represents a valid data transform; false, otherwise.
     * @param name a QString identifying a data transform.
     * @return true if the name represents a valid data tranform; false, otherwise.
     */
    bool isTransform( const QString& name ) const;

    /**
     * Returns a list of available data transforms.
     * @return a QStringList containing the names of available data transforms.
     */
    QStringList getTransformsData() const;

    virtual ~TransformsData();

    const static QString CLASS_NAME;
private:
    void _initializeDefaultState();
    void _initializeCallbacks();

    std::vector < QString > m_transforms;

    static bool m_registered;
    const static QString DATA_TRANSFORMS;
    const static QString TRANSFORM_COUNT;
    TransformsData( const QString& path, const QString& id );

    class Factory;


	TransformsData( const TransformsData& other);
	TransformsData operator=( const TransformsData & other );
};

}
}
