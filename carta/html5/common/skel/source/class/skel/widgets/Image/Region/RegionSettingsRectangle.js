/**
 * Displays the settings for a rectangular region.
 */

qx.Class.define("skel.widgets.Image.Region.RegionSettingsRectangle", {
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
    	 * Return the type of region whose settings are being displayed.
    	 * @return {String} - the type of region whose settings are being displayed.
    	 */
    	getType : function(){
    		return "rectangle";
    	},
        
        /*
         * Initializes the UI.
         */
        _init : function( ) { 
            var boxLabel = new qx.ui.basic.Label( "Box");
            this.m_content.add( boxLabel, {row:0,column:0,colSpan:2} );
            var widthLabel = new qx.ui.basic.Label( "Width:");
            this.m_widthText =  new skel.widgets.CustomUI.NumericTextField(0,null);
            skel.widgets.TestID.addTestId( this.m_widthText, "RectangleRegionWidth");
            this.m_widthText.setToolTipText( "Set the width of the region.");
            this.m_widthText.setIntegerOnly( false );
            this.m_widthListenId = this.m_widthText.addListener( "textChanged", this._sendWidthCmd, this );
            this.m_content.add( widthLabel, {row:1,column:0} );
            this.m_content.add( this.m_widthText, {row:1,column:1});
            var heightLabel = new qx.ui.basic.Label( "Height:");
            this.m_heightText = new skel.widgets.CustomUI.NumericTextField(0,null);
            skel.widgets.TestID.addTestId( this.m_heightText, "RectangleRegionHeight");
            this.m_heightText.setToolTipText( "Set the height of the region.");
            this.m_heightText.setIntegerOnly( false );
            this.m_heightListenId = this.m_heightText.addListener( "textChanged", this._sendHeightCmd, this );
            this.m_content.add( heightLabel, {row:2, column:0});
            this.m_content.add( this.m_heightText, {row:2, column:1});
        },
        
        /**
         * Send a command to the server to set the height.
         */
        _sendHeightCmd : function(){
        	if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var value = this.m_heightText.getValue();
                var cmd = this.m_id + path.SEP_COMMAND + "setHeight";
                var params = "height:"+value;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send a command to the server to set the width.
         */
        _sendWidthCmd : function(){
        	if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var value = this.m_widthText.getValue();
                var cmd = this.m_id + path.SEP_COMMAND + "setWidth";
                var params = "width:"+value;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
      
        /**
         * Set the region whose settings will be displayed.
         * @param region {Object} - the region whose settings will be displayed.
         */
        setRegion : function( region ){
        	
        	arguments.callee.base.apply(this, arguments, region );
        	this.m_heightText.removeListenerById( this.m_heightListenId );
        	this.m_heightText.setValue( region.height );
        	this.m_heightListenId = this.m_heightText.addListener( "textChanged", this._sendHeightCmd, this );
        	
        	this.m_widthText.removeListenerById( this.m_widthListenId );
        	this.m_widthText.setValue( region.width );
        	this.m_widthListenId = this.m_widthText.addListener( "textChanged", this._sendWidthCmd, this );
        },
        
        /**
         * Set the number of significant digits to use when doing a computation.
         * @param digits {Number} - the number of significant digits to use when doing a computation.
         */
        setSignificantDigits : function( digits ){
        	this.m_significantDigits = digits;
        }, 
        
        m_heightText : null,
        m_heightListenId : null,
        m_significantDigits: null,
        m_widthText : null,
        m_widthListenId : null
    }
});