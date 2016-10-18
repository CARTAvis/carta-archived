/**
 * Displays common settings for a region.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Image.Region.RegionSettingsShared", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._initShared( );
    },
    

    members : {
    	
    	/**
    	 * Return the type of region whose settings are being displayed.
    	 * @return {String} - the type of region whose settings are being displayed.
    	 */
    	getType : function(){
    		return "";
    	},
        
        /*
         * Initializes the UI.
         */
        _initShared : function( ) {
            this._setLayout( new qx.ui.layout.VBox() );
            this.m_content = new qx.ui.groupbox.GroupBox( "");
            var grid=new qx.ui.layout.Grid();
            grid.setRowAlign( 0, "center", "middle")
            this.m_content.setLayout( grid );
            this._add( this.m_content );
            
            this.m_content.add( new qx.ui.core.Spacer(5), {row:1,column:2});
            this.m_content.add( new qx.ui.core.Spacer(5), {row:2,column:2});
            
            var centerLabel = new qx.ui.basic.Label( "Center");
            this.m_content.add( centerLabel, {row:0,column:3,colSpan:2});
            var xLabel = new qx.ui.basic.Label( "X:");
            //Note:  need to re-evaluate whether the actual lower bound should be 0.
            this.m_centerXText = new skel.widgets.CustomUI.NumericTextField( null, null );
            skel.widgets.TestID.addTestId( this.m_centerXText, "RegionCenterX");
            this.m_centerXText.setToolTipText( "Set the x-coordinate of the region center.");
            this.m_centerXText.setIntegerOnly( false );
            this.m_centerXListenId = this.m_centerXText.addListener( "textChanged", this._sendCenterCmd, this );
            this.m_content.add( xLabel, {row:1,column:3} );
            this.m_content.add( this.m_centerXText, {row:1,column:4});
            var yLabel = new qx.ui.basic.Label( "Y:");
            this.m_centerYText = new skel.widgets.CustomUI.NumericTextField( null, null );
            skel.widgets.TestID.addTestId( this.m_centerYText, "RegionCenterY");
            this.m_centerYText.setToolTipText( "Set the y-coordinate of the region center.");
            this.m_centerYText.setIntegerOnly( false );
            this.m_centerYListenId = this.m_centerYText.addListener( "textChanged", this._sendCenterCmd, this );
            this.m_content.add( yLabel, {row:2, column:3});
            this.m_content.add( this.m_centerYText, {row:2, column:4});
        },
        
        /**
         * Send a command to set the center to the server.
         */
        _sendCenterCmd : function(){
        	if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var valueX = this.m_centerXText.getValue();
                var valueY = this.m_centerYText.getValue();
                var cmd = this.m_id + path.SEP_COMMAND + "setCenter";
                var params = "x:"+valueX+",y:"+valueY;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
       
        /**
         * Set the server-side id for the object that manages regions.
         * @param id {String} - the server-side id of the region manager object.
         */
        setId : function( id ){
        	this.m_id = id;
        },
        
        /**
         * Set the region whose settings will be displayed.
         * @param region {Object} - the region whose settings will be displayed.
         */
        setRegion : function( region ){
        	this.m_centerXText.removeListenerById( this.m_centerXListenId );
        	this.m_centerXText.setValue( region.x.toString() );
        	this.m_centerXListenId = this.m_centerXText.addListener( "textChanged", 
        			this._sendCenterCmd, this );
        	
        	this.m_centerYText.removeListenerById( this.m_centerYListenId );
        	this.m_centerYText.setValue( region.y.toString() );
        	this.m_centerYListenId = this.m_centerYText.addListener( "textChanged", 
        			this._sendCenterCmd, this );
        },
        
        m_centerXText : null,
        m_centerYText : null,
        m_centerXListenId : null,
        m_centerYListenId : null,
        m_connector : null,
        m_content : null,
        m_id : null
    }
});