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
            // init時被called到, 還有內部recursive
            // 把qx.ui.menu.CheckBox加進去, this.add or
            // qx.ui.menu.Button or
            // 一個 array, of new qx.ui.menu.RadioButton or qx.ui.toolbar.RadioButton 中途它們可能被加進去 qx.ui.form.RadioGroup !!!
            console.log("grimmer toolbar-_buildToolBar", cmds);
            if ( cmds.isVisibleToolbar() ){
                if ( cmds.getType() === skel.Command.Command.TYPE_COMPOSITE ){
                    console.log("grimmer toolbar-composite");
                    var vals = cmds.getValue();
                    for ( var i = 0; i < vals.length; i++ ){
                        this._buildToolBar( vals[i]);
                    }
                    console.log("grimmer toolbar-composite2");
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_GROUP ){
                    console.log("grimmer toolbar-group");
                    this._makeTool( cmds ); //可能會用到 qx.ui.form.RadioGroup?
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_BOOL ){
                    // console.log("grimmer toolbar-bool");
                    var check = skel.widgets.Util.makeCheck( cmds, null, true );
                    this.add( check );
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_BUTTON ){
                    // console.log("grimmer toolbar-button");
                    var button = skel.widgets.Util.makeButton( cmds, null, true, true );
                    this.add( button );
                }
                else {
                    console.log( "Tool bar unsupported command type="+ cmds.getType() );
                }
            } else {
                console.log("grimmer toolbar-invisile");
            }
        },

        _buildToolBar2 : function ( cmds ){
            // init時被called到, 還有內部recursive
            // 把qx.ui.menu.CheckBox加進去, this.add or
            // qx.ui.menu.Button or
            // 一個 array, of new qx.ui.menu.RadioButton or qx.ui.toolbar.RadioButton 中途它們可能被加進去 qx.ui.form.RadioGroup !!!
            console.log("grimmer toolbar2-_buildToolBar", cmds);
            if ( cmds.isVisibleToolbar() ){
                if ( cmds.getType() === skel.Command.Command.TYPE_COMPOSITE ){
                    console.log("grimmer toolbar2-composite");
                    var vals = cmds.getValue();
                    for ( var i = 0; i < vals.length; i++ ){
                        this._buildToolBar2( vals[i]);
                    }
                    console.log("grimmer toolbar2-composite2");
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_GROUP ){
                    console.log("grimmer toolbar2-group");
                    this._makeTool( cmds ); //可能會用到 qx.ui.form.RadioGroup?
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_BOOL ){
                    // console.log("grimmer toolbar-bool");
                    var check = skel.widgets.Util.makeCheck( cmds, null, true );
                    this.add( check );
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_BUTTON ){
                    // console.log("grimmer toolbar-button");
                    var button = skel.widgets.Util.makeButton( cmds, null, true, true );
                    this.add( button );
                }
                else {
                    console.log( "Tool bar unsupported command type="+ cmds.getType() );
                }
            } else {
                console.log("grimmer toolbar2-invisile");
            }
        },

        /**
         * Initialize a tool bar containing 'quick' access buttons.
         */
        _init : function() {

            // CAS-9814 Retrieve initial layout type to enable check status of default layout.
            // DisplayMain.js
            //                     var layoutType = layout.layoutType;
                    // if(layoutType) {
                    //     skel.Command.Layout.CommandLayout.getInstance().setCheckedType(layoutType);
                    // }

            //skel.Command.Clip.CommandClipValues?
            // 0.99
            var cmds = skel.Command.CommandAll.getInstance();
            console.log("grimmer toolbar2-init:", cmds);

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

            //百分百的每一的都有完整的% array, m_cmds, 來了5,6個
            console.log("grimmer toolbar3-_makeTool:", cmd);
            console.log("grimmer toolbar3-2:", cmd.length);

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

                    console.log("grimmer toolbar-makeTool-group:", cmd.getLabel());
                    //CommandClipValues: "Clips"
                    // 只有 clip & region
                    //  :"Draw" or 各種region type?

                    var radios = skel.widgets.Util.makeRadioGroup( cmd, true );
                    for ( var i = 0; i < radios.length; i++ ){
                        this.add( radios[i] ); //!!!!!!!!!!!!
                    }

                    //var fileButton = new qx.ui.toolbar.MenuButton("File");

                    // var fileButton = new qx.ui.menu.RadioButton(); not work


                    console.log("grimmer toolbar-makeTool-group2:");
                }
                else if ( cmdType === skel.Command.Command.TYPE_BOOL){
                    // console.log("grimmer toolbar3-_bool:");

                    var check = skel.widgets.Util.makeCheck( cmd, null, true );
                    this.add( check );
                }
                else if ( cmdType == skel.Command.Command.TYPE_BUTTON ){
                    // console.log("grimmer toolbar3-_button:");

                    var button = skel.widgets.Util.makeButton( cmd, null, true, true );
                    this.add( button );
                }
                else {
                    console.log( "Toolbar unrecognized type "+cmdType );
                }
            }
            else {
                console.log( "grimmer toolbar Cmd was not visible on toolbar "+cmd );
            }
        },


        /**
         * Erase the current tool bar and reset with the new list of commands.
         * @param cmds {Array} the new list of commands.
         */
        _resetToolbar : function( cmds ){
            // console.log("grimmer toolbar4:", cmds.length);
            this.removeAll();
            this._buildToolBar( cmds );
            this.addSpacer();
        }
    }



});
