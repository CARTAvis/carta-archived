/**
 * Displays controls for customizing profile range settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsLegend", {
    //extend : qx.ui.tabview.Page,
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init( );
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 0, 0, 0, 0 );
            this._setLayout(new qx.ui.layout.HBox(1));
            
            var content = new qx.ui.groupbox.GroupBox( "Legend");
            content.setLayout( new qx.ui.layout.VBox(0));
            content.setContentPadding(1,1,1,1);
            var showContainer = new qx.ui.container.Composite();
            showContainer.setLayout( new qx.ui.layout.HBox(2));
            var showLabel = new qx.ui.basic.Label( "Show:");
            this.m_legendShow = new qx.ui.form.CheckBox();
            this.m_legendShow.addListener( "changeValue", this._sendLegendVisibilityCmd, this );
            this.m_legendShow.setToolTipText( "Hide/show the plot legend.");
            showContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            showContainer.add( showLabel );
            showContainer.add( this.m_legendShow );
            showContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            content.add( showContainer );
            
            var posContainer = new qx.ui.container.Composite();
            posContainer.setLayout( new qx.ui.layout.HBox(2));
            var posLabel = new qx.ui.basic.Label( "External:");
            this.m_legendExternal = new qx.ui.form.CheckBox();
            this.m_legendExternal.setToolTipText( "Position the legend external/internal to the plot.");
            this.m_legendExternal.addListener( "changeValue", this._sendLegendExternalCmd, this );
            posContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            posContainer.add( posLabel );
            posContainer.add( this.m_legendExternal );
            posContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            content.add( posContainer );
            
            var lineContainer = new qx.ui.container.Composite();
            lineContainer.setLayout( new qx.ui.layout.HBox(2));
            var lineLabel = new qx.ui.basic.Label( "Show Line:");
            this.m_legendLine = new qx.ui.form.CheckBox();
            this.m_legendLine.setToolTipText( "Include a line symbol in the legend.");
            this.m_legendLine.addListener( "changeValue", this._sendLegendLineCmd, this );
            lineContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            lineContainer.add( lineLabel );
            lineContainer.add( this.m_legendLine );
            lineContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            content.add( lineContainer );
            
            var locateContainer = new qx.ui.container.Composite();
            locateContainer.setLayout( new qx.ui.layout.VBox());
            this.m_legendLocate = new skel.widgets.CustomUI.SelectBox("setLegendLocation","legendLocation");
            this.m_legendLocate.setToolTipText( "Choose the location of the legend on the plot.");
            locateContainer.add( this.m_legendLocate );
            content.add( locateContainer );
            
            this._add( content );
        },
        
        /**
         * Callback for a change in the available legend locations.
         */
        _legendLocationChangedCB : function(){
            if ( this.m_sharedVar ){
                var val = this.m_sharedVar.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        var locations = obj.locations;
                        this.m_legendLocate.setSelectItems( locations );
                    }
                    catch( err ){
                        console.log( "Could not parse legend locations: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        /**
         * Sets up the shared variable for receiving changes in the available legend
         * locations.
         * @param anObject {skel.widgets.Profile.SettingsLegend}.
         */
        _legendPositionsCallback : function( anObject ){
            return function( id ){
                if ( id && id.length > 0 ){
                    anObject.m_legendId = id;
                    anObject.m_sharedVar = anObject.m_connector.getSharedVar( id );
                    anObject.m_sharedVar.addCB( anObject._legendLocationChangedCB.bind( anObject ));
                    anObject._legendLocationChangedCB();
                }
            };
        },
        
        /**
         * Update of legend preferences from the server.
         * @param profilePrefs {Object} - profile preferences from the server.
         */
        prefUpdate : function( profilePrefs ){
            this.m_legendLocate.setSelectValue( profilePrefs.legendLocation );
            this.m_legendExternal.setValue( profilePrefs.legendExternal );
            this.m_legendShow.setValue( profilePrefs.legendShow );
            this.m_legendLine.setValue( profilePrefs.legendLine );
        },
        
        /**
         * Register to get the id of the object on the server side responsible for updates
         * of the available legend locations.
         */
        _registerLegendPositions : function(){
            var paramMap = "";
            var path = skel.widgets.Path.getInstance();
            var regCmd = this.m_id + path.SEP_COMMAND + "registerLegendLocations";
            this.m_connector.sendCommand( regCmd, paramMap, this._legendPositionsCallback(this));
        },
        
        /**
         * Notify the server of changes to the legend's visibility.
         */
        _sendLegendVisibilityCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var legendVisible = this.m_legendShow.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setLegendShow";
                var params = "legendShow:"+legendVisible;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Notify the server about changes to whether or not the legend is external
         * or internal to the plot.
         */
        _sendLegendExternalCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var legendExternal = this.m_legendExternal.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setLegendExternal";
                var params = "legendExternal:"+legendExternal;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        
        /**
         * Notify the server about changes to whether or not a legend line 
         * should be shown.
         */
        _sendLegendLineCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var legendLine = this.m_legendLine.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setLegendLine";
                var params = "legendLine:"+legendLine;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this._registerLegendPositions();
            this.m_legendLocate.setId( id );
        },
        
        m_id : null,
        m_legendId : null,
        m_connector : null,
        m_sharedVar : null,
        m_legendLine : null,
        m_legendShow : null,
        m_legendLocate : null,
        m_legendExternal : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});