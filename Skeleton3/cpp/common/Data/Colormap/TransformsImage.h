/***
 * List of possible image transforms.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>



namespace Carta {

namespace Data {

class TransformsImage : public Carta::State::CartaObject {

public:

    /**
     * Returns true if the name represents a valid image transform; false, otherwise.
     * @param name a QString identifying an image transform.
     * @return true if the name represents a valid image transform; false, otherwise.
     */
    bool isTransform( const QString& name ) const;

    /**
     * Returns a list of available image transforms.
     * @return a QStringList containing the names of available image transforms.
     */
    QStringList getTransformsImage() const;

    virtual ~TransformsImage();

    const static QString CLASS_NAME;
private:
    void _initializeDefaultState();
    void _initializeCallbacks();

    std::vector < QString > m_transforms;

    static bool m_registered;
    const static QString IMAGE_TRANSFORMS;
    const static QString TRANSFORM_COUNT;
    TransformsImage( const QString& path, const QString& id );

    class Factory;


	TransformsImage( const TransformsImage& other);
	TransformsImage& operator=( const TransformsImage& other );
};

}
}
