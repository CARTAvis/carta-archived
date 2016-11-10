/**
 * Displays the settings for a polygonal region.
 */

qx.Class.define("skel.widgets.Image.Region.RegionSettingsPolygon", {
    extend : skel.widgets.Image.Region.RegionSettingsRectangle,

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
    		return "polygon";
    	},
    	
        /**
         * Set the region whose settings will be displayed.
         * @param region {Object} - the region whose settings will be displayed.
         */
        setRegion : function( region ){
        	var minX = Number.MAX_VALUE;
        	var minY = Number.MAX_VALUE;
        	var maxX = -1;
        	var maxY = -1;
        	for( var i = 0; i < region.pts.length; i++ ){
        		if ( region.pts[i].x < minX ){
        			minX = region.pts[i].x;
        		}
        		if ( region.pts[i].x > maxX ){
        			maxX = region.pts[i].x;
        		}
        		if ( region.pts[i].y < minY ){
        			minY = region.pts[i].y;
        		}
        		if ( region.pts[i].y > maxY ){
        			maxY = region.pts[i].y;
        		}
        	}
        	
        	var width = maxX - minX;
        	var height = maxY - minY;
        	var centerX = ( minX + maxX ) / 2;
        	var centerY = ( minY + maxY ) / 2;  	
        	if ( this.m_significantDigits !== null ){
        		width = Math.round(width * Math.pow(10, this.m_significantDigits)) / Math.pow(10, this.m_significantDigits);
        		height = Math.round(height * Math.pow(10, this.m_significantDigits)) / Math.pow(10, this.m_significantDigits);
        		centerX = Math.round(centerX * Math.pow(10, this.m_significantDigits)) / Math.pow(10, this.m_significantDigits);
        		centerY = Math.round(centerY * Math.pow(10, this.m_significantDigits)) / Math.pow(10, this.m_significantDigits);
        	}
        	
        	this.m_centerXText.removeListenerById( this.m_centerXListenId );
        	this.m_centerXText.setValue( centerX.toString() );
        	this.m_centerXListenId = this.m_centerXText.addListener( "textChanged", 
        			this._sendCenterCmd, this );
        	
        	this.m_centerYText.removeListenerById( this.m_centerYListenId );
        	this.m_centerYText.setValue( centerY.toString() );
        	this.m_centerYListenId = this.m_centerYText.addListener( "textChanged", 
        			this._sendCenterCmd, this );
        	
        	this.m_heightText.removeListenerById( this.m_heightListenId );
        	this.m_heightText.setValue( height );
        	this.m_heightListenId = this.m_heightText.addListener( "textChanged", 
        			this._sendHeightCmd, this );
        	
        	this.m_widthText.removeListenerById( this.m_widthListenId );
        	this.m_widthText.setValue( width );
        	this.m_widthListenId = this.m_widthText.addListener( "textChanged", 
        			this._sendWidthCmd, this );
        }
        
        
    }
    
});