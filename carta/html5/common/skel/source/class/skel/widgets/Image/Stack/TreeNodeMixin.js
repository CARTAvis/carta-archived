/**
 * Common properties for a tree node in the stack.
 */
qx.Mixin.define("skel.widgets.Image.Stack.TreeNodeMixin", {


    members : {
        /**
         * Return the identifier for the node.
         * @return {String} - an identifier for the node.
         */
        getId : function(){
            return this.m_id;
        },
        
        /**
         * Return the node label.
         * @return {String} - the node label.
         */
        getNodeLabel : function(){
            return this.m_label;
        },
        
        /**
         * Return node specific settings.
         * @return {Object} - node specific settings.
         */
        getSettings : function(){
            return this.m_settings;
        },
        
        /**
         * Initialize the context menu.
         */
        _initContextMenu : function(){
            console.log("grimmer menu treenodemixin")
            var contextMenu = new qx.ui.menu.Menu();
            var path = skel.widgets.Path.getInstance();
            var closeCmd = new skel.Command.Data.CommandDataClose( this.getLabel(), 
                    path.IMAGE_DATA, path.CLOSE_IMAGE, this.m_id );
            var closeButton = new qx.ui.menu.Button( "Close");
            closeButton.addListener( "execute", function(){
                this.doAction( true, function(){} );
            }, closeCmd );
            contextMenu.add( closeButton );
            
            if ( this.m_visible ){
                //Hide button
                var hideCmd = new skel.Command.Data.CommandDataHideImage( this.getLabel(), this.m_id );
                var hideButton = new qx.ui.menu.Button( "Hide");
                hideButton.addListener( "execute", function(){
                    this.doAction( true, function(){});
                }, hideCmd );
                contextMenu.add( hideButton );
            }
            else {
                //Show button
                var showCmd = new skel.Command.Data.CommandDataShowImage( this.getLabel(), this.m_id );
                var showButton = new qx.ui.menu.Button( "Show");
                showButton.addListener( "execute", function(){
                    this.doAction( true, function(){});
                }, showCmd );
                contextMenu.add( showButton );
            }
            this.setContextMenu(contextMenu);
        },
        
        /**
         * Set node specific settings.
         * @param settings {Object} - node specific settings.
         */
        setSettings : function( settings ){
            if ( this.m_settings != settings ){
                this.m_settings = settings;
                if ( this.m_customIcon != null ){
                    this.m_customIcon.setColor( settings.red, settings.green, settings.blue, settings.colorSupport );
                }
            }
        },
        
        /**
         * Set whether or not the node represents a visible image.
         * @param visible {boolean} - true if the image is visible; false otherwise.
         */
        setVisible : function( visible ){
            if ( this.m_visible != visible ){
                this.m_visible = visible;
                this._initContextMenu();
            }
        },
    
        m_customIcon : null,
        m_id : null,
        m_label : null,
        m_settings : null,
        m_visible : false

    }
});