/***
 * List of possible data transforms that can be applied to the image.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include <vector>



namespace Carta {

namespace Data {

class TransformsData : public Carta::State::CartaObject {

public:

    /**
     * Returns true if the name represents a valid data transform; false, otherwise.
     * @param name a QString identifying a data transform.
     * @param actualName the actual name of the transform in case of a case mismatch
     * @return true if the name represents a valid data tranform; false, otherwise.
     */
    bool isTransform( const QString& name, QString& actualName ) const;

    /**
     * Returns a list of available data transforms.
     * @return a QStringList containing the names of available data transforms.
     */
    QStringList getTransformsData() const;

    Carta::Lib::PixelPipeline::ScaleType getScaleType( const QString& name ) const;

    virtual ~TransformsData();

    const static QString CLASS_NAME;
private:
    void _initializeDefaultState();
    void _initializeCallbacks();

    std::vector < QString > m_transforms;

    static bool m_registered;
    const static QString DATA_TRANSFORMS;
    const static QString TRANSFORM_COUNT;
    const static QString TRANSFORM_NONE;
    const static QString TRANSFORM_ROOT;
    const static QString TRANSFORM_POLY;
    const static QString TRANSFORM_SQUARE;
    const static QString TRANSFORM_LOG;
    TransformsData( const QString& path, const QString& id );

    class Factory;


	TransformsData( const TransformsData& other);
	TransformsData operator=( const TransformsData & other );
};

}
}
