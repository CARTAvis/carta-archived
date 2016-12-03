/*
 * Interface implemented by classes that can be linked to an image view.
 */

#pragma once

#include "core/State/ObjectManager.h"

namespace Carta {

namespace Data {

class ILinkable {
public:

    /**
     * Remove the link to the CartaObject.
     * @param cartaObj the CartaObject that should no longer be linked.
     * @return an error message if there was a problem removing the link;
     *      an empty string otherwise..
     */
    virtual QString removeLink( Carta::State::CartaObject * cartaObj ) = 0;

    /**
     * Add a link to the CartaObject.
     * @param artaObj the CartaObject that should be linked.
     * @return an error message if there was a problem adding the link;
     *      an empty string otherwise.
     */
    virtual QString addLink( Carta::State::CartaObject* cartaObj ) = 0;

    /**
     * Return a list of identifiers for all objects that are controlled by
     * this CartaObject.
     * @return a list of identifiers for objects under the control of this
     * CartaObject.
     */
    virtual QList<QString> getLinks() const = 0;

    /**
     * Returns whether or not the object with the given id is already linked to this object.
     * @param linkId - a QString identifier for an object.
     * @return true if this object is already linked to the one identified by the id; false otherwise.
     */
    virtual bool isLinked( const QString& linkId ) const = 0;

};
}
}
