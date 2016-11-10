/**
 * Controls for setting whether the histogram should be of the whole cube, a single
 * plane, or a range of planes.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramCube", {
    extend : qx.ui.core.Widget,

    construct : function(  ) {
        this.base(arguments);
        this._init( );
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVarUnit = this.m_connector.getSharedVar(path.CHANNEL_UNITS);
            this.m_sharedVarUnit.addCB(this._unitsChangedCB.bind(this));
        }
        this._unitsChangedCB();
    },
    
    statics : {
        CMD_SET_PLANE_MODE : "setPlaneMode",
        CMD_SET_RANGE : "setPlaneRange",
        CMD_SET_PLANE : "setPlaneSingle"
    },

    members : {

        /**
         * Callback for a server error when setting the plane range maximum value.
         * @param anObject {skel.widgets.Histogram.HistogramCube}.
         */
        _errorPlaneRangeMaxCB : function( anObject){
            return function( msg ){
                if ( msg !== null && msg.length > 0 ){
                    anObject.m_rangeMaxText.setError( true );
                }
                else {
                    this._clearErrors( anObject.m_rangeMaxText );
                }
            };
        },
        
        /**
         * Callback for a server error when setting the plane range minimum value.
         * @param anObject {skel.widgets.Histogram.HistogramCube}.
         */
        _errorPlaneRangeMinCB : function( anObject){
            return function( msg ){
                if ( msg !== null && msg.length > 0 ){
                    anObject.m_rangeMinText.setError( true );
                }
                else {
                    this._clearErrors( anObject.m_rangeMinText );
                }
            };
        },
        
        /**
         * Clear any server errors.
         * @param anObject {skel.widgets.Histogram.HistogramCube}.
         */
        _errorCB : function( anObject ){
            return function( msg ){
                if ( msg === null || msg.length == 0 ){
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();
                }
            }
        },
        
        /**
         * Clear the error indicators from the widget.
         * @param widget {skel.widgets.CustomUI.NumericTextField}.
         */
        _clearErrors : function( widget ){
            var oldError = widget.isError();
            if ( oldError ){
                widget.setError( false );
                var errorMan = skel.widgets.ErrorHandler.getInstance();
                errorMan.clearErrors();
            }
        },
        
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.VBox(1);
            this._setLayout(widgetLayout);
            this._initPlane();
        },
        
        /**
         * Initialize the controls that determine whether the histogram is of a single
         * plane or multiple planes.
         */
        _initPlane : function(){
            
            var planeContainer = new qx.ui.groupbox.GroupBox("Channel", "");
            planeContainer.setContentPadding(1,1,1,1);
            var boxLayout = new qx.ui.layout.VBox(1);
            planeContainer.setLayout( boxLayout );
            this._add( planeContainer );
            
            //All
            this.m_planeAll = new qx.ui.form.RadioButton( "All");
            this.m_planeAll.setToolTipText( "Compute based on the entire image.");
            this.m_planeAllListenerId = this.m_planeAll.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._planeAllChanged, this );
            this.m_smallCheck = new qx.ui.form.CheckBox();
            this.m_smallCheck.setToolTipText( "Whether or not a histogram of the entire cube should default to a histogram of the current channel for large cubes.");
            this.m_smallCheck.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._smallCheckChanged, this );
            var smallLabel = new qx.ui.basic.Label( "if size is <=");
            this.m_pixelText = new skel.widgets.CustomUI.NumericTextField( 0, null);
            this.m_pixelText.setToolTipText( "Cube size limit before the histogram of the entire cube defaults to a histogram of the current channel for performance reasons.");
            this.m_pixelText.addListener( "textChanged", this._sendCubeSizeCmd, this );
            var pixelLabel = new qx.ui.basic.Label( "pixels.");
           
            
            var topContainer = new qx.ui.container.Composite();
            var layout = new qx.ui.layout.HBox();
            layout.setAlignX( "center" );
            layout.setAlignY( "middle" );
            topContainer.setLayout( layout );
            topContainer.add( this.m_planeAll );
            topContainer.add( this.m_smallCheck);
            topContainer.add( smallLabel );
            topContainer.add( this.m_pixelText );
            topContainer.add( pixelLabel );
            planeContainer.add( topContainer );
            
            //Current plane
            this.m_planeSingle = new qx.ui.form.RadioButton( "Current");
            this.m_planeSingle.setToolTipText( "Compute based on the current channel.");
            this.m_planeSingleListenerId = this.m_planeSingle.addListener( skel.widgets.Path.CHANGE_VALUE, 
                   this._planeSingleChanged, this );
            planeContainer.add( this.m_planeSingle);
            
            //Channel selection
            this.m_planeChannel = new qx.ui.form.RadioButton( "Channel");
            this.m_planeChannel.setToolTipText( "Compute based on a specific channel.");
            this.m_planeChannelListenerId = this.m_planeChannel.addListener( skel.widgets.Path.CHANGE_VALUE, 
                   this._planeChannelChanged, this );
            this.m_planeChannelText = new skel.widgets.CustomUI.NumericTextField( 0, null );
            this.m_planeChannelText.setIntegerOnly( true );
            this.m_planeChannelText.addListener( "textChanged", 
                    this._sendPlaneChannelCmd, this );
            var channelComposite = new qx.ui.container.Composite();
            channelComposite.setLayout( new qx.ui.layout.HBox(2));
            channelComposite.add( this.m_planeChannel );
            channelComposite.add( this.m_planeChannelText );
            planeContainer.add( channelComposite );
            
            //Range
            var rangeComposite = new qx.ui.container.Composite();
            rangeComposite.setLayout( new qx.ui.layout.HBox(2));
            this.m_planeRange = new qx.ui.form.RadioButton( "Range");
            this.m_planeRange.setToolTipText( "Compute based on a range of frequencies.");
            this.m_planeRangeListenerId = this.m_planeRange.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._planeRangeChanged, this );
            rangeComposite.add( this.m_planeRange );
            this.m_unitCombo = new qx.ui.form.ComboBox();
            this.m_unitCombo.setToolTipText( "Select units for cube range.");
            this.m_unitCombo.setEnabled( false );
            this.m_unitCombo.addListener( skel.widgets.Path.CHANGE_VALUE, function(e){
                if ( this.m_id !== null ){
                    var unitName = e.getData();
                    //Send a command to the server to let them know the frequency unit changed.
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setCubeRangeUnit";
                    var params = "rangeUnit:"+unitName;
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
            },this);
            rangeComposite.add( this.m_unitCombo );
            
            var minContainer = new qx.ui.container.Composite();
            minContainer.setLayout( new qx.ui.layout.HBox(2));
            var minLabel = new qx.ui.basic.Label( "Min:");
            this.m_rangeMinText = new skel.widgets.CustomUI.NumericTextField(0, null);
            this.m_rangeMinText.setEnabled( false );
            this.m_rangeMinText.setIntegerOnly( false );
            this.m_rangeMinText.setToolTipText( "Smallest frequency in range.");
            minContainer.add( new qx.ui.core.Spacer(15) );
            minContainer.add( minLabel );
            minContainer.add(this.m_rangeMinText );
            this.m_minListenerId = this.m_rangeMinText.addListener( "textChanged",function(){
                this._minRangeChanged();
            }, this );
            
            var maxContainer = new qx.ui.container.Composite();
            maxContainer.setLayout( new qx.ui.layout.HBox(2));
            var maxLabel = new qx.ui.basic.Label( "Max:");
            this.m_rangeMaxText = new skel.widgets.CustomUI.NumericTextField(0, null);
            this.m_rangeMaxText.setToolTipText( "Largest frequency in range.");
            this.m_rangeMaxText.setEnabled( false );
            this.m_rangeMaxText.setIntegerOnly( false );
            this.m_maxListenerId = this.m_rangeMaxText.addListener( "textChanged", function(){
                this._maxRangeChanged();
            }, this );
            
            maxContainer.add( new qx.ui.core.Spacer(15));
            maxContainer.add( maxLabel );
            maxContainer.add( this.m_rangeMaxText );
            
            var radGroup = new qx.ui.form.RadioGroup();
            radGroup.add(this.m_planeAll, this.m_planeSingle, this.m_planeChannel, this.m_planeRange );
            planeContainer.add( rangeComposite );
            planeContainer.add( minContainer );
            planeContainer.add( maxContainer );
        },
        
        /**
         * Callback for a change in the maximum plane range.
         */
        _maxRangeChanged : function(){
            var value = this.m_rangeMaxText.getValue();
            var min = this.m_rangeMinText.getValue();
            if ( value >= min ){
                var cb = this._errorPlaneRangeMaxCB(this);
                this._clearErrors( this.m_rangeMaxText );
                this._sendChangeBoundsCmd( cb );
            }
            else {
                this.m_rangeMaxText.setError( true );
                var errorMan = skel.widgets.ErrorHandler.getInstance();
                errorMan.updateErrors( "Minimum range "+min+" must be less than maximimum: "+value);
            }
        },
        
        
        /**
         * Callback for a change in the minimum plane range.
         */
        _minRangeChanged : function(){
            var value = this.m_rangeMinText.getValue();
            var max = this.m_rangeMaxText.getValue();
            if ( value <= max ){
                var cb = this._errorPlaneRangeMinCB(this);
                this._clearErrors( this.m_rangeMinText );
                this._sendChangeBoundsCmd( cb );
            }
            else {
                this.m_rangeMinText.setError( true );
                var errorMan = skel.widgets.ErrorHandler.getInstance();
                errorMan.updateErrors( "Minimum range "+value+" must be less than  "+max);
            }
        },
        
        /**
         * Notify the server that the histogram's plane settings have changed.
         */
        _planeModeChanged : function( mode ){
            if ( this.m_id !== null ){
                if ( mode !== null && this.m_connector !== null){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramCube.CMD_SET_PLANE_MODE;
                    var params = "planeMode:"+mode;
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
            }
        },
        
        /**
         * Plane mode all changed its value.
         */
        _planeAllChanged : function(){
            if ( this.m_planeAll.getValue() ){
                this._planeModeChanged( this.m_planeAll.getLabel());
                this.m_smallCheck.setEnabled(true );
            }
            else {
                this.m_smallCheck.setEnabled( false );
            }
            var smallChecked = this.m_smallCheck.getValue();
            this.m_pixelText.setEnabled( smallChecked && this.m_smallCheck.isEnabled() );
        },
        
        /**
         * Plane mode channel changed its value.
         */
        _planeChannelChanged : function(){
            var planeChannelSelected = this.m_planeChannel.getValue();
            if ( planeChannelSelected ){
                this._planeModeChanged( this.m_planeChannel.getLabel());
            }
            this.m_planeChannelText.setEnabled( planeChannelSelected );
        },
        
        /**
         * Plane mode single changed its value.
         */
        _planeSingleChanged : function(){
            if ( this.m_planeSingle.getValue() ){
                this._planeModeChanged( this.m_planeSingle.getLabel());
            }
        },
        
        /**
         * Plane mode range changed its value.
         */
        _planeRangeChanged : function(){
            var planeChecked = this.m_planeRange.getValue();
            if ( planeChecked ){
                this._planeModeChanged( this.m_planeRange.getLabel());
            }
            this.setPlaneRangeEnabled( planeChecked );
        },
        
        /**
         * A change in the checked status in the UI of the checkbox
         * governing whether or not to default to a single channel for
         * large cubes.
         */
        _smallCheckChanged : function(){
            var checked = this.m_smallCheck.getValue();
            this.m_pixelText.setEnabled( checked);
            this._sendCubeLimitCmd();
        },
        
        /**
         * Send a command to the server as to whether or not a size limit
         * should be imposed on cubes where a histogram of the entire cube
         * is rendered.
         */
        _sendCubeLimitCmd : function(){
            if ( this.m_connector !== null && this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setCubeLimit";
                var params = "limitCubeSize:"+this.m_smallCheck.getValue();
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send a command to the server giving a new size limit of how large
         * a cube can be before a histogram of the entire cube will not be
         * rendered.
         */
        _sendCubeSizeCmd : function(){
            if ( this.m_connector !== null && this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setCubeSizeLimit";
                var params = "cubeSizeMax:"+this.m_pixelText.getValue();
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Notify the server that the upper and/or lower bound of planes
         * has changed.
         */
        _sendChangeBoundsCmd : function(cb){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramCube.CMD_SET_RANGE;
                var params = "planeMin:"+this.m_rangeMinText.getValue()+",planeMax:"+this.m_rangeMaxText.getValue();
                this.m_connector.sendCommand( cmd, params, cb);
            }
        },
        
        /**
         * Send a command to the server to manually set the histogram channel.
         */
        _sendPlaneChannelCmd : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setPlaneChannel";
                var params = "planeChannel:"+this.m_planeChannelText.getValue();
                this.m_connector.sendCommand( cmd, params, this._errorCB());
            }
        },
        
        
        /**
         * Set a range of planes to use in computing the histogram.
         * @param planeMin {number} a lower (inclusive) bound for the planes.
         * @param planeMax {number} an upper {inclusive} bound for the planes.
         */
        setPlaneBounds : function( planeMin, planeMax ){
            if ( 0<=planeMin && planeMin<= planeMax ){
                this.m_rangeMinText.removeListenerById( this.m_minListenerId );
                this.m_rangeMaxText.removeListenerById( this.m_maxListenerId );
                if ( this.m_rangeMinText.getValue() != planeMin ){
                    this.m_rangeMinText.setValue( planeMin );
                }
                if ( this.m_rangeMaxText.getValue() != planeMax ){
                    this.m_rangeMaxText.setValue( planeMax );
                }
                this.m_minListenerId =this.m_rangeMinText.addListener( "textChanged", 
                        this._minRangeChanged, this);
                this.m_maxListenerId =this.m_rangeMaxText.addListener( "textChanged", 
                        this._maxRangeChanged, this);
            }
        },
        
        /**
         * Update the manually set histogram channel based on information from the
         * server.
         * @param planeChannel {Number} - the channel index stored on the server.
         */
        setPlaneChannel : function( planeChannel ){
            if ( this.m_planeChannelText.getValue() != planeChannel ){
                this.m_planeChannelText.setValue( planeChannel );
            }
        },
        
        /**
         * Set the maximum allowed value for the plane channel based on server information.
         * @param planeChannelMax {Number} - the maximum allowed channel index.
         */
        setPlaneChannelMax : function( planeChannelMax ){
            this.m_planeChannelText.setUpperBound( planeChannelMax );
        },
        
        /**
         * Set parameters relating to how large a cube can be before a histogram
         * of the entire cube will not be rendered.
         * @param pixelLimit {boolean} - true if there should be a limit on the size
         *      of cubes; false otherwise.
         * @param pixelLimitSize {Number} - how big a cube can be before the cube
         *      histogram defaults to a single plane if a pixel limit has been imposed.
         */
        setCubeAllLimits : function( pixelLimit, pixelLimitSize ){
            this.m_smallCheck.setValue( pixelLimit );
            this.m_pixelText.setValue( pixelLimitSize );
        },
        
        /**
         * Sets whether or not the plane range controls should be enabled.
         * @param valid {boolean} - true if the image is NOT single plane;
         *      false otherwise.
         */
        setPlaneRangeEnabled : function( valid ){
            this.m_rangeMinText.setEnabled( valid );
            this.m_rangeMaxText.setEnabled( valid );
            this.m_unitCombo.setEnabled( valid );
        },
        
        /**
         * Set whether or not this histogram is of a single plane, range of planes, or
         *      a full cube.
         * @param mode {String} an identifier for the plane(s) to use in computing the histogram..
         */
        setPlaneMode : function ( mode ){
            this.m_planeAll.removeListenerById( this.m_planeAllListenerId );
            this.m_planeSingle.removeListenerById( this.m_planeSingleListenerId );
            this.m_planeChannel.removeListenerById( this.m_planeChannelListenerId );
            this.m_planeRange.removeListenerById( this.m_planeRangeListenerId );
           
            var planeMode = false;
            if ( mode == this.m_planeAll.getLabel()){
                if ( !this.m_planeAll.getValue()){
                    this.m_planeAll.setValue( true );
                }
                
            }
            else if ( mode == this.m_planeSingle.getLabel()){
                if ( !this.m_planeSingle.getValue()){
                    this.m_planeSingle.setValue( true );
                }
                
            }
            else if ( mode == this.m_planeRange.getLabel()){          	
                if ( !this.m_planeRange.getValue()){
                    this.m_planeRange.setValue( true );
                }
                planeMode = true;
            }
            else if ( mode == this.m_planeChannel.getLabel()){
                if ( !this.m_planeChannel.getValue()){
                    this.m_planeChannel.setValue( true );
                }
            }
            else {
                console.log( "Unrecognized plane mode"+mode);
            }
            this.setPlaneRangeEnabled( planeMode );
            this.m_planeAllListenerId = this.m_planeAll.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._planeAllChanged, this );
            this.m_planeSingleListenerId = this.m_planeSingle.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._planeSingleChanged, this );
            this.m_planeChannelListenerId = this.m_planeChannel.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._planeChannelChanged, this );
            this.m_planeRangeListenerId = this.m_planeRange.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._planeRangeChanged, this );
        },
        
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        /**
         * Updates the channel units change on the server.
         */
        _unitsChangedCB : function(){
            if ( this.m_sharedVarUnit ){
                var val = this.m_sharedVarUnit.get();
                if ( val ){
                    try {
                        var oldUnit = this.m_unitCombo.getValue();
                        var units = JSON.parse( val );
                        var unitsCount = units.channelUnitCount;
                        this.m_unitCombo.removeAll();
                        for ( var i = 0; i < unitsCount; i++ ){
                            var unitName = units.channelUnitList[i];
                            var tempItem = new qx.ui.form.ListItem( unitName );
                            this.m_unitCombo.add( tempItem );
                        }
                        //Try to reset the old selection
                        if ( oldUnit !== null ){
                            this.m_dataCombo.setValue( oldUnit );
                        }
                        //Select the first item
                        else if ( unitsCount > 0 ){
                            var selectables = this.m_unitCombo.getChildrenContainer().getSelectables(true);
                            if ( selectables.length > 0 ){
                                this.m_unitCombo.setValue( selectables[0].getLabel());
                            }
                        }
                    }
                    catch( err ){
                        console.log( "Could not parse cube units: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        /**
         * Set the channel unit used to specify a channel range.
         * @param unit {String} the new channel range unit.
         */
        setUnit : function( unit ){
            var selectables = this.m_unitCombo.getChildrenContainer().getSelectables(true);
            for ( var i = 0; i < selectables.length; i++ ){
                if ( selectables[i].getLabel() === unit ){
                    this.m_unitCombo.setValue( selectables[i].getLabel());
                    break;
                }
            }
        },
        
        m_id : null,
        m_connector : null,
        m_sharedVarUnit : null,
        m_minListenerId : null,
        m_maxListenerId : null,
        m_pixelText : null,
        m_planeAll : null,
        m_planeSingle : null,
        m_planeChannel : null,
        m_planeChanelText : null,
        m_planeRange : null,
        m_planeAllListenerId : null,
        m_planeChannelListenerId : null,
        m_planeSingleListenerId : null,
        m_planeRangeListenerId : null,
        m_rangeMinText : null,
        m_rangeMaxText : null,
        m_smallCheck : null,
        m_unitCombo : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});