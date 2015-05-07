/***
 * List of available animation types.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>

namespace Carta {

namespace Data {

class AnimationTypes : public Carta::State::CartaObject {

public:

    /**
     * Returns a list of supported animations
     * @return a list of supported animations.
     */
    QStringList getAnimations() const;

    const static QString CLASS_NAME;
    const static QString ANIMATION_LIST;
    virtual ~AnimationTypes();

private:


    void _initializeState();

    static bool m_registered;
    AnimationTypes( const QString& path, const QString& id );
    class Factory;


	AnimationTypes( const AnimationTypes& other);
	AnimationTypes operator=( const AnimationTypes& other );
};
}
}
