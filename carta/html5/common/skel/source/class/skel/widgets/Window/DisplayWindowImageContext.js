/**
 * A display window specialized for displaying a context view of an image.
 */

/*global mImport */
/**
 @ignore( mImport)
 ************************************************************************ */

qx.Class.define("skel.widgets.Window.DisplayWindowImageContext", {
    extend : skel.widgets.Window.DisplayWindow,
    include : skel.widgets.Window.PreferencesMixin,
    
    /**
     * Constructor.
     */
    construct : function(index, detached) {
        var path = skel.widgets.Path.getInstance();
        this.base(arguments, path.IMAGE_CONTEXT, index, detached );
        this.m_links = [];
        this.m_viewContent = new qx.ui.container.Composite();
        this.m_viewContent.setLayout(new qx.ui.layout.Canvas());
        this.m_contextControls = new skel.widgets.Image.Context.ContextControls();
        this.m_content.add( this.m_viewContent, {flex:1} );
    },

    members : {
        
        /**
         * Add or remove the image context settings based on whether the user
         * had configured any of the settings visible.
         * @param content {boolean} - true if the content should be visible; false otherwise.
         */
        _adjustControlVisibility : function(content){
            this.m_controlsVisible = content;
            this._layoutControls();
        },
        
        /**
         * Clean-up items; this window is going to disappear.
         */
        clean : function(){
            //Remove the view so we don't get spurious mouse events sent to a 
            //server object that no longer exists.
            if ( this.m_view !== null ){
                if ( this.m_viewContent.indexOf( this.m_view) >= 0 ){
                    this.m_viewContent.remove( this.m_view);
                   
                }
            }
        },
       
        
        /**
         * Initialize the list of window specific commands this window supports.
         */
        _initSupportedCommands : function(){
            this.m_supportedCmds = [];
            var settingsCmd = skel.Command.Settings.SettingsContext.getInstance();
            this.m_supportedCmds.push( settingsCmd.getLabel());
            var linksCmd = skel.Command.Link.CommandLink.getInstance();
            this.m_supportedCmds.push( linksCmd.getLabel() );
            arguments.callee.base.apply(this, arguments);
        },
        
        /**
         * Returns whether or not this window can be linked to a window
         * displaying a named plug-in.
         * @param pluginId {String} a name identifying a plug-in.
         */
        isLinkable : function(pluginId) {
            var linkable = false;
            var path = skel.widgets.Path.getInstance();
            if (pluginId == path.CASA_LOADER ) {
                linkable = true;
            }
            return linkable;
        },
        

        /**
         * Returns whether or not this window supports establishing a two-way
         * link with the given plug-in.
         * @param pluginId {String} the name of a plug-in.
         */
        isTwoWay : function(pluginId) {
            var biLink = false;
            if (pluginId == this.m_pluginId) {
                biLink = true;
            }
            return biLink;
        },
        
        /**
         * Layout the display.
         */
        _layoutControls : function(){
            this.m_content.removeAll();
            this.m_content.add( this.m_viewContent, {flex:1} );
            var overlayMap = {left:"0%",right:"0%",top:"0%",bottom: "0%"};
            if (this.m_viewContent.indexOf(this.m_view) < 0) {
                this.m_viewContent.add(this.m_view, overlayMap );
                this.m_view.setOverlayWidget( this.m_contextDraw );
            }
          
            if ( this.m_controlsVisible ){
                this.m_content.add( this.m_contextControls );
            }
        },
        
        /**
         * Callback for updating the visibility of the user settings from the server.
         */
        _preferencesCB : function(){
            if ( this.m_sharedVarPrefs !== null ){
                var val = this.m_sharedVarPrefs.get();
               
                if ( val !== null ){
                    try {
                        var setObj = JSON.parse( val );
                        this._adjustControlVisibility( setObj.settings );
                    }
                    catch( err ){
                        console.log( "ImageContextDisplay could not parse settings: "+val);
                        console.log( "err="+err);
                    }
                }
            }
        },
      
        
        /**
         * Set the appearance of this window based on whether or not it is selected.
         * @param selected {boolean} true if the window is selected; false otherwise.
         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
         */
        setSelected : function(selected, multiple) {
            this._initSupportedCommands();
            arguments.callee.base.apply(this, arguments, selected, multiple );
           
        },
        
        /**
         * Callback for when the data shared variable changes.
         */
        _sharedVarDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var contextRect = JSON.parse( val );
                    var corner0 = contextRect.corner0;
                    var corner1 = contextRect.corner1;
                    var selected = contextRect.boxSelected;
                    var width = contextRect.imageWidth;
                    var height = contextRect.imageHeight;
                    var rotate = contextRect.rotate;
                    if ( this.m_contextDraw !== null ){
                        this.m_contextDraw.setImageCorners( corner0.x, corner0.y, corner1.x, corner1.y, 
                                selected, width, height, rotate );
                    }
                }
                catch( err ){
                    console.log( "DisplayWindowContextImage could not parse: "+val );
                    console.log( "Error: "+err);
                }
            }
        },
       
        
        /**
         * Implemented to add the view.
         */
        windowIdInitialized : function() {
            arguments.callee.base.apply(this, arguments);
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.DragView(this.m_identifier);
            }
            if ( this.m_contextDraw == null ){
                this.m_contextDraw = new skel.widgets.Image.Context.ContextCanvas();
            }
            this._layoutControls();
            this.m_view.setVisibility( "visible" );
            
            this.initializePrefs();
            this.m_contextControls.setId( this.getIdentifier());
            this.registerDataUpdates();
        },
        
        /**
         * Register to receive updates when the data shared variable changes on
         * the server-side.
         */
        registerDataUpdates : function(){
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVarData = this.m_connector.getSharedVar( this.m_identifier+path.SEP +path.DATA );
            this.m_sharedVarData.addCB( this._sharedVarDataCB.bind( this ));
            this._sharedVarDataCB();
        },
        
        
        /**
         * Update from the server.
         * @param winObj {Object} - an object containing server side information values.
         */
        windowSharedVarUpdate : function( winObj ){
            this.m_contextControls.setControls( winObj );
            if ( this.m_contextDraw == null ){
                this.m_contextDraw = new skel.widgets.Image.Context.ContextCanvas();
            }
            this.m_contextDraw.setControls( winObj );
        },
       
        m_view : null,
        m_contextControls : null,
        m_controlsVisible : null,
        m_contextDraw : null,
        m_sharedVarData : null,
        m_viewContent : null
    }
});
