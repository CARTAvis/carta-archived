/**
 * Displays controls for customizing the contours.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Contour.ContourControls", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments, "Contour", "");
        this.m_connector = mImport("connector");
        this._init();
    },
    
    events : {
        "gridControlsChanged" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Callback for a change in contour preference settings.
         */
        _controlsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    if ( this.m_contourTabs !== null ){
                        this.m_contourTabs.setControls(controls);
                    }
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();
                }
                catch( err ){
                    console.log( "Contour controls could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Callback for a change in contour preference settings.
         */
        _controlsDataChangedCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    if ( this.m_contourTabs !== null ){
                        this.m_contourTabs.setControlsData(controls);
                    }
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();
                }
                catch( err ){
                    console.log( "Contour controls data could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout( new qx.ui.layout.VBox(1));
            this.m_contourTabs = new skel.widgets.Image.Contour.ContourTabs();
            this._add( this.m_contourTabs );
        },
       
        
        /**
         * Register to get updates on contour settings from the server.
         */
        _registerControls : function(){
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._controlsChangedCB.bind(this));
            this._controlsChangedCB();
            var path = skel.widgets.Path.getInstance();
            var dataId = this.m_id+path.SEP + "data";
            this.m_sharedVarData = this.m_connector.getSharedVar( dataId );
            this.m_sharedVarData.addCB( this._controlsDataChangedCB.bind(this ));
            this._controlsDataChangedCB();
        },
        
        /**
         * Callback for when the registration is complete and an id is available.
         * @param anObject {skel.widgets.Image.Contour.ContourControls}.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                anObject._setContourId( id );
            };
        },
        
        /**
         * Send a command to the server to get the contour control id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
            var path = skel.widgets.Path.getInstance();
            var cmd = imageId + path.SEP_COMMAND + "registerContourControls";
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._registrationCallback( this));
        },
        
        /**
         * Store the contour control id.
         * @param contourId {String} a server-side identifier for the contour controls.
         */
        _setContourId : function( contourId ){
            if ( contourId !== null && contourId.length > 0 ){
                this.m_id = contourId;
                this.m_contourTabs.setId( contourId );
                this._registerControls();
            }
        },
        
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        m_sharedVarData : null,
        
        m_contourTabs : null
    }
});