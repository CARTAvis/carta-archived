/**
 * Displays controls for customizing the grid lines.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.Settings.SettingsGridPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Settings", "");
        if ( typeof mImport !== "undefined"){
        	this.m_connector = mImport("connector");
        }
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
            this.m_thickness = new skel.widgets.CustomUI.TextSlider("setGridThickness", "grid",
                    1, 10, 1, "Thickness", false, "Set grid thickness.", "Slide to set grid thickness.",
                    "gridThicknessTextField", "gridThicknessSlider", false);
            this.m_transparency = new skel.widgets.CustomUI.TextSlider("setGridTransparency", "alpha",
                    0, skel.widgets.Path.MAX_RGB, 25, "Opacity", false, "Set the grid opacity.", "Slide to set the grid opacity.",
                    "gridTransparencyTextField", "gridTransparencySlider", false);
            this.m_spacing = new skel.widgets.CustomUI.TextSlider("setGridSpacing", "spacing",
                    0, skel.widgets.CustomUI.TextSlider.MAX_SLIDER, 25, "Spacing", false, "Set grid spacing.", "Slide to set grid spacing.",
                    "gridSpacingTextField", "gridSpacingSlider", true );
            
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            sliderContainer.add( this.m_thickness );
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            sliderContainer.add( this.m_transparency );
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            sliderContainer.add( this.m_spacing );
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            
            this.m_content.add( sliderContainer );
        },
       
        
        /**
         * Initialize the visibility controls.
         */
        _initVisible : function(){
            var visibleContainer = new qx.ui.container.Composite();
            visibleContainer.setLayout( new qx.ui.layout.HBox());
            this.m_showGridLines = new qx.ui.form.CheckBox( "Grid Lines");
            this.m_showGridLines.setValue( true );
            this.m_showGridLines.setToolTipText( "Show/hide grid lines.");
            this.m_showListenerId = this.m_showGridLines.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowGridLinesCmd, this );
            
            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            visibleContainer.add( this.m_showGridLines );
            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            this.m_content.add( visibleContainer );
        },

        
        /**
         * Sends a command to the server to change whether or not the grid lines are visible.
         */
        _sendShowGridLinesCmd : function(){
        	if ( this.m_connector !== null ){
        		var path = skel.widgets.Path.getInstance();
        		var cmd = this.m_id + path.SEP_COMMAND + "setShowGridLines";
        		var showLines = this.m_showGridLines.getValue();
        		var params = "showGridLines:"+showLines;
        		this.m_connector.sendCommand( cmd, params, function(){});
        	}
        },

        /**
         * Update the UI based on server-side grid settings.
         * @param controls {Object} - server side grid settings.
         */
        setControls : function( controls ){
            this._setShowGridLines( controls.grid.showGridLines );
            if ( typeof controls.grid.grid !== "undefined"){
                this._setThickness( controls.grid.grid.width );
                this._setTransparency( controls.grid.grid.alpha);
                this._setSpacing( controls.grid.spacing );
            }
        },
        
        /**
         * Set the enabled status of the custom grid controls based on
         * whether the grid lines are being shown or not.
         */
        _setControlsEnabledStatus : function(){
        	var enabled = this.m_showGridLines.getValue();
        	this.m_thickness.setEnabled( enabled );
        	this.m_spacing.setEnabled( enabled );
        	this.m_transparency.setEnabled( enabled );
        },

        
        /**
         * Set the visibility of the grid lines.
         * @param showGridLines {boolean} - true if the grid lines should be visible; false otherwise.
         */
        _setShowGridLines : function ( showGridLines ){
            if ( typeof showGridLines !== "undefined"){ 
                if ( this.m_showGridLines.getValue() != showGridLines ){
                	this.m_showGridLines.removeListenerById( this.m_showListenerId );
                	this.m_showGridLines.setValue( showGridLines );
                	this.m_showListenerId = this.m_showGridLines.addListener( skel.widgets.Path.CHANGE_VALUE, 
                        this._sendShowGridLinesCmd, this );
                }
                this._setControlsEnabledStatus();
            }
            
        },
        

        /**
         * Set the thickness of the grid lines.
         * @param thickness {Number} the grid line thickness.
         */
        _setThickness : function( thickness ){
            if ( typeof thickness !=="undefined" && 
                    this.m_thickness.getValue() != thickness ){
                this.m_thickness.setValue( thickness );
            }
        },

        
        /**
         * Set the amount of spacing between grid lines.
         * @param spacing {Number} - the amount of spacing between grid lines.
         */
        _setSpacing : function( spacing ){
            if ( typeof spacing !== "undefined"){
                var spaceFloat = spacing * skel.widgets.CustomUI.TextSlider.MAX_SLIDER;
                var normSpacing = Math.round( spaceFloat );
                if ( this.m_spacing.getValue() != normSpacing ){
                    this.m_spacing.setValue( normSpacing );
                }
            }
        },
        
        /**
         * Set the transparency of the grid lines.
         * @param transparency {Number} - the amount of grid line transparency.
         */
        _setTransparency : function( transparency ){
            if ( typeof transparency !== "undefined" && 
                    this.m_transparency.getValue() != transparency ){
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
            this.m_spacing.setId( gridId );
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_showGridLines : null,
        m_showListenerId : null,
        
        m_thickness : null,
        m_spacing : null,
        m_transparency : null
    }
});