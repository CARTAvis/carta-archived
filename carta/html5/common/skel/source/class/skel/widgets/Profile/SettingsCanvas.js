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
            gridContainer.setLayout( new qx.ui.layout.Grid());
            
            var gridLabel = new qx.ui.basic.Label( "Grid Lines:");
            this.m_gridCheck = new qx.ui.form.CheckBox();
            this.m_gridCheck.setToolTipText( "Show/hide horizontal grid lines.");
            this.m_gridListenId = this.m_gridCheck.addListener( "changeValue", this._sendGridLinesCmd, this );
            
            var frameLabel = new qx.ui.basic.Label( "Frame:");
            this.m_frameCheck = new qx.ui.form.CheckBox();
            this.m_frameCheck.setToolTipText( "Show/hide vertical line showing current frame.");
            this.m_frameListenId = this.m_frameCheck.addListener( "changeValue", this._sendShowFrameCmd, this );
           
            var cursorLabel = new qx.ui.basic.Label( "Cursor:");
            this.m_cursorCheck = new qx.ui.form.CheckBox();
            this.m_cursorCheck.setToolTipText( "Show/hide cursor position mouse-over.");
            this.m_cursorListenId = this.m_cursorCheck.addListener( "changeValue", this._sendShowCursorCmd, this );
           
            gridContainer.add( gridLabel, {row:0,column:0} );
            gridContainer.add( this.m_gridCheck, {row:0,column:1} );
            gridContainer.add( frameLabel, {row:1,column:0});
            gridContainer.add( this.m_frameCheck, {row:1,column:1});
            gridContainer.add( cursorLabel, {row:2,column:0});
            gridContainer.add( this.m_cursorCheck, {row:2, column:1});
           
            overallContainer.add( gridContainer );
        },
        
        /**
         * Update of settings from the server.
         */
        prefUpdate : function( prefs ){
            this.setShowGridLines( prefs.gridLines );
            this.setShowCursor( prefs.showCursor );
            this.setShowFrame( prefs.showFrame );
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
         * Notify the server that whether or not to show/hide information
         * about the point underneath the mouse cursor.
         */
        _sendShowCursorCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var showCursor = this.m_cursorCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowCursor";
                var params = "showCursor:"+showCursor;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },

        /**
         * Notify the server that whether or not to show/hide the current
         * frame vertical line.
         */
        _sendShowFrameCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var showFrame = this.m_frameCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowFrame";
                var params = "showFrame:"+showFrame;
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
         * Update whether or not to show/hide information about the point underneath
         * the mouse cursor based on server-side values.
         */
        setShowCursor : function( show ){
            var oldShow = this.m_cursorCheck.getValue();
            if ( show != oldShow ){
                if ( this.m_cursorListenId !== null ){
                    this.m_cursorCheck.removeListenerById( this.m_cursorListenId );
                }
                this.m_cursorCheck.setValue( show );
                this.m_cursorListenId = this.m_cursorCheck.addListener( "changeValue", this._sendShowCursorCmd, this )
            }
        },
        
        /**
         * Update whether or not to show/hide the frame marker based on server-side values.
         */
        setShowFrame : function( show ){
            var oldShow = this.m_frameCheck.getValue();
            if ( show != oldShow ){
                if ( this.m_frameListenId !== null ){
                    this.m_frameCheck.removeListenerById( this.m_frameListenId );
                }
                this.m_frameCheck.setValue( show );
                this.m_frameListenId = this.m_frameCheck.addListener( "changeValue", this._sendShowFrameCmd, this )
            }
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
        m_cursorCheck : null,
        m_cursorListenId : null,
        m_frameCheck : null,
        m_frameListenId : null,
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
