/**
 * Represents a region of an image.
 */

qx.Class.define("skel.widgets.Image.Region", {
    extend : qx.core.Object,

    /**
     * Constructor.
     */
    construct : function( id, type, corners) {
        this.m_id = id;
        this.m_type = type;
        this.m_vertices = corners;
    },
    
    members : {
        
        /**
         * Returns true if this region is the same as the other region;
         * false otherwise.
         * @param otherRegion {skel.widgets.Image.Region} - the region to compare
         *      this one to.
         * @return {boolean} - true if the region is the same as the other region; false otherwise.
         */
        equals : function( otherRegion ){
            var equalRegions = false;
            if ( otherRegion.getId()  == this.getId()){
                equalRegions = true;
            }
            return equalRegions;
        },
        
        /**
         * Returns the identifier for this region.
         * @return {String} - this region's identifier.
         */
        getId : function(){
            return this.m_id;
        },
        
        /**
         * Return a user-friendly label for this region.
         * @return {String} - a user-friendly label for this region.
         */
        getLabel : function(){
            var regionId = this.m_type + ": ";
            var count = this.m_vertices.length;
            for ( var j = 0; j < count; j++ ){
                var corner = "("+Math.round( this.m_vertices[j].x ) + ", " +
                        Math.round( this.m_vertices[j].y) + ")";
                regionId = regionId + corner;
                if ( j < count - 1){
                    regionId = regionId + ",";
                }
            }
            return regionId;
        },
        
        m_id : "",
        m_type : "",
        m_vertices : null
    }
});