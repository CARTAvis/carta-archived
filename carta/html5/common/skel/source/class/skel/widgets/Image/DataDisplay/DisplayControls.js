/**
 * Displays controls for data such as which axes of an image should be displayed.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.DataDisplay.DisplayControls", { 
    extend : qx.ui.tabview.Page,
    
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments, "Data", "");
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        
        /**
         * Callback for a change in the data display controls.
         */
        _controlsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    this._updateAxes( controls.axes );
                    this._setAxisX( controls.xAxis );
                    this._setAxisY( controls.yAxis );
                    this._setAxisZ( controls.zAxis );
                    //this.fireDataEvent( "gridControlsChanged", data );
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();
                }
                catch( err ){
                    console.log( "Display controls could not parse: "+val);
                    console.log( "Error: "+err);
                }
            }
        },
        
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );

            this._setLayout( new qx.ui.layout.HBox(1));

            var content = new qx.ui.container.Composite();
            this._add( new qx.ui.core.Spacer(), {flex:1} );
            this._add( content );
            this._add( new qx.ui.core.Spacer(), {flex:1} );
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setSpacing( 3 );
            content.setLayout( gridLayout );
            
            content.add( new qx.ui.core.Spacer(), {row:0, column:0,colSpan:2})
            gridLayout.setRowFlex( 0, 1 );
            
            this.m_allCheck = new qx.ui.form.CheckBox( "All Images");
            gridLayout.setRowAlign( 1, "center", "middle" );
            content.add( this.m_allCheck, {row:1,column:0,colSpan:2});
            
            var chgVal = skel.widgets.Path.CHANGE_VALUE;
            var axisXLabel = new qx.ui.basic.Label( "X Axis:");
            this.m_axisXCombo = new skel.widgets.CustomUI.SelectBox( "setAxisX", "xAxis");
            content.add( axisXLabel, {row:2,column:0});
            content.add( this.m_axisXCombo, {row:2,column:1});
            var axisYLabel = new qx.ui.basic.Label( "Y Axis:");
            this.m_axisYCombo = new skel.widgets.CustomUI.SelectBox( "setAxisY", "yAxis");
            content.add( axisYLabel, {row:3,column:0});
            content.add( this.m_axisYCombo, {row:3,column:1});
            var axisZLabel = new qx.ui.basic.Label( "Z Axis:");
            this.m_axisZCombo = new skel.widgets.CustomUI.SelectBox( "setAxisZ", "zAxis");
            content.add( axisZLabel, {row:4, column:0});
            content.add( this.m_axisZCombo, {row:4, column:1});
            
            gridLayout.setRowFlex( 5, 1 );
            content.add( new qx.ui.core.Spacer(), {row:5, column:0,colSpan:2});
        },
        
        /**
         * Register to get updates on data display settings from the server.
         */
        _registerControls : function(){
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._controlsChangedCB.bind(this));
            this._controlsChangedCB();
        },
        
        /**
         * Callback for when the registration is complete and an id is available.
         * @param anObject {skel.widgets.Grid.GridControls}.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                anObject._setDataId( id );
            };
        },
        
        /**
         * Set the image axis that should be the x-axis of the display.
         * @param val {String} - an identifier for x-axis that should be displayed.
         */
        _setAxisX : function( val ){
            this.m_axisXCombo.setSelectValue( val, false );
        },
        
        /**
         * Set the image axis that should be the y-axis of the display.
         * @param val {String} - an identifier for the y-axis that should be displayed.
         */
        _setAxisY : function( val ){
            this.m_axisYCombo.setSelectValue( val, false );
        },
        
        /**
         * Set the image axis that should be the z-axis of the display.
         * @param val {String} - an identifier for the z-axis that should be displayed.
         */
        _setAxisZ : function( val ){
            this.m_axisZCombo.setSelectValue( val, false );
        },
        
        /**
         * Send a command to the server to get the display controls id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
            var path = skel.widgets.Path.getInstance();
            var cmd = imageId + path.SEP_COMMAND + "registerDisplayControls";
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._registrationCallback( this));
        },
        
        /**
         * Store the display controls id.
         * @param dataId {String} - a server-side identifier for the data display controls.
         */
        _setDataId : function( dataId ){
            if ( dataId !== null && dataId.length > 0 ){
                this.m_id = dataId;
                this.m_axisXCombo.setId( dataId );
                this.m_axisYCombo.setId( dataId );
                this.m_axisZCombo.setId( dataId );
                this._registerControls();
            }
        },
        
        /**
         * Update the list of available display axes based on the image that is
         * loaded on the server.
         * @param axis {Array} - a list of available display axes.
         */
        _updateAxes : function( axes ){
            this.m_axisXCombo.setSelectItems( axes );
            this.m_axisYCombo.setSelectItems( axes );
            this.m_axisZCombo.setSelectItems( axes );
        },
        
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        
        m_axisXCombo : null,
        m_axisYCombo : null,
        m_axisZCombo : null,
        m_allCheck : null
    }
});