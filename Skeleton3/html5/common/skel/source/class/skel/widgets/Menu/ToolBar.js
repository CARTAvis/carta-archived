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
                    this._makeToggle( cmds );
                }
                else if ( cmds.getType() == skel.Command.Command.TYPE_BUTTON ){
                    this._makeButton( cmds );
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
            //Disabled for now
            this.m_contextMenu = new qx.ui.menu.Menu();
            var customizeButton = new qx.ui.menu.Button("Customize...");
            customizeButton.addListener("execute", function() {
                var data = {
                        menu: false
                };
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "showCustomizeDialog", data));
            }, this);
            this.m_contextMenu.add(customizeButton);
            this.setContextMenu(this.m_contextMenu);
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
         * Make a group of radio buttons out of the command's children.
         * @param cmd {skel.Command.CommandGroup} a group of commands, only one of which
         *      can be selected at a time.
         */
        _makeRadioGroup : function( cmd ){
            var radioGroup = new qx.ui.form.RadioGroup();
            radioGroup.setAllowEmptySelection(true);
            var values = cmd.getValue();
            var labelFunction = function( anObject, button){
                return function(){
                    anObject._invokeCmd( button.getLabel(), "", null );
                };
            };
            var enableFunction = function( cmd  ){
                return function(){
                    this.setEnabled( cmd.isEnabled());
                };
            };
            var enabled = cmd.isEnabled();
            for ( var i = 0; i < values.length; i++ ){
                if ( values[i].isVisibleToolbar() ){
                    var label = values[i].getLabel();
                    var button = new qx.ui.toolbar.RadioButton(label).set({
                        toolTipText: values[i].getToolTip()
                    });
                    button.addListener("execute", labelFunction(this, button), button );
                    button.addListener("mouseup", labelFunction( this, button ), button );
                    values[i].addListener( "cmdEnabledChanged", enableFunction( values[i]), button);
                    radioGroup.add(button);
                    button.setFocusable(false);
                    button.setEnabled( enabled );
                    this.add(button);
                }
            }
        },
        
        /**
         * Make a check box out of the command.
         * @param cmd {skel.Command.Command} a command that can either be true/false.
         */
        _makeToggle : function( cmd ){
            var toggle = skel.widgets.Util.makeToggle( cmd );
            this.add( toggle );
        },
        
        /**
         * Make a button that can be executed ou of the command.
         * @param cmd {skel.Command.Command} a command that can be executed.
         */
        _makeButton : function( cmd ){
            var label = cmd.getLabel();
            var button = new qx.ui.toolbar.Button( cmd.getLabel());
            //Send values to the server
            button.addListener( "clicked", function( ){
                this._invokeCmd( label, null, null );
            }, this);
            cmd.addListener( "cmdEnabledChanged", function( evt ){
                var data = evt.getData();
                this.setEnabled( data.enabled );
            }, button);
            var cmdEnabled = cmd.isEnabled();
            button.setEnabled( cmdEnabled );
            this.add( button );
        },
        
        /**
         * Make a widget for the tool bar out of the command.
         * @param cmd {skel.Command.Command}.
         */
        _makeTool : function( cmd ){
            
            if ( cmd.isVisibleToolbar()){
                var cmdType = cmd.getType();
                if ( cmdType === skel.Command.Command.TYPE_GROUP){
                    this._makeRadioGroup( cmd );
                }
                else if ( cmdType === skel.Command.Command.TYPE_BOOL){
                    this._makeToggle( cmd );
                }
                else if ( cmdType == skel.Command.Command.TYPE_BUTTON ){
                    this._makeButton( cmd );
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
