/**
 * Displays controls for customizing the grid axes.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.GridAxes", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
        
        var pathDict = skel.widgets.Path.getInstance();
        
        this.m_sharedVar = this.m_connector.getSharedVar(pathDict.COORDINATE_SYSTEMS);
        this.m_sharedVar.addCB(this._systemsChangedCB.bind(this));
        this._systemsChangedCB();
    },
    
    statics : {
        MAX_SLIDER : 1000
    },

    members : {
        
       
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content );
            this.m_content.setLayout(new qx.ui.layout.VBox(2));
            this._initSystem();
            this._initVisible();
            this._initSliders();
        },
        
        /**
         * Initializes the sliders.
         */
        _initSliders : function(){
            var sliderContainer = new qx.ui.container.Composite();
            sliderContainer.setLayout( new qx.ui.layout.HBox(2));
            this.m_thickness = new skel.widgets.CustomUI.TextSlider("setGridThickness", "thickness",
                    skel.widgets.Grid.GridAxes.MAX_SLIDER, 25, "Thickness", false, "Set grid thickness.", "Slide to set grid thickness.",
                    "gridThicknessTextField", "gridThicknessSlider", true);
            this.m_spacing = new skel.widgets.CustomUI.TextSlider("setGridSpacing", "spacing",
                    skel.widgets.Grid.GridAxes.MAX_SLIDER, 25, "Spacing", false, "Set grid spacing.", "Slide to set grid spacing.",
                    "gridSpacingTextField", "gridSpacingSlider", true );
            this.m_transparency = new skel.widgets.CustomUI.TextSlider("setGridTransparency", "transparency",
                    skel.widgets.Grid.GridAxes.MAX_SLIDER, 25, "Transparency", false, "Set the grid transparency.", "Slide to set the grid transparency.",
                    "gridTransparencyTextField", "gridTransparencySlider", true);
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            sliderContainer.add( this.m_thickness );
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            sliderContainer.add( this.m_spacing );
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            sliderContainer.add( this.m_transparency );
            sliderContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            this.m_content.add( sliderContainer );
        },
        
        /**
         * Initializes the coordinate system.
         */
        _initSystem : function(){
            var systemContainer = new qx.ui.container.Composite();
            systemContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_coordSystem = new skel.boundWidgets.ComboBox("setCoordinateSystem", "skyCS");
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
            visibleContainer.setLayout( new qx.ui.layout.HBox());
            this.m_showAxes = new qx.ui.form.CheckBox( "Axes");
            this.m_showGridLines = new qx.ui.form.CheckBox( "Grid Lines");
            this.m_showInternalLabels = new qx.ui.form.CheckBox( "Internal Labels ");
            
            this.m_showAxes.setValue( true );
            this.m_showGridLines.setValue( true );
            this.m_showInternalLabels.setValue( true );
            this.m_showAxes.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowAxisCmd, this);
            this.m_showGridLines.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowGridLinesCmd, this );
            this.m_showInternalLabels.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowInternalLabelsCmd, this );
            
            visibleContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            visibleContainer.add( this.m_showAxes);
            visibleContainer.add( this.m_showGridLines );
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
         * Sends a command to the server to change whether or not the grid lines are visible.
         */
        _sendShowGridLinesCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setShowGridLines";
            var showLines = this.m_showGridLines.getValue();
            var params = "showGridLines:"+showLines;
            this.m_connector.sendCommand( cmd, params, function(){});
        },

        /**
         * Update the UI based on server-side axes settings.
         * @param controls {Object} - server side grid settings.
         */
        setControls : function( controls ){
            this.m_coordSystem.setComboValue( controls.skyCS );
            this._setShowAxes( controls.showAxis );
            this._setShowGridLines( controls.showGridLines );
            this._setShowInternalLabels( controls.showInternalLabels);
            this._setThickness( controls.grid.width );
            this._setTransparency( controls.transparency );
            this._setSpacing( controls.spacing );
            
        },
        
        /**
         * Set the visibility of the axes.
         * @param showAxes {boolean} - true if the axes should be shown; false otherwise.
         */
        _setShowAxes : function ( showAxes ){
            if ( this.m_showAxes.getValue() != showAxes ){
                this.m_showAxes.setValue( showAxes );
            }
        },
        
        /**
         * Set the visibility of the grid lines.
         * @param showGridLines {boolean} - true if the grid lines should be visible; false otherwise.
         */
        _setShowGridLines : function ( showGridLines ){
            if ( this.m_showGridLines.getValue() != showGridLines ){
                this.m_showGridLines.setValue( showGridLines );
            }
        },
        
        /**
         * Set whether or not the axes should be internal.
         * @param showInternalLabels {boolean} - true if the axes should be internal;
         *      false otherwise.
         */
        _setShowInternalLabels : function( showInternalLabels ){
            if ( this.m_showInternalLabels.getValue() != showInternalLabels ){
                this.m_showInternalLabels.setValue( showInternalLabels );
            }
        },

        /**
         * Set the thickness of the grid.
         * @param thickness {Number} the grid thickness.
         */
        _setThickness : function( thickness ){
            var thickFloat = thickness * skel.widgets.Grid.GridAxes.MAX_SLIDER;
            var normThickness = Math.round( thickFloat );
            if ( this.m_thickness.getValue() != normThickness ){
                this.m_thickness.setValue( normThickness );
            }
        },
        
        /**
         * Set the transparency of the grid.
         * @param transparency {Number} - the amount of grid transparency.
         */
        _setTransparency : function( transparency ){
            var transFloat = transparency * skel.widgets.Grid.GridAxes.MAX_SLIDER;
            var normTrans = Math.round( transFloat );
            if ( this.m_transparency.getValue() != normTrans ){
                this.m_transparency.setValue( normTrans );
            }
        },
        
        /**
         * Set the amount of spacing between grid lines.
         * @param spacing {Number} - the amount of spacing between grid lines.
         */
        _setSpacing : function( spacing ){
            var spaceFloat = spacing * skel.widgets.Grid.GridAxes.MAX_SLIDER;
            var normSpacing = Math.round( spaceFloat );
            if ( this.m_spacing.getValue() != normSpacing ){
                this.m_spacing.setValue( normSpacing );
            }
        },
        

        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_coordSystem.setId( gridId );
            this.m_thickness.setId( gridId );
            this.m_spacing.setId( gridId );
            this.m_transparency.setId( gridId );
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
        
        m_showAxes : null,
        m_showGridLines : null,
        m_showInternalLabels : null,
        
        m_thickness : null,
        m_spacing : null,
        m_transparency : null,
        
        m_sharedVar : null
        
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }


});