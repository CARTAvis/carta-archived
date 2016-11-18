/**
 * Displays controls for customizing the grid canvas.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.Settings.SettingsCanvasPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Settings", "");
        this.m_connector = mImport("connector");
        this._init();
        var pathDict = skel.widgets.Path.getInstance();
        
        this.m_sharedVar = this.m_connector.getSharedVar(pathDict.COORDINATE_SYSTEMS);
        this.m_sharedVar.addCB(this._systemsChangedCB.bind(this));
        this._systemsChangedCB();
    },

    members : {
        
       
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            
            this._setLayout(new qx.ui.layout.VBox(1));
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content );
            this.m_content.setLayout(new qx.ui.layout.VBox(1));
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
            this._initVisible();
            this._initSystem();
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
        },
        

        /**
         * Initializes the coordinate system.
         */
        _initSystem : function(){
            var systemContainer = new qx.ui.container.Composite();
            systemContainer.setLayout( new qx.ui.layout.HBox(1));
            
            this.m_coordSystem = new skel.boundWidgets.ComboBox("setCoordinateSystem", "skyCS");
            this.m_coordSystem.setToolTipText( "Select a coordinate system for the image.");
            skel.widgets.TestID.addTestId( this.m_coordSystem, "ImageCoordinateSystem");
            var systemLabel = new qx.ui.basic.Label( "System:");
            systemContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            systemContainer.add( systemLabel );
            systemContainer.add( this.m_coordSystem );
            systemContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            this.m_content.add( systemContainer );
        },
        
        /**
         * Initialize the visibility controls.
         */
        _initVisible : function(){
            var visibleContainer = new qx.ui.container.Composite();
            visibleContainer.setLayout( new qx.ui.layout.VBox());
            
            this.m_allImages = new qx.ui.form.CheckBox( "All Images");
            this.m_allImages.setToolTipText( "Apply grid control settings to all images in the stack.");
            this.m_allImages.setValue( false );
            this.m_allListenerId = this.m_allImages.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendApplyAllCmd, this );
            var allContainer = new qx.ui.container.Composite();
            allContainer.setLayout( new qx.ui.layout.HBox());
            allContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            allContainer.add( this.m_allImages );
            allContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            
            
            this.m_showSystem = new qx.ui.form.CheckBox( "Coordinate System");
            this.m_showSystem.setToolTipText( "Show the grid coordinate system.");
            this.m_showSystem.setValue( true );
            this.m_showListenerId = this.m_showSystem.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowSystemCmd, this);
            var sysContainer = new qx.ui.container.Composite();
            sysContainer.setLayout( new qx.ui.layout.HBox());
            sysContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            sysContainer.add( this.m_showSystem );
            sysContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            
            this.m_showStatistics = new qx.ui.form.CheckBox( "Cursor");
            this.m_showStatistics.setToolTipText( "Show/hide cursor information.");
            this.m_showStatistics.setValue( true );
            this.m_cursorListenerId = this.m_showStatistics.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowStatisticsCmd, this);
            var cursorContainer = new qx.ui.container.Composite();
            cursorContainer.setLayout( new qx.ui.layout.HBox());
            cursorContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            cursorContainer.add( this.m_showStatistics );
            cursorContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            
            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            visibleContainer.add( allContainer );
            visibleContainer.add( sysContainer );
            visibleContainer.add( cursorContainer );
            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            this.m_content.add( visibleContainer );
        },
        
        /**
         * Sends command to server for applying (or not) the grid settings to all stacked
         * images.
         */
        _sendApplyAllCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setApplyAll";
            var applyAll = this.m_allImages.getValue();
            var params = "applyAll:"+applyAll;
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        /**
         * Sends a command to the server to change the visibility of the image statistics.
         */
        _sendShowStatisticsCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setShowStatistics";
            var showStatistics = this.m_showStatistics.getValue();
            var params = "showStatistics:"+showStatistics;
            this.m_connector.sendCommand( cmd, params, function(){});
        },

        /**
         * Sends a command to the server to show/hide the grid coordinate system.
         */
        _sendShowSystemCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setShowCoordinateSystem";
            var showCoords = this.m_showSystem.getValue();
            var params = "showCoordinateSystem:"+showCoords;
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
       
        /**
         * Update the UI based on server-side grid settings.
         * @param controls {Object} - server side grid settings.
         */
        setControls : function( controls ){
            this._setApplyAll( controls.applyAll );
            this._setShowSystem( controls.grid.showCoordinateSystem );
            this._setCoordinateSystem( controls.grid.skyCS );
            this._setShowStatistics( controls.grid.showStatistics );
        },
        
        /**
         * Set whether or not the grid coordinate system should be visible
         *      based on server side values.
         * @param showSystem {boolean} - true if the grid coordinate system 
         *      should be shown; false otherwise.
         */
        _setShowSystem : function ( showSystem ){
            if ( typeof showSystem !== "undefined" && 
                    this.m_showSystem.getValue() != showSystem ){
                this.m_showSystem.removeListenerById( this.m_showListenerId );
                this.m_showSystem.setValue( showSystem );
                this.m_showListenerId = this.m_showSystem.addListener( skel.widgets.Path.CHANGE_VALUE, 
                        this._sendShowSystemCmd, this);
            }
        },
        
        /**
         * Set whether or not grid settings should be applied to all images in
         * the stack based on server side values.
         * @param applyAll {boolean} - true if the grid settings should be applied
         *      to all images in the stack; false otherwise.
         */
        _setApplyAll : function ( applyAll ){
            if ( typeof applyAll !== "undefined" && 
                    this.m_allImages.getValue() != applyAll ){
                this.m_allImages.removeListenerById( this.m_allListenerId );
                this.m_allImages.setValue( applyAll );
                this.m_allListenerId = this.m_allImages.addListener( skel.widgets.Path.CHANGE_VALUE, 
                        this._sendApplyAllCmd, this );
            }
        },
        
        /**
         * Set the coordinate system based on server side values.
         * @param skyCS {String} - an identifier for a coordinate system.
         */
        _setCoordinateSystem : function( skyCS ){
            if ( typeof skyCS !== "undefined"){
                this.m_coordSystem.setValue( skyCS );
            }
        },
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_coordSystem.setId( gridId );
        },
        
        /**
         * Set the visibility of the axes.
         * @param showAxes {boolean} - true if the axes should be shown; false otherwise.
         */
        _setShowStatistics : function ( showStatistics ){
            if ( typeof showStatistics !=="undefined" && 
                    this.m_showStatistics.getValue() != showStatistics ){
                this.m_showStatistics.removeListenerById( this.m_cursorListenerId );
                this.m_showStatistics.setValue( showStatistics );
                this.m_cursorListenerId = this.m_showStatistics.addListener( skel.widgets.Path.CHANGE_VALUE, 
                        this._sendShowStatisticsCmd, this);
            }
        },
        
        /**
         * Callback for a change in the available coordinate systems.
         */
        _systemsChangedCB : function(){
            if ( this.m_sharedVar ){
                var val = this.m_sharedVar.get();
                if ( val ){
                    try {
                        var systems = JSON.parse( val );
                        this.m_coordSystem.setComboItems( systems.skyCS );
                    }
                    catch( err ){
                        console.log( "Axes coordinate system could not parse: "+val );
                    }
                }
            }
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_coordSystem : null,
        
        m_showStatistics : null,
        m_showSystem : null,
        m_allImages : null,
        m_cursorListenerId : null,
        m_showListenerId : null,
        m_allListenerId : null,
        
        m_sharedVar : null
    }


});