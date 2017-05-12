/**
 * Displays application and window specific menu items as well as a customizable
 * tool bar.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Menu.ToolBar", {
    extend : qx.ui.toolbar.ToolBar,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this._init();
        this._initContextMenu();
        this._initSubscriptions();
    },

    members : {



        /**
         * Dynamically build the toolbar based on the list of commands.
         * @param cmds {Array} the list of available commands.
         */
        _buildToolBar : function ( cmds ){
            if ( cmds.isVisibleToolbar() ){
                if ( cmds.getType() === skel.Command.Command.TYPE_COMPOSITE ){
                    var vals = cmds.getValue();
                    for ( var i = 0; i < vals.length; i++ ){
                        this._buildToolBar( vals[i]);
                    }
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_GROUP ){
                    this._makeTool( cmds );
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_BOOL ){
                    var check = skel.widgets.Util.makeCheck( cmds, null, true );
                    this.add( check );
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_BUTTON ){
                    var button = skel.widgets.Util.makeButton( cmds, null, true, true );
                    this.add( button );
                }
                else {
                    console.log( "Tool bar unsupported command type="+ cmds.getType() );
                }
            }
        },

        /**
         * Initialize a tool bar containing 'quick' access buttons.
         */
        _init : function() {
            var cmds = skel.Command.CommandAll.getInstance();
            this._resetToolbar( cmds );
        },

        /**
         * Initialize a context menu.
         */
        _initContextMenu : function() {
            var contextMenu = new qx.ui.menu.Menu();
            var customizeButton = new qx.ui.menu.Button("Customize...");
            var showDialog = skel.Command.Customize.CommandShowCustomizeDialog.getInstance();
            customizeButton.addListener("execute", function() {
                showDialog.doAction( false, null );
            }, this);
            var removeButton = new qx.ui.menu.Button( "Hide Tool Bar");
            removeButton.addListener( "execute", function(){
                var toolVisibleCmd = skel.Command.Preferences.Show.CommandShowToolBar.getInstance();
                toolVisibleCmd.doAction( false, null);
            }, this );
            contextMenu.add(customizeButton);
            contextMenu.add( removeButton );
            this.setContextMenu(contextMenu);
        },

        /**
         * Initialize the list of messages to listen to.
         */
        _initSubscriptions : function(){
            //Available commands have changed (such as new clip values);
            qx.event.message.Bus.subscribe( "commandsChanged", function( message ){
                this._init();
            }, this );
            qx.event.message.Bus.subscribe( "commandVisibilityToolChanged", function( message){
                this._init();
            }, this );

        },

        /**
         * Tell a command to perform its action.
         * @param lookup {String} an identifier for a command.
         * @param val {Object} the new command value.
         * @param undo {Function} an undo callback if the action of the command does not succeed.
         */
        _invokeCmd : function( lookup, val, undo ){
            var cmdFactory = skel.Command.CommandFactory.getInstance();
            var cmd = cmdFactory.getCommand( lookup);
            cmd.doAction( val, undo );
        },



        /**
         * Make a widget for the tool bar out of the command.
         * @param cmd {skel.Command.Command}.
         */
        _makeTool : function( cmd ){

            if ( cmd.isVisibleToolbar()){
                var cmdType = cmd.getType();
                if ( cmdType === skel.Command.Command.TYPE_GROUP){

                    if(cmd.getLabel()=="Clips") {
                        var fileButton = new qx.ui.toolbar.RadioButton("Clipping:");
                        fileButton.setEnabled(false);
                        this.add(fileButton);
                    } else if (cmd.getLabel()=="Draw") {
                        var fileButton = new qx.ui.toolbar.RadioButton("Region:");
                        fileButton.setEnabled(false);
                        this.add(fileButton);
                    }

                    var radios = skel.widgets.Util.makeRadioGroup( cmd, true );
                    for ( var i = 0; i < radios.length; i++ ){
                        this.add( radios[i] );
                    }
                }
                else if ( cmdType === skel.Command.Command.TYPE_BOOL){
                    var check = skel.widgets.Util.makeCheck( cmd, null, true );
                    this.add( check );
                }
                else if ( cmdType == skel.Command.Command.TYPE_BUTTON ){
                    var button = skel.widgets.Util.makeButton( cmd, null, true, true );
                    this.add( button );
                }
                else {
                    console.log( "Toolbar unrecognized type "+cmdType );
                }
            }
            else {
                console.log( "Cmd was not visible on toolbar "+cmd );
            }
        },


        /**
         * Erase the current tool bar and reset with the new list of commands.
         * @param cmds {Array} the new list of commands.
         */
        _resetToolbar : function( cmds ){
            this.removeAll();
            this._buildToolBar( cmds );
            this.addSpacer();
        }
    }



});
