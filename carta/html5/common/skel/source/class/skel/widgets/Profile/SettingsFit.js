/**
 * Displays controls for fitting profile curves.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsFit", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Fit", "");
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
            this._setLayout(new qx.ui.layout.VBox(2));
           
            var gaussContainer = new qx.ui.container.Composite();
            gaussContainer.setLayout( new qx.ui.layout.HBox(2));
            var gaussLabel = new qx.ui.basic.Label( "Gauss Count:");
            this.m_gaussCountSpin = new qx.ui.form.Spinner();
            this.m_gaussCountListenId = this.m_gaussCountSpin.addListener( "changeValue", 
                    this._sendGaussCountCmd, this );
            gaussContainer.add( gaussLabel );
            gaussContainer.add( this.m_gaussCountSpin );
            this.add( gaussContainer );
            
            var polyContainer = new qx.ui.container.Composite();
            polyContainer.setLayout( new qx.ui.layout.HBox(2));
            var polyLabel = new qx.ui.basic.Label( "Polynomial Count:");
            this.m_polyCountSpin = new qx.ui.form.Spinner();
            this.m_polyCountListenId = this.m_polyCountSpin.addListener( "changeValue", 
                    this._sendPolyCountCmd, this );
            polyContainer.add( polyLabel );
            polyContainer.add( this.m_polyCountSpin );
            this.add( polyContainer );
        },
        
        /**
         * Update the UI based on server side values.
         * @param prefs {Object} - server-side fit settings.
         */
        prefUpdate : function( prefs ){
            if ( this.m_gaussCountListenId != null ){
                this.m_gaussCountSpin.removeListenerById( this.m_gaussCountListenId );
            }
            this.m_gaussCountSpin.setValue( prefs.gaussCount );
            this.m_gaussCountListenId = this.m_gaussCountSpin.addListener( "changeValue", 
                    this._sendGaussCountCmd, this );
            if ( this.m_polyCountListenId != null ){
                this.m_polyCountSpin.removeListenerById( this.m_polyCountListenId );
            }
            this.m_polyCountSpin.setValue( prefs.polyCount );
            this.m_polyCountListenId = this.m_polyCountSpin.addListener( "changeValue", 
                    this._sendPolyCountCmd, this );
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
         * Send a command to the server indicating the degree of the polynomial to fit.
         */
        _sendPolyCountCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var polyCount = this.m_polyCountSpin.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setPolyCount";
                var params = "polyCount:"+polyCount;
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
        m_gaussCountListenId : null,
        m_gaussCountSpin : null,
        m_polyCountListenId : null,
        m_polyCountSpin : null,
        m_id : null
      
    }
});