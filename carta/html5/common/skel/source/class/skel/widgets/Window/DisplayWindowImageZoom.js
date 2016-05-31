/**
 * A display window specialized for viewing images.
 */

/*global mImport */
/**
 @ignore( mImport)
 ************************************************************************ */

qx.Class.define("skel.widgets.Window.DisplayWindowImageZoom", {
    extend : skel.widgets.Window.DisplayWindow,
    
    /**
     * Constructor.
     */
    construct : function(index, detached) {
        console.log( "Constructing image zoom");
        var path = skel.widgets.Path.getInstance();
        this.base(arguments, path.IMAGE_ZOOM, index, detached );
        this.m_links = [];
        this.m_viewContent = new qx.ui.container.Composite();
        this.m_viewContent.setLayout(new qx.ui.layout.Canvas());
        this.m_content.add( this.m_viewContent, {flex:1} );
        console.log( "Finished constructing image zoom");
    },

    members : {
        
        /**
         * Clean-up items; this window is going to disappear.
         */
        clean : function(){
            //Remove the view so we don't get spurious mouse events sent to a 
            //server-side object that no longer exists.
            if ( this.m_view !== null ){
                if ( this.m_viewContent.indexOf( this.m_view) >= 0 ){
                    this.m_viewContent.remove( this.m_view);
                   
                }
            }
        },
        
        _initSharedVar : function(){
            
        },


       
        
        /**
         * Initialize the list of window specific commands this window supports.
         */
        _initSupportedCommands : function(){
            this.m_supportedCmds = [];
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
         * Set the appearance of this window based on whether or not it is selected.
         * @param selected {boolean} true if the window is selected; false otherwise.
         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
         */
        setSelected : function(selected, multiple) {
            this._initSupportedCommands();
           
            arguments.callee.base.apply(this, arguments, selected, multiple );
        },
        
      
        
        /**
         * Implemented to initialize the context menu.
         */
        windowIdInitialized : function() {
            arguments.callee.base.apply(this, arguments);
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.PanZoomView(this.m_identifier);
            }
            
            if (this.m_viewContent.indexOf(this.m_view) < 0) {
                var overlayMap = {left:"0%",right:"0%",top:"0%",bottom: "0%"};
                this.m_viewContent.add(this.m_view, overlayMap );
                
            }
           
            this.m_view.setVisibility( "visible" );
        },
        
        /**
         * Update from the server.
         * @param winObj {Object} - an object containing server side information values.
         */
        windowSharedVarUpdate : function( winObj ){
           
        },
        
       
       
        m_view : null,
        m_viewContent : null
       
    }

});
