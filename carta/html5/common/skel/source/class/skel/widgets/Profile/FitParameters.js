/**
 * Displays controls for fitting profile curves.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.FitParameters", {
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
            
            var paramContainer = new qx.ui.groupbox.GroupBox( "Parameters", "");
            var gridLayout =  new qx.ui.layout.Grid();
            gridLayout.setColumnAlign(0, "right","middle");
            gridLayout.setColumnAlign(1, "center", "middle");
            paramContainer.setLayout(gridLayout);
            paramContainer.setContentPadding(1,1,1,1);
            this._add( paramContainer );
           
            var gaussLabel = new qx.ui.basic.Label( "Gauss Count:");
            this.m_gaussCountSpin = new qx.ui.form.Spinner();
            this.m_gaussCountSpin.setToolTipText( "Set the number of Gaussians to fit.");
            this.m_gaussCountListenId = this.m_gaussCountSpin.addListener( "changeValue", 
                    this._sendGaussCountCmd, this );
           
            paramContainer.add( gaussLabel, {row:0,column:0} );
            paramContainer.add( this.m_gaussCountSpin, {row:0, column:1} );
            
            var polyLabel = new qx.ui.basic.Label( "Polynomial Terms:");
            this.m_polyCountSpin = new qx.ui.form.Spinner();
            this.m_polyCountSpin.setToolTipText( "Set the degree of the polynomial to fit.");
            this.m_polyCountListenId = this.m_polyCountSpin.addListener( "changeValue", 
                    this._sendPolyDegreeCmd, this );
            paramContainer.add( polyLabel, {row:1,column:0} );
            paramContainer.add( this.m_polyCountSpin, {row:1,column:1} );
            
            this.m_heuristicCheck = new qx.ui.form.CheckBox();
            var heurLabel = new qx.ui.basic.Label( "Random Heuristics:");
            this.m_heuristicListenId = this.m_heuristicCheck.addListener( "changeValue",
                    this._sendHeuristicCmd, this );
            this.m_heuristicCheck.setToolTipText( "Use random heuristics.")
            paramContainer.add( heurLabel, {row:2, column:0} );
            paramContainer.add( this.m_heuristicCheck, {row:2, column:1} );
            
            this.m_manualGuessCheck = new qx.ui.form.CheckBox();
            var guessLabel = new qx.ui.basic.Label( "Manual Initial Guess:");
            this.m_manualGuessListenId = this.m_manualGuessCheck.addListener( "changeValue",
                    this._sendManualGuessCmd, this );
            this.m_manualGuessCheck.setToolTipText( "Specify initial fit guesses manually.");
            paramContainer.add( guessLabel, {row:3, column:0} );
            paramContainer.add( this.m_manualGuessCheck, {row:3, column:1} );
            
            this.m_resetGuessButton = new qx.ui.form.Button("Reset Initial Guesses");
            this.m_resetGuessButton.setToolTipText( "Reset initial fit guesses.");
            this.m_resetGuessButton.addListener( "execute", this._sendResetGuessCmd, this );
            var guessContainer = new qx.ui.container.Composite();
            guessContainer.setLayout( new qx.ui.layout.HBox(1));
            guessContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            guessContainer.add( this.m_resetGuessButton );
            guessContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            paramContainer.add( guessContainer, {row:4, column:0, colSpan:2});
            
            this.m_clearButton = new qx.ui.form.Button( "Clear");
            this.m_clearButton.setToolTipText( "Remove fit curves from plot.");
            this.m_clearButton.addListener( "execute", this._sendClearFitCmd, this );
            var clearContainer = new qx.ui.container.Composite();
            clearContainer.setLayout( new qx.ui.layout.HBox(1));
            clearContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            clearContainer.add( this.m_clearButton );
            clearContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            paramContainer.add( clearContainer, {row:5, column:0, colSpan:2});
        },
        
        
        /**
         * Update the UI based on server side values.
         * @param prefs {Object} - server-side fit settings.
         */
        fitUpdate : function( prefs ){
            if ( typeof prefs.gaussCount != 'undefined'){
                if ( this.m_gaussCountListenId != null ){
                    this.m_gaussCountSpin.removeListenerById( this.m_gaussCountListenId );
                }
                this.m_gaussCountSpin.setValue( prefs.gaussCount );
                this.m_gaussCountListenId = this.m_gaussCountSpin.addListener( "changeValue", 
                    this._sendGaussCountCmd, this );
            }
            if ( typeof prefs.polyDegree != 'undefined'){
                if ( this.m_polyCountListenId != null ){
                    this.m_polyCountSpin.removeListenerById( this.m_polyCountListenId );
                }
                this.m_polyCountSpin.setValue( prefs.polyDegree );
                this.m_polyCountListenId = this.m_polyCountSpin.addListener( "changeValue", 
                    this._sendPolyDegreeCmd, this );
            }
            if ( typeof prefs.heuristics != 'undefined'){
                if ( this.m_heuristicListenId != null ){
                    this.m_heuristicCheck.removeListenerById( this.m_heuristicListenId );
                }
                this.m_heuristicCheck.setValue( prefs.heuristics );
                this.m_heuristicListenId = this.m_heuristicCheck.addListener( "changeValue", 
                    this._sendHeuristicCmd, this );
            }
            if ( typeof prefs.manualGuess != 'undefined'){
                if ( this.m_manualGuessListenId != null ){
                    this.m_manualGuessCheck.removeListenerById( this.m_manualGuessListenId );
                }
                this.m_manualGuessCheck.setValue( prefs.manualGuess );
                this.m_manualGuessId = this.m_manualGuessCheck.addListener( "changeValue", 
                    this._sendManualGuessCmd, this );
            }
        },
        
        /**
         * Callback from the server for a change in fit parameters.
         */
        _profileFitCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var profileFit = JSON.parse( val );
                    this.fitUpdate( profileFit.fit );
                }
                catch( err ){
                    console.log( "Could not parse fit parameters: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Register to get updates from the server.
         */
        _register : function(){
            var path = skel.widgets.Path.getInstance();
            var fitPath = this.m_id + path.SEP + path.FIT;
            this.m_sharedVar = this.m_connector.getSharedVar( fitPath );
            this.m_sharedVar.addCB( this._profileFitCB.bind( this));
            this._profileFitCB();
          
        },
       
        /**
         * Send a command to the server indicating all fits should be cleared.
         */
         _sendClearFitCmd : function(){
             if ( this.m_id !== null && this.m_connector !== null ){
                 var path = skel.widgets.Path.getInstance();
                 var cmd = this.m_id + path.SEP_COMMAND + "clearFits";
                 var params = "";
                 this.m_connector.sendCommand( cmd, params, null );
             }
         },
         
        
       /**
        * Send a command to the server indicating the number of Gaussians to fit.
        */
        _sendGaussCountCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var gaussCount = this.m_gaussCountSpin.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setGaussCount";
                var params = "gaussCount:"+gaussCount;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server indicating whether or not to use random
         * heuristics to fit the data.
         */
         _sendHeuristicCmd : function(){
             if ( this.m_id !== null && this.m_connector !== null ){
                 var heuristic = this.m_heuristicCheck.getValue();
                 var path = skel.widgets.Path.getInstance();
                 var cmd = this.m_id + path.SEP_COMMAND + "setRandomHeuristics";
                 var params = "heuristics:"+heuristic;
                 this.m_connector.sendCommand( cmd, params, null );
             }
         },
         
         /**
          * Send a command to the server indicating whether manual initial
          * guesses will be made for the fit.
          */
         _sendManualGuessCmd : function(){
             if ( this.m_id !== null && this.m_connector !== null ){
                 var manGuess = this.m_manualGuessCheck.getValue();
                 var data = {
                         manual : manGuess
                 };
                 var path = skel.widgets.Path.getInstance();
                 var cmd = this.m_id + path.SEP_COMMAND + "setManualGuess";
                 var params = "manualGuess:"+manGuess;
                 this.m_connector.sendCommand( cmd, params, null );
             }
         },
        
        /**
         * Send a command to the server indicating the degree of the polynomial to fit.
         */
        _sendPolyDegreeCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var polyDegree = this.m_polyCountSpin.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setPolyDegree";
                var params = "polyDegree:"+polyDegree;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server to reset the initial Gaussian guesses.
         */
        _sendResetGuessCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "resetInitialFitGuesses";
                var params = "";
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
            this._register();
        },
        
        m_connector : null,
        m_id : null,
        m_sharedVar : null,
        m_clearButton : null,
        m_gaussCountListenId : null,
        m_gaussCountSpin : null,
        m_heuristicCheck : null,
        m_heuristicListenId : null,
        m_manualGuessCheck : null,
        m_manualGuessListenId : null,
        m_polyCountListenId : null,
        m_polyCountSpin : null,
        m_resetGuessButton : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});