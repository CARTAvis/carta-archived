/**
 * Displays application and window specific menu items as well as a customizable
 * tool bar.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Menu.MenuBar", {
    extend : qx.ui.toolbar.ToolBar,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this.m_connector = mImport("connector");

        //this.m_menuPart = new skel.widgets.Menu.MenuBarPart();
        this.m_menuPart = new qx.ui.toolbar.Part();
        this.add(this.m_menuPart);
        this._initMenu();
        this.addSpacer();
        
        this._initSubscriptions();
        this._initContextMenu();
    },

    members : {

        /**
         * Adds menu buttons to the application menu based on
         * the window that is selected.
         */
        addWindowMenu : function(ev) {
            var i = 0;
            if (this.m_windowButtons ) {
                for ( i = 0; i < this.m_windowButtons.length; i++) {
                    this.m_menuPart
                            .remove(this.m_windowButtons[i]);
                }
            }
            this.m_windowButtons = ev.getData();
            for ( i = 0; i < this.m_windowButtons.length; i++) {
                this.m_menuPart.add(this.m_windowButtons[i]);
            }
        },
        
        /**
         * Dynamically build the menu based on available commands.
         * @param cmds {Array} a list of available commands.
         */
        _buildMenu : function ( cmds ){
            if ( cmds.isVisibleMenu() ){
                var vals = cmds.getValue();
                var emptyFunc = function(){};
                for ( var i = 0; i < vals.length; i++ ){
                    //Only add top-level commands to the menu that are supported by 
                    //the selected window(s).
                    var enabled = vals[i].isEnabled();
                    var menuVisible = vals[i].isVisibleMenu();
                    if ( enabled && menuVisible ){
                        var cmdType = vals[i].getType();
                        if ( cmdType === skel.Command.Command.TYPE_COMPOSITE  || 
                            cmdType === skel.Command.Command.TYPE_GROUP ){
                            
                            var menu = skel.widgets.Util.makeMenu( vals[i]);
                            var menuButton = new qx.ui.toolbar.MenuButton( vals[i].getLabel() );
                            this.m_menuPart.add( menuButton );
                            menuButton.setMenu( menu);
                        
                        }
                        else if ( cmdType === skel.Command.Command.TYPE_BUTTON ){
                            var button = skel.widgets.Util.makeButton( vals[i], emptyFunc, true, true );
                            this.m_menuPart.add( button );
                        }
                        else if ( cmdType === skel.Command.Command.TYPE_BOOL ){
                            var check = skel.widgets.Util.makeCheck( vals[i], emptyFunc, true );
                            this.m_menuPart.add( check );
                        }
                        else {
                            console.log( "Menu unsupported top level command type="+ cmdType );
                        }
                    }
                }
            }
        },
        
        /**
         * Initialize a context menu.
         */
        _initContextMenu : function() {
            this.m_contextMenu = new qx.ui.menu.Menu();
            var customizeButton = new qx.ui.menu.Button("Customize...");
            var showDialog = skel.Command.Customize.CommandShowCustomizeDialog.getInstance();
            customizeButton.addListener("execute", function() {
                showDialog.doAction( true, null );
            }, this);
            var removeButton = new qx.ui.menu.Button( "Hide Menu Bar");
            removeButton.addListener( "execute", function(){
                var toolVisibleCmd = skel.Command.Preferences.Show.CommandShowMenu.getInstance();
                toolVisibleCmd.doAction( false, null);
            }, this );
            this.m_contextMenu.add(customizeButton);
            this.m_contextMenu.add( removeButton );
            this.setContextMenu(this.m_contextMenu);
        },
        
        /**
         * Initializes the main menu.
         */
        _initMenu : function() {
            var cmds = skel.Command.CommandAll.getInstance();
            this._resetMenu( cmds );
        },
        
        /**
         * Initialize message subscriptions.
         */
        _initSubscriptions : function(){
            qx.event.message.Bus.subscribe( "commandsChanged", function( message ){
                this._initMenu();
             }, this );
           
            qx.event.message.Bus.subscribe( "commandVisibilityMenuChanged", function( message){
                this._initMenu();
            }, this );
        },

        
        /**
         * Reset the menu after erasing the old one.
         * @param cmds {Array} a list of available commands.
         */
        _resetMenu: function( cmds ){
            this.m_menuPart.removeAll();
            this._buildMenu( cmds );
        },


 
        /*
         * Hides or shows the status bar based on the location of the mouse.
         * Mouse close to screen top = show; Otherwise, hide.
         */
        showHideMenu : function(ev) {
            /*var widgetLoc = skel.widgets.Util.getTop(this);
            var mouseLoc = ev.getDocumentTop();
            if (!this.isTop()) {
                widgetLoc = skel.widgets.Util.getLeft(this);
                mouseLoc = ev.getDocumentLeft();
            }
            this.showHide(this, mouseLoc, widgetLoc);*/
        },
        

        m_contextMenu : null,
        m_menuPart : null
    }

});
