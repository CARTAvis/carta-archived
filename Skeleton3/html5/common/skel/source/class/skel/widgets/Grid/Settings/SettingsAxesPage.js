/**
 * Displays controls for customizing the grid axes.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.Settings.SettingsAxesPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Settings", "");
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content, {flex:1} );
            this.m_content.setLayout(new qx.ui.layout.VBox(2));
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
            this._initVisible();
            this._initSliders();
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
        },
        
        /**
         * Initializes the sliders.
         */
        _initSliders : function(){
            var sliderContainer = new qx.ui.container.Composite();
            sliderContainer.setLayout( new qx.ui.layout.HBox(2));
            this.m_thickness = new skel.widgets.CustomUI.TextSlider("setAxesThickness", "axes",
                    1, 10, 1, "Thickness", false, "Set axes thickness.", "Slide to set axes thickness.",
                    "axesThicknessTextField", "axesThicknessSlider", false);
            this.m_transparency = new skel.widgets.CustomUI.TextSlider("setAxesTransparency", "alpha",
                    0, skel.widgets.Path.MAX_RGB, 25, "Transparency", false, 
                    "Set the axes transparency.", "Slide to set the axes transparency.",
                    "axesTransparencyTextField", "axesTransparencySlider", false);
         
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            sliderContainer.add( this.m_thickness );
            sliderContainer.add( this.m_transparency );
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
           
            this.m_content.add( sliderContainer );
        },
        
        
        /**
         * Initialize the visibility controls.
         */
        _initVisible : function(){
            var visibleContainer = new qx.ui.container.Composite();
            visibleContainer.setLayout( new qx.ui.layout.HBox());
            this.m_showAxes = new qx.ui.form.CheckBox( "Axes/Border");
            this.m_showAxes.setToolTipText( "Show/hide the axes/border.");
            this.m_showInternalLabels = new qx.ui.form.CheckBox( "Internal Axes");
            this.m_showInternalLabels.setToolTipText( "Use internal axes as opposed to external ones.");
            this.m_showAxes.setValue( true );
            this.m_showInternalLabels.setValue( true );
            this.m_showListenerId = this.m_showAxes.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._showAxesChanged, this);
           
            this.m_internalListenerId = this.m_showInternalLabels.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowInternalLabelsCmd, this );
            
            
            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            visibleContainer.add( this.m_showAxes);
            visibleContainer.add( this.m_showInternalLabels );
            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            this.m_content.add( visibleContainer );
        },

        /**
         * Sends a command to the server to change the visibility of the axes.
         */
        _sendShowAxisCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setShowAxis";
            var showAxis = this.m_showAxes.getValue();
            var params = "showAxis:"+showAxis;
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        /**
         * Sends a command to the server to change whether the axes are internal/external.
         */
        _sendShowInternalLabelsCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setShowInternalLabels";
            var showLabels = this.m_showInternalLabels.getValue();
            var params = "showInternalLabels:"+showLabels;
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        

        /**
         * Update the UI based on server-side axes settings.
         * @param controls {Object} - server side grid settings.
         */
        setControls : function( controls ){
            this._setShowAxes( controls.grid.showAxis );
            this._setShowInternalLabels( controls.grid.showInternalLabels);
            this._setThickness( controls.grid.axes.width );
            this._setTransparency( controls.grid.axes.alpha );
        },
        
        /**
         * Set the visibility of the axes.
         * @param showAxes {boolean} - true if the axes should be shown; false otherwise.
         */
        _setShowAxes : function ( showAxes ){
            if ( this.m_showAxes.getValue() != showAxes ){
                this.m_showAxes.removeListenerById( this.m_showListenerId );
                this.m_showAxes.setValue( showAxes );
                this.m_showListenerId = this.m_showAxes.addListener( skel.widgets.Path.CHANGE_VALUE, 
                        this._showAxesChanged, this);
            }
        },
       
        
        /**
         * Set whether or not the axes should be internal.
         * @param showInternalLabels {boolean} - true if the axes should be internal;
         *      false otherwise.
         */
        _setShowInternalLabels : function( showInternalLabels ){
            if ( this.m_showInternalLabels.getValue() != showInternalLabels ){
                this.m_showInternalLabels.removeListenerById( this.m_internalListenerId );
                this.m_showInternalLabels.setValue( showInternalLabels );
                this.m_internalListenerId = this.m_showInternalLabels.addListener( skel.widgets.Path.CHANGE_VALUE, 
                        this._sendShowInternalLabelsCmd, this );
            }
        },
        
        /**
         * User has toggled show/hide axes.
         */
        _showAxesChanged : function(){
            var showAxes = this.m_showAxes.getValue();
            this.m_showInternalLabels.setEnabled( showAxes );
            this._sendShowAxisCmd();
        },

        /**
         * Set the thickness of the axes.
         * @param thickness {Number} the axes thickness.
         */
        _setThickness : function( thickness ){
            if ( this.m_thickness.getValue() != thickness ){
                this.m_thickness.setValue( thickness );
            }
        },
        
        /**
         * Set the transparency of the axes.
         * @param transparency {Number} - the amount of axes transparency.
         */
        _setTransparency : function( transparency ){
            if ( this.m_transparency.getValue() != transparency ){
                this.m_transparency.setValue( transparency );
            }
        },

        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_thickness.setId( gridId );
            this.m_transparency.setId( gridId );
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_showAxes : null,
        m_showInternalLabels : null,
        
        m_showListenerId : null,
        m_internalListenerId : null,
        
        m_thickness : null,
        m_transparency : null
    }


});