/**
 * Displays controls for hiding/showing various aspects of profile
 * fitting.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.FitDisplay", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
        this._init( );
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.HBox());
            
            var displayContainer = new qx.ui.groupbox.GroupBox( "Display", "");
            var gridLayout =  new qx.ui.layout.Grid();
            gridLayout.setColumnAlign(0, "right","middle");
            gridLayout.setColumnAlign(1, "center", "middle");
            displayContainer.setLayout(gridLayout);
            displayContainer.setContentPadding(1,1,1,1);
            this._add( displayContainer );
           
            this.m_residualCheck = new qx.ui.form.CheckBox();
            var resLabel = new qx.ui.basic.Label( "Residuals:");
            this.m_residualListenId = this.m_residualCheck.addListener( "changeValue", this._sendResidualCmd, this );
            displayContainer.add( resLabel, {row:0, column:0} );
            displayContainer.add( this.m_residualCheck, {row:0, column:1} );
            
            this.m_guessCheck = new qx.ui.form.CheckBox();
            var guessLabel = new qx.ui.basic.Label( "Manual Guesses:");
            this.m_guessListenId = this.m_guessCheck.addListener( "changeValue", this._sendShowGuessesCmd, this );
            displayContainer.add( guessLabel, {row:1, column:0} );
            displayContainer.add( this.m_guessCheck, {row:1, column:1} );
            
            this.m_statisticsCheck = new qx.ui.form.CheckBox();
            var statLabel = new qx.ui.basic.Label( "Statistics:");
            this.m_statisticsListenId = this.m_statisticsCheck.addListener( "changeValue", this._sendShowStatisticsCmd, this );
            displayContainer.add( statLabel, {row:2, column:0} );
            displayContainer.add( this.m_statisticsCheck, {row:2, column:1} );
            
            this.m_meanRMSCheck = new qx.ui.form.CheckBox();
            var meanLabel = new qx.ui.basic.Label( "Mean and RMS:");
            this.m_meanRMSListenId = this.m_meanRMSCheck.addListener( "changeValue", this._sendShowMeanRMSCmd, this );
            displayContainer.add( meanLabel, {row:3, column:0} );
            displayContainer.add( this.m_meanRMSCheck, {row:3, column:1} );
            
            this.m_labelCheck = new qx.ui.form.CheckBox();
            var labLabel = new qx.ui.basic.Label( "Peak Labels:");
            this.m_labelListenId = this.m_labelCheck.addListener( "changeValue", this._sendShowPeakLabelsCmd, this );
            displayContainer.add( labLabel, {row:4, column:0} );
            displayContainer.add( this.m_labelCheck, {row:4, column:1} );
        },
        
        
        /**
         * Update the UI based on server side values.
         * @param prefs {Object} - server-side fit settings.
         */
        prefUpdate : function( prefs ){
            if ( this.m_residualListenId != null ){
                this.m_residualCheck.removeListenerById( this.m_residualListenId );
            }
            this.m_residualCheck.setValue( prefs.showResiduals );
            this.m_residualListenId = this.m_residualCheck.addListener( "changeValue", 
                    this._sendResidualCmd, this );
            
            if ( this.m_guessListenId != null ){
                this.m_guessCheck.removeListenerById( this.m_guessListenId );
            }
            this.m_guessCheck.setValue( prefs.showGuesses );
            this.m_guessCheck.setEnabled( prefs.manualGuess );
            this.m_guessListenId = this.m_guessCheck.addListener( "changeValue", 
                    this._sendShowGuessesCmd, this );
            
            if ( this.m_statisticsListenId != null ){
                this.m_statisticsCheck.removeListenerById( this.m_statisticsListenId );
            }
            this.m_statisticsCheck.setValue( prefs.showStats );
            this.m_statisticsListenId = this.m_statisticsCheck.addListener( "changeValue", 
                    this._sendShowStatisticsCmd, this );
            
            if ( this.m_meanRMSListenId != null ){
                this.m_meanRMSCheck.removeListenerById( this.m_meanRMSListenId );
            }
            this.m_meanRMSCheck.setValue( prefs.showMeanRMS );
            this.m_meanRMSListenId = this.m_meanRMSCheck.addListener( "changeValue", 
                    this._sendShowMeanRMSCmd, this );
            
            if ( this.m_labelListenId != null ){
                this.m_labelCheck.removeListenerById( this.m_labelListenId );
            }
            this.m_labelCheck.setValue( prefs.showPeakLabels );
            this.m_labelListenId = this.m_labelCheck.addListener( "changeValue", 
                    this._sendShowPeakLabelsCmd, this );
        },
        
       
       /**
        * Send a command to the server to hide/show manual initial
        * guesses.
        */
        _sendShowGuessesCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var show = this.m_guessCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowGuesses";
                var params = "showGuesses:"+show;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server to hide/show the mean and
         * RMS.
         */
        _sendShowMeanRMSCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var show = this.m_meanRMSCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowMeanRMS";
                var params = "showMeanRMS:"+show;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server to hide/show fit informational
         * messages at the peaks of Gaussian fits.
         */
        _sendShowPeakLabelsCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var show = this.m_labelCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowPeakLabels";
                var params = "showPeakLabels:"+show;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server to hide/show the fit residuals.
         */
        _sendResidualCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var residuals = this.m_residualCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowResiduals";
                var params = "showResiduals:"+residuals;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server to show/hide fit statistics.
         */
        _sendShowStatisticsCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var show = this.m_statisticsCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowStatistics";
                var params = "showStats:"+show;
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
           
        },
        
        m_connector : null,
        m_id : null,
       
        m_guessCheck : null,
        m_guessListenId : null,
        m_meanRMSCheck : null,
        m_meanRMSListenId : null,
        m_labelCheck : null,
        m_labelListenId : null,
        m_residualCheck : null,
        m_residualListenId : null,
        m_statisticsCheck : null,
        m_statisticsListenId : null
      
    },
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});