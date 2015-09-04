/**
 * Displays controls for data such as which axes of an image should be displayed.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.Settings.DisplayAxes", { 
    extend : qx.ui.core.Widget,
    
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        
        /**
         * Update the UI with information from the server.
         * @param controls {Object} - information from the server.
         */
        setControls : function( controls ){
            this._updateAxes( controls.supportedAxes );
            this._setAxisX( controls.xAxis );
            this._setAxisY( controls.yAxis );
            this._setAxisZ( controls.zAxis );
            //this.fireDataEvent( "gridControlsChanged", data );
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            errorMan.clearErrors();
        },
        
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );

            this._setLayout( new qx.ui.layout.HBox(1));
            var content = new qx.ui.container.Composite();
            this._add( content );
           
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setSpacing( 3 );
            content.setLayout( gridLayout );
            
            content.add( new qx.ui.core.Spacer(), {row:0, column:0,colSpan:2})
            gridLayout.setRowFlex( 0, 1 );
            
            var chgVal = skel.widgets.Path.CHANGE_VALUE;
            var axisXLabel = new qx.ui.basic.Label( "X Axis:");
            this.m_axisXCombo = new skel.widgets.CustomUI.SelectBox( "setAxisX", "xAxis");
            this.m_axisXCombo.setToolTipText( "Set the horizontal display axis of the image(s).");
            content.add( axisXLabel, {row:1,column:0});
            content.add( this.m_axisXCombo, {row:1,column:1});
            var axisYLabel = new qx.ui.basic.Label( "Y Axis:");
            this.m_axisYCombo = new skel.widgets.CustomUI.SelectBox( "setAxisY", "yAxis");
            this.m_axisYCombo.setToolTipText( "Set the vertical display axis of the image(s).");
            content.add( axisYLabel, {row:2,column:0});
            content.add( this.m_axisYCombo, {row:2,column:1});
            var axisZLabel = new qx.ui.basic.Label( "Z Axis:");
            this.m_axisZCombo = new skel.widgets.CustomUI.SelectBox( "setAxisZ", "zAxis");
            this.m_axisZCombo.setToolTipText( "Set the hidden (animation) axis of the image(s).");
            content.add( axisZLabel, {row:3, column:0});
            content.add( this.m_axisZCombo, {row:3, column:1});
            
            gridLayout.setRowFlex( 4, 1 );
            content.add( new qx.ui.core.Spacer(), {row:4, column:0,colSpan:2});
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
         * Send a command to the server to get the grid controls id.
         * @param dataId {String} the server side id of the grid controls object.
         */
        setId : function( dataId ){
            if ( dataId !== null && dataId.length > 0 ){
                this.m_id = dataId;
                this.m_axisXCombo.setId( dataId );
                this.m_axisYCombo.setId( dataId );
                this.m_axisZCombo.setId( dataId );
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
        m_axisZCombo : null
    }
});