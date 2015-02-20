/**
 * Represents a link from a source to a destination.
 */

qx.Class.define("skel.widgets.Link.Link", {
    extend : qx.core.Object,

    construct : function( sourceId, destinationId) {
        this.source = sourceId;
        this.destination = destinationId;
    },
    members : {
        
        /**
         * Returns true if the other link is equal to this one; false otherwise.
         * @param otherLink {skel.widgets.Link} the link to compare this one to.
         * @return {boolean} true if the links have the same source and destination; false otherwise.
         */
        equals : function( otherLink ){
            var equalLinks = false;
            if ( otherLink.hasOwnProperty( "source") && otherLink.source == this.source){
                if ( otherLink.hasOwnProperty("destination") && otherLink.destination== this.destination ){
                    equalLinks = true;
                }
            }
            return equalLinks;
        },
        source : "",
        destination : ""
    }
});