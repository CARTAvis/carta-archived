/***
 * List of available animation types.
 *
 */

#pragma once

#include <QStringList>

namespace Carta {

namespace Data {

class AnimationTypes {

public:

    /**
     * Returns a list of supported animations
     * @return a list of supported animations.
     */
    static QStringList getAnimations();

    virtual ~AnimationTypes();

private:

    static void _init();
    AnimationTypes();

    static QList<QString> m_animations;
	AnimationTypes( const AnimationTypes& other);
	AnimationTypes& operator=( const AnimationTypes& other );
};
}
}
