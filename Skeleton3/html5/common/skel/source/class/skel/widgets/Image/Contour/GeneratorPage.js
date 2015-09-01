/**
 * Displays controls for generating a contour set.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Contour.GeneratorPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     * @param title {String} - the aspect of the grid being customized.
     */
    construct : function() {
        this.base(arguments, "Generate", "");
        this._init( );
        
        //Shared variable for method used to generate contour levels.
        this.m_connector = mImport( "connector" );
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVarGenerateModes = this.m_connector.getSharedVar(pathDict.CONTOUR_GENERATE_MODES);
        this.m_sharedVarGenerateModes.addCB(this._generateModesChangedCB.bind(this));
        this._generateModesChangedCB();
        
        this.m_sharedVarSpacingModes = this.m_connector.getSharedVar( pathDict.CONTOUR_SPACING_MODES );
        this.m_sharedVarSpacingModes.addCB( this._spacingModesChangedCB.bind(this));
        this._spacingModesChangedCB();
    },

    members : {
        
        /**
         * Disables/enables widgets when the user changes the method used to generate
         * contours.
         */
        _generateMethodChanged : function(){
            //If minimum is selected, spacing interval should be enabled; otherwise,
            //it should be disabled.
            var limitMethod = this.m_limitCombo.getValue();
            if ( this.m_limitCombo.getValue() == "Minimum" ){
                this.m_intervalWidget.setEnabled( true );
                this.m_limitMaxText.setEnabled( false );
            }
            else {
                this.m_intervalWidget.setEnabled( false );
                this.m_limitMaxText.setEnabled( true );
            }
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.VBox(2));
            
            //Name
            var nameContainer = new qx.ui.container.Composite();
            nameContainer.setLayout( new qx.ui.layout.HBox(2));
            nameContainer.add( new qx.ui.core.Spacer(), {flex:1});
            var nameLabel = new qx.ui.basic.Label( "Name:");
            nameContainer.add( nameLabel );
            this.m_nameText = new skel.widgets.CustomUI.ErrorTextField();
            nameContainer.add( this.m_nameText );
            nameContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            //Limit count
            var limitCountContainer = new qx.ui.container.Composite();
            limitCountContainer.setLayout( new qx.ui.layout.HBox(2));
            limitCountContainer.add( new qx.ui.core.Spacer(), {flex:1});
            var countLabel = new qx.ui.basic.Label( "Level Count:");
            this.m_levelCountSpin = new qx.ui.form.Spinner();
            this.m_levelCountSpin.setMinimum( 1 );
            this.m_levelCountSpin.addListener( "changeValue", this._sendLevelCountCmd, this );
            limitCountContainer.add( countLabel );
            limitCountContainer.add( this.m_levelCountSpin );
            limitCountContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            //Dashed negatives
            var negContainer = new qx.ui.container.Composite();
            negContainer.setLayout( new qx.ui.layout.HBox(2));
            negContainer.add( new qx.ui.core.Spacer(), {flex:1});
            var negLabel = new qx.ui.basic.Label( "Dashed Negative Contours");
            this.m_negativeCheck = new qx.ui.form.CheckBox();
            this.m_negativeCheck.addListener( "changeValue", this._sendNegativeDashedCmd, this );
            negContainer.add( negLabel );
            negContainer.add( this.m_negativeCheck );
            negContainer.add( new qx.ui.core.Spacer(), {flex:1});
           
            //Generate Methods
            var methodContainer = new qx.ui.container.Composite();
            methodContainer.setLayout( new qx.ui.layout.VBox(2));
            var limitContainer = new qx.ui.container.Composite();
            limitContainer.setLayout( new qx.ui.layout.HBox(2));
            var limitsLabel = new qx.ui.basic.Label( "Method:");
            this.m_limitCombo = new skel.boundWidgets.ComboBox("setGenerateMethod", "method");
            this.m_limitCombo.addListener( "changeValue", this._generateMethodChanged, this );
            limitContainer.add( limitsLabel );
            limitContainer.add( this.m_limitCombo );
            this.m_limitCombo.setToolTipText( "Specify a mode for generating levels.");
            
            var minMaxContainer = new qx.ui.container.Composite();
            var minMaxLayout = new qx.ui.layout.Grid();
            minMaxLayout.setColumnFlex( 0, 1 );
            minMaxLayout.setColumnFlex( 3, 1 );
            minMaxContainer.setLayout( minMaxLayout );
            var minLabel = new qx.ui.basic.Label( "Min:");
            this.m_limitMinText = new skel.widgets.CustomUI.NumericTextField(null,null);
            this.m_limitMinText.setIntegerOnly( false );
            this.m_limitMinText.addListener( "textChanged", this._sendLimitMinCmd, this );
            var maxLabel = new qx.ui.basic.Label( "Max:");
            this.m_limitMaxText = new skel.widgets.CustomUI.NumericTextField(null,null);
            this.m_limitMaxText.setIntegerOnly( false );
            this.m_limitMaxText.addListener( "textChanged", this._sendLimitMaxCmd, this );
            minMaxContainer.add( new qx.ui.core.Spacer(), {row:0,column:0, rowSpan:2});
            minMaxContainer.add( minLabel, {row:0,column:1});
            minMaxContainer.add( this.m_limitMinText, {row:0,column:2});
            minMaxContainer.add( maxLabel, {row:1,column:1});
            minMaxContainer.add( this.m_limitMaxText, {row:1,column:2});
            minMaxContainer.add( new qx.ui.core.Spacer(), {row:0, column:3, rowSpan:2});
            methodContainer.add( limitContainer );
            methodContainer.add( minMaxContainer );
            
            //Spacing
            var optionsContainer = new qx.ui.container.Composite();
            optionsContainer.setLayout( new qx.ui.layout.VBox(2));

            var spacingContainer = new qx.ui.container.Composite();
            spacingContainer.setLayout( new qx.ui.layout.HBox(2));
            var spacingLabel = new qx.ui.basic.Label( "Spacing:");
            this.m_spacingCombo = new skel.boundWidgets.ComboBox( "setSpacing", "method");
            spacingContainer.add( spacingLabel);
            spacingContainer.add( this.m_spacingCombo );
            
            //Spacing Interval
            var intervalContainer = new qx.ui.container.Composite();
            intervalContainer.setLayout( new qx.ui.layout.HBox(2));
            intervalContainer.add( new qx.ui.core.Spacer(), {flex:1});
            var intervalLabel = new qx.ui.basic.Label( "Interval:");
            this.m_intervalWidget = new skel.widgets.CustomUI.NumericTextField(0.0001,null);
            this.m_intervalWidget.setIntegerOnly( false );
            this.m_intervalWidget.addListener( "textChanged", this._sendIntervalCmd, this );
            intervalContainer.add( intervalLabel );
            intervalContainer.add( this.m_intervalWidget );
            intervalContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            optionsContainer.add( limitCountContainer );
            optionsContainer.add( spacingContainer );
            optionsContainer.add( intervalContainer );
            
            var butContainer = new qx.ui.container.Composite();
            butContainer.setLayout( new qx.ui.layout.HBox(2));
            butContainer.add( new qx.ui.core.Spacer(), {flex:1});
            this.m_generateButton = new qx.ui.form.Button( "Generate Contour Set");
            this.m_generateButton.addListener( "execute", this._sendGenerateCmd, this );
            butContainer.add( this.m_generateButton );
            butContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            var settingsContainer = new qx.ui.container.Composite();
            settingsContainer.setLayout( new qx.ui.layout.HBox(2));
            settingsContainer.add( methodContainer );
            settingsContainer.add( optionsContainer );
            
            this._add( nameContainer );
            this._add( negContainer );
            this._add( settingsContainer );
            this._add( butContainer );
        },
        
        /**
         * Callback from the server when the list of available methods used to generate
         * contours changes.
         */
        _generateModesChangedCB : function(){
            if ( this.m_sharedVarGenerateModes ){
                var val = this.m_sharedVarGenerateModes.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        this.m_limitCombo.setComboItems( obj.generateModes );
                    }
                    catch( err ){
                        console.log( "Contour generate page could not parse: "+val );
                        console.log( "Error: "+err );
                    }
                }
            }
        },
      
        /**
         * Send a command to the server specifying the method used to generate contour
         * levels.
         */
        _sendGenerateCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "generateLevels";
                var name = this.m_nameText.getValue();
                var params = "name:"+name;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send a command to the server specifying spacing between contour levels.
         */
        _sendIntervalCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null){
                var interval = this.m_intervalWidget.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setInterval";
                var params = "interval:"+interval;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send a command to the server specifying the number of contour levels.
         */
        _sendLevelCountCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null){
                var levelCount = this.m_levelCountSpin.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setLevelCount";
                var params = "count:"+levelCount;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send a command to the server specifying the minimum contour level.
         */
        _sendLimitMinCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null){
                var limitMin = this.m_limitMinText.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setLevelMin";
                var params = "min:"+limitMin;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send a command to the server specifying the maximum contour level.
         */
        _sendLimitMaxCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null){
                var limitMax = this.m_limitMaxText.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setLevelMax";
                var params = "max:"+limitMax;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send a command to the server specifying whether or not negative contours
         * should be dashed.
         */
        _sendNegativeDashedCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null){
                var dashed = this.m_negativeCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setDashedNegative";
                var params = "dashed:"+dashed;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Update from the server when the contour controls have changed.
         * @param controls {Object} - information about the contour controls from the server.
         */
        setControls : function( controls ){
            this._setContourSetName( controls.contourSets.length );
            this._setDashedNegative( controls.dashedNegative );
            this._setGenerateMethod( controls.generateMode );
            this._setLevelCountMax( controls.levelCountMax );
            this._setLevelCount( controls.levelCount );
            this._setSpacingMethod( controls.spacingMode );
            this._setRange( controls.rangeMin, controls.rangeMax );
            this._setInterval( controls.spacingInterval );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param contourId {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( contourId ){
            this.m_id = contourId;
            this.m_limitCombo.setId( contourId );
            this.m_spacingCombo.setId( contourId );
        },
        
        /**
         * Set a default name for the contour set based on the number of contour
         * sets available.
         * @param setCount {Number} - the index of the contour set.
         */
        _setContourSetName : function( setCount ){
            var textValue = this.m_nameText.getValue();
            var suggestName = "Contour Set "+ setCount;
            if ( textValue === null || suggestName != textValue ){
                this.m_nameText.setValue( suggestName );
            }
        },
        
        /**
         * Update the UI based on server-side information about whether negative contours
         * should be dashed.
         * @param dashed {boolean} - true if negative contours should be dashed; false otherwise.
         */
        _setDashedNegative : function( dashed ){
            if ( this.m_negativeCheck.getValue() != dashed ){
                this.m_negativeCheck.setValue( dashed );
            }
        },
        
        /**
         * Update the UI based on server-side information about spacing between contour levels.
         * @param value {Number} - spacing between contour levels.
         */
        _setInterval : function( value ){
            var interval = this.m_intervalWidget.getValue();
            if ( interval === null || interval != value ){
                this.m_intervalWidget.setValue( value.toString() );
            }
        },
        
        /**
         * Update the UI based on server-side information about the method used to generate
         * contours.
         * @param method {String} - an identifier for the method used to generate contours.
         */
        _setGenerateMethod : function( method ){
            if ( this.m_limitCombo.getValue() != method ){
                this.m_limitCombo.setComboValue( method );
            }
        },
        
        /**
         * Update the UI based on server-side information about the number of
         * contour levels.
         * @param count {Number} - the number of contour levels.
         */
        _setLevelCount : function( count ){
            if ( this.m_levelCountSpin.getValue() != count ){
                this.m_levelCountSpin.setValue( count );
            }
        },
        
        /**
         * Update the UI based on server-side information about the maximum number of
         * contour levels that can be specified.
         * @param dashed {max} - the maximum allowed number of contour levels.
         */
        _setLevelCountMax : function( max ){
            if ( this.m_levelCountSpin.getMaximum() != max ){
                this.m_levelCountSpin.setMaximum( max );
            }
        },
        
        /**
         * Update the UI based on server-side information about the minimum and
         * maximum contour level.
         * @param min {Number} - the minimum contour level.
         * @param max {Number} - the maximum contour level.
         */
        _setRange : function( min, max ){
            if ( min != this.m_limitMinText.getValue() ){
                this.m_limitMinText.setValue( min.toString() );
            }
            if ( max != this.m_limitMaxText.getValue() ){
                this.m_limitMaxText.setValue( max.toString() );
            }
        },
        
        /**
         * Update the UI based on server-side information about how spacing should
         * be distributed between contour levels.
         * @param method {String} - how spacing should be distributed between contour levels.
         */
        _setSpacingMethod : function( method ){
            if ( this.m_spacingCombo.getValue() != method ){
                this.m_spacingCombo.setComboValue( method );
            }
        },
        
        /**
         * Callback from the server when the available spacing methods between contour levels
         * have changed.
         */
        _spacingModesChangedCB : function(){
            if ( this.m_sharedVarSpacingModes ){
                var val = this.m_sharedVarSpacingModes.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        this.m_spacingCombo.setComboItems( obj.spacingModes );
                    }
                    catch( err ){
                        console.log( "Contour generate page could not parse spacing modes: "+val );
                        console.log( "Error: "+err );
                    }
                }
            }
        },
        
        m_id : null,
        m_connector : null,
        
        m_controls : null,
        m_intervalWidget : null,
        m_levelCountSpin : null,
        m_limitCombo : null,
        m_limitMinText : null,
        m_limitMaxText : null,
        m_nameText : null,
        m_negativeCheck : null,
        m_sharedVarGenerateModes : null,
        m_sharedVarSpacingModes : null,
        m_spacingCombo : null,
        m_generateButton : null

    }
});