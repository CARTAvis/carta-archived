/**
 * Represents a link from a source to a destination.
 */

qx.Class.define("skel.widgets.Link.Link", {
    extend : qx.core.Object,

    construct : function( sourceId, destinationId) {
        this.m_source = sourceId;
        this.m_destination = destinationId;
    },
    members : {
        
        /**
         * Returns true if the other link is equal to this one; false otherwise.
         * @param otherLink {skel.widgets.Link} the link to compare this one to.
         * @return {boolean} true if the links have the same source and destination; false otherwise.
         */
        equals : function( otherLink ){
            var equalLinks = false;
            if ( otherLink.hasOwnProperty( "source") && otherLink.source == this.m_source){
                if ( otherLink.hasOwnProperty("destination") && otherLink.destination== this.m_destination ){
                    equalLinks = true;
                }
            }
            return equalLinks;
        },
        
        /**
         * Returns the link destination.
         * @return {String} an identifier for the link destination.
         */
        getDestination : function(){
            return this.m_destination;
        },
        
        /**
         * Returns the link source.
         * @return {String} an identifier for the link source.
         */
        getSource : function(){
            return this.m_source;
        },
        
        m_source : "",
        m_destination : ""
    }
});