/**
 * Displays controls for customizing the grid ticks.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.Settings.SettingsTicksPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function(  ) {
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
            this.m_thickness = new skel.widgets.CustomUI.TextSlider("setTickThickness", "tick",
                    1, 10, 1, "Thickness", false, "Set tick thickness.", "Slide to set tick thickness.",
                    "tickThicknessTextField", "tickThicknessSlider", false);
            this.m_transparency = new skel.widgets.CustomUI.TextSlider("setTickTransparency", "alpha",
                    0, skel.widgets.Path.MAX_RGB, 25, "Transparency", false, "Set the tick transparency.", "Slide to set the tick transparency.",
                    "tickTransparencyTextField", "tickTransparencySlider", false);
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            sliderContainer.add( this.m_thickness );
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
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
            this.m_showTicks = new qx.ui.form.CheckBox( "Show Ticks");
            this.m_showTicks.setToolTipText( "Show/hide axis ticks.");
            this.m_showTicks.setValue( true );
            this.m_showListenerId = this.m_showTicks.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowTicksCmd, this );

            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            visibleContainer.add( this.m_showTicks);
            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            this.m_content.add( visibleContainer );
        },

        /**
         * Sends a command to the server to show/hide the ticks.
         */
        _sendShowTicksCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setShowTicks";
            var showTicks = this.m_showTicks.getValue();
            var params = "showTicks:"+showTicks;
            this.m_connector.sendCommand( cmd, params, function(){});
        },

        /**
         * Update the UI based on server-side grid settings.
         * @param controls {Object} - server side grid settings.
         */
        setControls : function( controls ){
            this._setShowTicks( controls.grid.showTicks );
            this._setThickness( controls.grid.tick.width );
            this._setTransparency( controls.grid.tick.alpha );
        },
        
        /**
         * Set the visibility of the ticks based on server-side information.
         * @param showTicks {boolean} - true if the ticks should be shown; false otherwise.
         */
        _setShowTicks : function ( showTicks ){
            if ( this.m_showTicks.getValue() != showTicks ){
                this.m_showTicks.removeListenerById( this.m_showListenerId );
                this.m_showTicks.setValue( showTicks );
                this.m_showListenerId = this.m_showTicks.addListener( skel.widgets.Path.CHANGE_VALUE, 
                        this._sendShowTicksCmd, this );
            }
        },
        

        /**
         * Set the length of the ticks based on server-side information.
         * @param thickness {Number} the tick thickness.
         */
        _setThickness : function( thickness ){
            if ( this.m_thickness.getValue() != thickness ){
                this.m_thickness.setValue( thickness );
            }
        },
        
        
        /**
         * Set the transparency of the ticks.
         * @param transparency {Number} - the amount of tick transparency.
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
            this.m_transparency.setId( gridId );
            this.m_thickness.setId( gridId );
        },

        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_showTicks : null,
        m_showListenerId : null,
        m_thickness : null,
        m_transparency : null
    }

});