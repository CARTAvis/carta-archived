/**
 * Displays settings for an elliptical region.
 */



qx.Class.define("skel.widgets.Image.Region.RegionSettingsEllipse", {
    extend : skel.widgets.Image.Region.RegionSettingsShared,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init( );
    },
    

    members : {
    	
    	/**
    	 * Clear error messages that may have appeared in the major and minor radius
    	 * text fields.
    	 */
    	_clearErrors : function(){
        	this.m_radiusMajorText.setError( false );
        	this.m_radiusMinorText.setError( false );
    		this.m_radiusMinorText.clearWarning();
			this.m_radiusMajorText.clearWarning();
        },
    	
    	/**
    	 * Return the type of region whose settings are being displayed.
    	 * @return {String} - the type of region whose settings are being displayed.
    	 */
    	getType : function(){
    		return "ellipse";
    	},
        
        /*
         * Initializes the UI.
         */
        _init : function( ) {         
            var radiusLabel = new qx.ui.basic.Label( "Radius");
            this.m_content.add( radiusLabel, {row:0,column:0,colSpan:2} );
            var radiusMajorLabel = new qx.ui.basic.Label( "Major:");
            this.m_radiusMajorText =  new skel.widgets.CustomUI.NumericTextField(0,null);
            skel.widgets.TestID.addTestId( this.m_radiusMajorText, "EllipseRegionMajorRadius");
            this.m_radiusMajorText.setToolTipText( "Set the length of the major radius of the ellipse.");
            this.m_radiusMajorListenId = this.m_radiusMajorText.addListener( "textChanged", this._sendRadiusMajorCmd, this );
            this.m_radiusMajorText.setIntegerOnly( false );
            this.m_content.add( radiusMajorLabel, {row:1,column:0} );
            this.m_content.add( this.m_radiusMajorText, {row:1,column:1});
            var radiusMinorLabel = new qx.ui.basic.Label( "Minor:");
            this.m_radiusMinorText = new skel.widgets.CustomUI.NumericTextField(0,null);
            skel.widgets.TestID.addTestId( this.m_radiusMinorText, "EllipseRegionMinorRadius");
            this.m_radiusMinorText.setToolTipText( "Set the length of the minor radius of the ellipse.");
            this.m_radiusMinorListenId = this.m_radiusMinorText.addListener( "textChanged", this._sendRadiusMinorCmd, this );
            this.m_radiusMinorText.setIntegerOnly( false );
            this.m_content.add( radiusMinorLabel, {row:2, column:0});
            this.m_content.add( this.m_radiusMinorText, {row:2, column:1});      
        },
        
        /**
         * Send a command to the server to set the major radius of the ellipse.
         */
        _sendRadiusMajorCmd : function(){
        	var value = this.m_radiusMajorText.getValue();
    		var minRadius = this.m_radiusMinorText.getValue();
    		if ( value >= minRadius ){
    			this._clearErrors();
    			if ( this.m_id !== null ){
    				var path = skel.widgets.Path.getInstance();
    				var cmd = this.m_id + path.SEP_COMMAND + "setRadiusMajor";
    				var params = "radiusMajor:"+value;
    				this.m_connector.sendCommand( cmd, params, function(){});
    			}
    		}
    		else {
    			this.m_radiusMajorText.setError( true );
    			this.m_radiusMajorText.postWarning( "Required: MajorRadius >= MinorRadius");
    		}
        },
        
        /**
         * Send a command to the server to set the minor radius of the ellipse.
         */
        _sendRadiusMinorCmd : function(){
        	var value = this.m_radiusMinorText.getValue();
        	var maxValue = this.m_radiusMajorText.getValue();
        	if ( value <= maxValue ){
        		this._clearErrors();
        		if ( this.m_id !== null ){
        			var path = skel.widgets.Path.getInstance();               
        			var cmd = this.m_id + path.SEP_COMMAND + "setRadiusMinor";
        			var params = "radiusMinor:"+value;
        			this.m_connector.sendCommand( cmd, params, function(){});
        		}
        	}
        	else {
        		this.m_radiusMinorText.setError( true );
        		this.m_radiusMinorText.postWarning( "Required: MajorRadius >= MinorRadius");
        	}
        },
        
        
        
        /**
         * Set the region whose settings will be displayed.
         * @param region {Object} - the region whose settings are to be displayed.
         */
        setRegion : function( region ){
        	
        	this.m_centerXText.removeListenerById( this.m_centerXListenId );
        	this.m_centerXText.setValue( region.centerx.toString() );
        	this.m_centerXListenId = this.m_centerXText.addListener( "textChanged", this._sendCenterCmd, this );
        	
        	this.m_centerYText.removeListenerById( this.m_centerYListenId );
        	this.m_centerYText.setValue( region.centery.toString() );
        	this.m_centerYListenId = this.m_centerYText.addListener( "textChanged", this._sendCenterCmd, this );
        	
        	this._clearErrors();
        	this.m_radiusMajorText.removeListenerById( this.m_radiusMajorListenId );
        	this.m_radiusMajorText.setValue( region.radiusMajor );
        	this.m_radiusMajorListenId = this.m_radiusMajorText.addListener( "textChanged", this._sendRadiusMajorCmd, this );
        	
        	this._clearErrors();
        	this.m_radiusMinorText.removeListenerById( this.m_radiusMinorListenId );
        	this.m_radiusMinorText.setValue( region.radiusMinor );
        	this.m_radiusMinorListenId = this.m_radiusMinorText.addListener( "textChanged", this._sendRadiusMinorCmd, this );
        },
        
        /**
         * Set the number of significant digits to use when doing a computation.
         * @param digits {Number} - the number of significant digits to use when doing a computation.
         */
        setSignificantDigits : function( digits ){
        	this.m_significantDigits = digits;
        }, 
        
        m_radiusMajorText : null,
        m_radiusMinorText : null,
        m_radiusMajorListenId : null,
        m_radiusMinorListenId : null,
        m_significantDigits : null
      
    }
});