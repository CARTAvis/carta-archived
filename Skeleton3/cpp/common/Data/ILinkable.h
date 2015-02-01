/*
 * Interface implemented by classes that can be linked to an image view.
 */

#ifndef ILINKABLE_H_
#define ILINKABLE_H_

#include "Data/Controller.h"

namespace Carta {

namespace Data {

class ILinkable {
public:

    /**
     * Remove the link to the image view.
     * @param controller the image view that should no longer be linked.
     * @return true if the link was successfully removed; false otherwise.
     */
    virtual bool removeLink( const std::shared_ptr<Controller>& controller ) = 0;

    /**
     * Add a link to the image view.
     * @param controller the image view that should be linked.
     * @return true if the controller is linked; false otherwise.
     */
    virtual bool addLink( const std::shared_ptr<Controller>& controller ) = 0;

};
}
}


#endif /* ILINKABLE_H_ */
