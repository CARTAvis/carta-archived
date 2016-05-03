/**
 * Controls for setting the rest frequency used to calculate a profile.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsProfilesRest", {
    extend : qx.ui.core.Widget,

    construct : function( ) {
        this.base(arguments);
        this._init( );
        
        //Initiate connector.
         if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
             
             var path = skel.widgets.Path.getInstance();
             
             //Frequency units
             this.m_sharedVarFreqUnits = this.m_connector.getSharedVar(path.PROFILE_FREQ_UNITS);
             this.m_sharedVarFreqUnits.addCB(this._freqUnitsChangedCB.bind(this));
             this._freqUnitsChangedCB();
             
             //Wavelength units
             this.m_sharedVarWaveUnits = this.m_connector.getSharedVar(path.PROFILE_WAVE_UNITS);
             this.m_sharedVarWaveUnits.addCB(this._waveUnitsChangedCB.bind(this));
             this._waveUnitsChangedCB();
         }
    },
    
   
    members : {
        
        /**
         * Notification that the available frequency units have changed on the
         * server.
         */
        _freqUnitsChangedCB : function(){
            if ( this.m_sharedVarFreqUnits ){
                var val = this.m_sharedVarFreqUnits.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        this.m_unitsFreq = obj.units;
                        this._unitChanged();
                    }
                    catch( err ){
                        console.log( "Could not parse rest frequency units: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.VBox(1);
            this._setLayout(widgetLayout);
            
            var overallContainer = new qx.ui.container.Composite();
            overallContainer.setLayout( new qx.ui.layout.VBox(1));
            this._add( overallContainer );
            
            var checkContainer = new qx.ui.container.Composite();
            checkContainer.setLayout( new qx.ui.layout.HBox(1));
            var typeLabel = new qx.ui.basic.Label( "Rest:");
            this.m_freqRadio = new qx.ui.form.RadioButton( "Frequency");
            this.m_freqRadio.setValue( true );
            this.m_unitListenId = this.m_freqRadio.addListener( "changeValue", this._unitChanged, this );
            this.m_waveRadio = new qx.ui.form.RadioButton( "Wavelength");
            var typeGroup = new qx.ui.form.RadioGroup();
            typeGroup.add( this.m_freqRadio );
            typeGroup.add( this.m_waveRadio );
            checkContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            checkContainer.add( typeLabel );
            checkContainer.add( this.m_freqRadio );
            checkContainer.add( this.m_waveRadio );
            checkContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            overallContainer.add( checkContainer, {flex:1});
            
            var valueContainer = new qx.ui.container.Composite();
            valueContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_freqText = new skel.widgets.CustomUI.NumericTextField(0,null);
            this.m_freqText.setIntegerOnly( false );
            this.m_freqText.addListener( "textChanged", this._sendRestFreqCmd, this );
            this.m_freqUnitsSelect = new skel.widgets.CustomUI.SelectBox();
            this.m_freqUnitsSelect.addListener( "selectChanged", this._sendUnitsCmd, this );
            this.m_resetButton = new qx.ui.form.Button( "Reset");
            this.m_resetButton.addListener( "execute", this._sendResetCmd, this );
            valueContainer.add( this.m_freqText );
            valueContainer.add( this.m_freqUnitsSelect );
            valueContainer.add( this.m_resetButton );
            overallContainer.add( valueContainer );
        },
        
        /**
         * Send a command to the server to set the rest frequency.
         */
        _sendRestFreqCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var restFreq = this.m_freqText.getValue();
                var cmd = this.m_id + path.SEP_COMMAND + "setRestFrequency";
                var params = "name:"+this.m_curveName+", restFrequency:"+restFreq;
                this.m_connector.sendCommand( cmd, params, null );
             }
        },
        
        /**
         * Send a command to the server to reset the rest frequency to its
         * original value.
         */
        _sendResetCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "resetRestFrequency";
                var params = "name:"+this.m_curveName;
                this.m_connector.sendCommand( cmd, params, null );
             }
        },
        
        /**
         * Send a command to the server to set the type of units used to
         * specify rest frequency.
         */
        _sendRestUnitsChangedCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
               var freqUnits = this.m_freqRadio.getValue();
               var path = skel.widgets.Path.getInstance();
               var cmd = this.m_id + path.SEP_COMMAND + "setRestUnitType";
               var params = "restFrequencyUnits:"+freqUnits+",name:"+this.m_curveName;
               console.log( "Rest unit type changed params="+params);
               this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server to set the rest frequency units.
         */
        _sendUnitsCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var restUnits = this.m_freqUnitsSelect.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setRestUnit";
                var params = "restUnitFreq:"+restUnits+",name:"+this.m_curveName;
                this.m_connector.sendCommand( cmd, params, null );
             }
        },
        
        /**
         * Set the server side id of the object managing state for this UI..
         * @param id {String} the server side id of the object.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        /**
         * The user has changed the type of units used to specify rest
         * frequency.
         */
        _unitChanged : function(){
            //Update the available units based on what is checked.
            if ( this.m_freqRadio.getValue() ){
                if ( this.m_unitsFreq != null ){
                    this.m_freqUnitsSelect.setSelectItems( this.m_unitsFreq );
                }
            }
            else {
                if ( this.m_unitsWave != null ){
                    this.m_freqUnitsSelect.setSelectItems( this.m_unitsWave );
                }
            }
            this._sendRestUnitsChangedCmd();
        },
        
        /**
         * A new profile curve has been selected.
         * @param curveInfo {Object} - information about the selected curve.
         */
        update : function( curveInfo ){
            this.m_curveName = curveInfo.name;
            if ( this.m_freqText !== null ){
                this.m_freqText.setValue( curveInfo.restFrequency );
            }
            var freq = curveInfo.restFrequencyUnits;
            if ( this.m_freqRadio.getValue() != freq ){
                if ( this.m_unitListenId !== null ){
                    this.m_freqRadio.removeListenerById( this.m_unitListenId );
                }
                this.m_freqRadio.setValue( freq );
                this.m_unitListenId = this.m_freqRadio.addListener( "changeValue", this._unitChanged, this );
            }
            if ( this.m_freqRadio.getValue()){
                this.m_freqUnitsSelect.setSelectValue( curveInfo.restUnitFreq );
            }
            else {
                this.m_freqUnitsSelect.setSelectValue( curveInfo.restUnitWave );
            }
        },
        
        /**
         * Update from the server indicating the available wavelength units have
         * changed.
         */
        _waveUnitsChangedCB : function(){
            if ( this.m_sharedVarWaveUnits ){
                var val = this.m_sharedVarWaveUnits.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        this.m_unitsWave = obj.units;
                        this._unitChanged();
                    }
                    catch( err ){
                        console.log( "Could not parse rest frequency units: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
       
        m_id : null,
        m_connector : null,
        m_curveName : "",
        m_sharedVarFreqUnits : null,
        m_sharedVarWaveUnits : null,
        m_unitListenId : null,
        m_unitsFreq : null,
        m_unitsWave : null,
        
        m_freqRadio : null,
        m_waveRadio : null,
        m_freqText : null,
        m_freqUnitsSelect : null,
        m_resetButton : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});
