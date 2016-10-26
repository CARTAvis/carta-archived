/**
 * Displays the settings for a point region.
 */

qx.Class.define("skel.widgets.Image.Region.RegionSettingsPoint", {
    extend : skel.widgets.Image.Region.RegionSettingsShared,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
    },
    

    members : {
    	
    	/**
    	 * Return the type of region whose settings are being displayed.
    	 * @return {String} - the type of region whose settings are being displayed.
    	 */
    	getType : function(){
    		return "Point";
    	},
        
      
        /**
         * Set the region whose settings will be displayed.
         * @param region {Object} - the region whose settings will be displayed.
         */
        setRegion : function( region ){	
        	arguments.callee.base.apply(this, arguments, region );
        }
    }
});