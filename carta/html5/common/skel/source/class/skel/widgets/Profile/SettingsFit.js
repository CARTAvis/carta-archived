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
         * Update the list of profile curves based on server information.
         * @param curveUpdate {Object} - information from the server about profile curves.
         */
        dataUpdate : function( curveUpdate ){
            this.m_curveInfo = curveUpdate.curves;
            var curveNames = [];
            var selectIndices = [];
            var curveIndex = 0;
            for ( var i = 0; i < this.m_curveInfo.length; i++ ){
                if ( ! this.m_curveInfo[i].pointSource ){
                    curveNames[curveIndex] = this.m_curveInfo[i].name;
                    if ( this.m_curveInfo[i].fitSelect ){
                        selectIndices.push( curveIndex );
                    }
                    curveIndex++;
                }
            }
            if ( this.m_curveListenId !== null ){
                this.m_curveList.removeListenerById( this.m_curveListenId );
            }
            this.m_curveList.setItems( curveNames );
            this.m_curveList.setSelected( selectIndices );
            this.m_curveListenId = this.m_curveList.addListener( "itemsSelected", 
                    this._sendFitsCmd, this );
        }, 
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.HBox(2));
            this._initProfiles();
            this._initParams();
            this._initDisplay();
        },
        
        /**
         * Initialize the parameters that show/hide various aspects of the
         * fit display.
         */
        _initDisplay : function(){
            this.m_fitDisplay = new skel.widgets.Profile.FitDisplay();
            this.add( this.m_fitDisplay );
        },
      
        /**
         * Initialize the panel that displays fit parameters.
         */
        _initParams : function(){
            this.m_fitParameters = new skel.widgets.Profile.FitParameters();
            this.add( this.m_fitParameters );
        },
        
        /**
         * Initialize the list of profiles that are available for fitting.
         */
        _initProfiles : function(){
            var TABLE_WIDTH = 150;
            this.m_curveList  = new skel.widgets.CustomUI.ItemTable( "Profiles", TABLE_WIDTH);
            this.m_curveList.setTestId( "profileTable");
            this.m_curveList.setToolTipText( "Select one or more profiles to fit." );
            this.m_curveList.setWidth( TABLE_WIDTH );
            this.m_curveListenId = this.m_curveList.addListener( "itemsSelected", 
                    this._sendFitsCmd, this );
            this.add( this.m_curveList );
        },
        
        /**
         * Update the UI based on server side values.
         * @param prefs {Object} - server-side fit settings.
         */
        prefUpdate : function( prefs ){
           this.m_fitDisplay.prefUpdate( prefs );
        },
        
        /**
         * Send a command to the server identifying the profiles that need
         * to be fit.
         */
        _sendFitsCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var fits = this.m_curveList.getSelected();
                var fitsList = fits.join( ";");
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setFitCurves";
                var params = "fitCurves:" + fitsList;
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
            this.m_fitParameters.setId( id );
            this.m_fitDisplay.setId( id );
        },
        
        /**
         * Set whether or not fit estimates will be specified manually.
         * @param manual {boolean} - true, if fit estimates should be specified
         *      manually; false, otherwise.
         */
        setManual : function( manual ){
            this.m_fitDisplay.setManual( manual );
        },
      
        
        m_connector : null,
        m_curveList : null,
        m_curveListenId : null,
        m_fitDisplay : null,
        m_fitParameters : null,
        m_id : null
      
    }
});