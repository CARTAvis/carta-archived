/**
 * Displays controls for customizing the image stack.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Stack.StackControls", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments, "Stack", "");
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {

        /**
         * Callback for a change in stack settings.
         */
        _controlsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    this.m_imageList.setListItems( controls.data );
                   
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();
                }
                catch( err ){
                    console.log( "Stack controls could not parse: "+val+" error: "+err );
                }
            }
        },

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout( new qx.ui.layout.HBox(1));
            this.m_imageList = new skel.widgets.Image.Stack.DragDropList( 300 );
            this.m_imageList.addListener( "listReordered", this._sendReorderCmd, this );
            this._add( this.m_imageList );
        },

        /**
         * Register to get updates on stack settings from the server.
         */
        _registerControls : function(){
            var path = skel.widgets.Path.getInstance();
            var dataPath = this.m_id + path.SEP + "data";
            this.m_sharedVar = this.m_connector.getSharedVar( dataPath );
            this.m_sharedVar.addCB(this._controlsChangedCB.bind(this));
            this._controlsChangedCB();
        },
        
        /**
         * Send a command to the server to reorder the images in the stack.
         * @param msg {Array} - a list specifying the new image order.
         */
        _sendReorderCmd : function( msg ){
            var imageList = msg.getData().listItems;
            var params = "images:"+imageList.join(";");
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setImageOrder";
            this.m_connector.sendCommand( cmd, params, function(){});
        },

        /**
         * Send a command to the server to get the stack control id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
            this.m_id = imageId;
            this._registerControls();
        },

        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        m_imageList : null
    }
});