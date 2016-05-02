/**
 * Allows the user to set properties of the plot canvas.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsCanvas", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init();
        
        //Initiate connector.
         if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
         }
    },

    members : {

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(1);
            this._setLayout(widgetLayout);
            
            var overallContainer = new qx.ui.groupbox.GroupBox( "Plot", "");
            overallContainer.setLayout( new qx.ui.layout.VBox(1));
            overallContainer.setContentPadding(1,1,1,1);
            this._add( overallContainer );
            
            var gridContainer = new qx.ui.container.Composite();
            gridContainer.setLayout( new qx.ui.layout.HBox(1));
            var gridLabel = new qx.ui.basic.Label( "Grid Lines");
            this.m_gridCheck = new qx.ui.form.CheckBox();
            this.m_gridCheck.setToolTipText( "Show/hide horizontal grid lines.");
            this.m_gridListenId = this.m_gridCheck.addListener( "changeValue", this._sendGridLinesCmd, this );
           
            gridContainer.add( this.m_gridCheck );
            gridContainer.add( gridLabel );
            overallContainer.add( gridContainer );
        },
        
        /**
         * Update of settings from the server.
         */
        prefUpdate : function( prefs ){
            this.setShowGridLines( prefs.gridLines );
        },
        
        /**
         * Notify the server that whether or not to show/hide grid lines
         * on the plot has changed.
         */
        _sendGridLinesCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var gridLines = this.m_gridCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setGridLines";
                var params = "gridLines:"+gridLines;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },

        
        /**
         * Set the server side id of this plot.
         * @param id {String} the server side id of the object that produced this plot.
         */
        setId : function( id ){
            this.m_id = id;
           
        },
        
        /**
         * Update whether or not to show/hide grid lines based on server-side values.
         */
        setShowGridLines : function( show ){
            var oldShow = this.m_gridCheck.getValue();
            if ( show != oldShow ){
                if ( this.m_gridListenId !== null ){
                    this.m_gridCheck.removeListenerById( this.m_gridListenId );
                }
                this.m_gridCheck.setValue( show );
                this.m_gridListenId = this.m_gridCheck.addListener( "changeValue", this._sendGridLinesCmd, this )
            }
        },
        

       
        m_id : null,
        m_connector : null,
        m_gridCheck : null,
        m_gridListenId : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});
