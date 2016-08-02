/**
 * Displays a profile and controls for customizing it.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.Profile", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },
    
    events : {
        "statVisibilityChanged" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout(new qx.ui.layout.Grow());
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content );
            this.m_content.setLayout(new qx.ui.layout.VBox());
            
            this._initMain();
        },
       
        /**
         * Stores the profile settings.
         * @param controls {skel.widgets.Profile.Settings} - the profile settings
         *      container.
         */
        setControls : function( controls){
            this.m_settingsContainer = controls;
        },
        
        
        /**
         * Initializes the menu for setting the visibility of individual profile
         * settings and the main graph.
         */
        _initMain : function(){
            this.m_mainComposite = new qx.ui.container.Composite();
            this.m_mainComposite.setLayout( new qx.ui.layout.VBox(2));
            this.m_mainComposite.set ({
                minWidth : this.m_MIN_DIM,
                minHeight : this.m_MIN_DIM,
                allowGrowX: true,
                allowGrowY: true
            });
            
            this.m_content.add( this.m_mainComposite, {flex:1});
        },
        
        
        /**
         * Initialize the profile view.
         */
        _initView : function(){
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.DragView(this.m_id);
                this.m_fitOverlay = new skel.widgets.Profile.FitOverlay();
                this.m_view.setOverlayWidget( this.m_fitOverlay );
                this.m_fitOverlay.setId( this.m_id );
                this.m_view.setAllowGrowX( true );
                this.m_view.setAllowGrowY( true );
                this.m_view.setMinHeight(this.m_MIN_DIM);
                this.m_view.setMinWidth(this.m_MIN_DIM);
                if ( this.m_mainComposite.indexOf( this.m_view) < 0 ){
                    this.m_mainComposite.add( this.m_view, {flex:1} );
                }
            }
        },
        
        
        /**
         * Add or remove the control settings.
         * @param visible {boolean} - true if the control settings should be visible;
         *      false otherwise.
         */
        _layoutControls : function( ){
            if(this.m_showSettings){
                //Make sure the settings container is visible.
                if ( this.m_content.indexOf( this.m_settingsContainer ) < 0 ){
                    this.m_content.add( this.m_settingsContainer );
                }
            }
            else {
                if ( this.m_content.indexOf( this.m_settingsContainer ) >= 0 ){
                    this.m_content.remove( this.m_settingsContainer );
                }
            }
        },
        
        /**
         * Callback for updates of server-side fit preferences.
         */
        _profileCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var profilePrefs = JSON.parse( val );
                    this.m_fitOverlay.setManualShow( profilePrefs.showGuesses );
                    this.m_settingsContainer.prefUpdate( profilePrefs );
                    var data = {
                        showStats : profilePrefs.showStats
                    }
                    this.fireDataEvent( "statVisibilityChanged", data );
                }
                catch( err ){
                    console.log( "Could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Callback for updates of server-side fit preferences.
         */
        _profileFitCB : function(){
            var val = this.m_sharedVarFit.get();
            if ( val ){
                try {
                    var fitInfo = JSON.parse( val );
                    this.m_fitOverlay.profileFitUpdate( fitInfo );
                    var manualMode = fitInfo.fit.manualGuess;
                    this.m_settingsContainer.setManualFitGuesses( manualMode );
                }
                catch( err ){
                    console.log( "Could not parse profile fit: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Register to receive updates to server-side profile
         * variables.
         */
        _register : function(){
            var path = skel.widgets.Path.getInstance();
            //Preferences update
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id );
            this.m_sharedVar.addCB( this._profileCB.bind( this));
            this._profileCB();
            //Fit update
            var fitPath = this.m_id + path.SEP + path.FIT;
            this.m_sharedVarFit = this.m_connector.getSharedVar( fitPath );
            this.m_sharedVarFit.addCB( this._profileFitCB.bind( this));
            this._profileFitCB();
        },
        
        
        /**
         * Set the server side id of this profiler.
         * @param controlId {String} the server side id of the object that produced this profile.
         */
        setId : function( controlId ){
            this.m_id = controlId;
            this._initView();
            this._register();
        },

        
        m_content : null,
        m_fitOverlay : null,
        m_mainComposite : null,
        m_settingsContainer : null,
        m_showSettings : null,
        m_MIN_DIM : 195,
        m_id : null,
        m_sharedVar : null,
        m_sharedVarFit : null,
        m_connector : null,
        
        m_view : null
    }


});