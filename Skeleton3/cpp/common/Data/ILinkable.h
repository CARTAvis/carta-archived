/*
 * Interface implemented by classes that can be linked to an image view.
 */

#ifndef ILINKABLE_H_
#define ILINKABLE_H_

#include "State/ObjectManager.h"

namespace Carta {

namespace Data {

class ILinkable {
public:

    /**
     * Remove the link to the CartaObject.
     * @param cartaObj the CartaObject that should no longer be linked.
     * @return true if the link was successfully removed; false otherwise.
     */
    virtual bool removeLink( CartaObject * cartaObj ) = 0;

    /**
     * Add a link to the CartaObject.
     * @param artaObj the CartaObject that should be linked.
     * @return true if the CartaObject is linked; false otherwise.
     */
    virtual bool addLink( CartaObject* cartaObj ) = 0;

};
}
}


#endif /* ILINKABLE_H_ */
