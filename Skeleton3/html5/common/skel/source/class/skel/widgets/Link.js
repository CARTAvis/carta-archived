/**
 * Represents a link from a source to a destination.
 */

qx.Class.define("skel.widgets.Link", {
    extend : qx.core.Object,

    construct : function( sourceId, destinationId) {
        this.source = sourceId;
        this.destination = destinationId;
    },
    members : {
        source : "",
        destination : ""
    }
});