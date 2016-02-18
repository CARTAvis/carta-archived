/**
 * Settings for statistics of a particular type (image or region).
 */


/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Statistics.SettingsPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     * @param label {String} - an identifier for the type of statistic.
     * @param cmd {String} - the command for changing the visibility of the statistics.
     */
    construct : function( label, cmd ) {
        this.base(arguments, label, "");
        this.m_connector = mImport("connector");
        this._init( label );
        this.m_cmd = cmd;
    },

    members : {
        
        /**
         * Initializes the UI.
         * @param label {String} - label for the type of statistics to display.
         */
        _init : function( label ) {
            this._setLayout(new qx.ui.layout.HBox(2));
            
            var content = new qx.ui.container.Composite();
            content.setLayout( new qx.ui.layout.VBox(2) );
            this._add( content, {flex:1} );
            
            var controlContainer = new qx.ui.container.Composite();
            controlContainer.setLayout( new qx.ui.layout.HBox(2) );
            var showLabel = new qx.ui.basic.Label( "Show:");
            this.m_showCheck = new qx.ui.form.CheckBox();
            this.m_showCheck.setToolTipText( "Show/hide "+label+" statistics.");
            skel.widgets.TestID.addTestId( this.m_showCheck, label+"ShowStats");
            this.m_showId = this.m_showCheck.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._statVisibilityChanged, this );
            controlContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            controlContainer.add( showLabel);
            controlContainer.add( this.m_showCheck);
            controlContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            content.add( controlContainer );
            
            this.m_checkContainer = new skel.widgets.Statistics.DragDropGrid( label );
            this.m_checkContainer.addListener( "statMoved", this._sendMoveCmd, this );
            content.add( this.m_checkContainer );
        },
        
        /**
         * Send a command to the server to move an individual statistic.
         * @param ev {Object} - information about the current location of the statistic
         *      and where it should be moved.
         */
        _sendMoveCmd : function( ev ){
            if ( this.m_id !== null ){
                var data = ev.getData();
                var type = data.type;
                var origIndex = data.originalIndex;
                var destIndex = data.moveIndex;
               
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "moveStat";
                var params = "type:"+type + ", from:"+origIndex+", to:"+destIndex;
                this.m_connector.sendCommand( cmd, params, null);
            }
            else {
                console.log( "Could not send move id was null");
            }
        },
        
        
        /**
         * Send a command to the server to show/hide image statistics.
         */
        _sendShowStatsCmd : function(){
            if ( this.m_id !== null ){
                var showStats = this.m_showCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + this.m_cmd;
                var params = "visible:"+showStats;
                this.m_connector.sendCommand( cmd, params, null);
            }
        },
        
        
        /**
         * Set the server-side id for the statistics settings.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_checkContainer.setId( id );
        },
        
        
        /**
         * Update the UI with new visibility and statistics labels.
         * @param showStats {boolean} - true if the statistics should be visible; false otherwise.
         * @param labels {Array} - list of statistics to show.
         */
        setPrefs : function( showStats, labels ){
            if ( this.m_showId !== null ){
                
                this.m_showCheck.removeListenerById( this.m_showId );
                this.m_showCheck.setValue( showStats );
                   
                this.m_checkContainer.setGridEnabled( showStats );
                this.m_showId = this.m_showCheck.addListener( skel.widgets.Path.CHANGE_VALUE, 
                            this._statVisibilityChanged, this );
            }
            this.m_checkContainer.setLabels ( labels );
        },
        
        
        /**
         * Update the visibility of the image/region statistics.
         */
        _statVisibilityChanged : function(){
            var showStats = this.m_showCheck.getValue();
            this.m_checkContainer.setGridEnabled( showStats );
            this._sendShowStatsCmd();
        },
        
        
        m_checkContainer : null,
        m_connector : null,
        m_id : null,
        m_cmd : null,
        m_colCount : 2,
        m_showCheck : null,
        m_showId : null,
        m_checkList : null

    }
});