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
        this._initSubscriptions();
    },

    events : {
        "layoutRowCount" : "qx.event.type.Data",
        "layoutColCount" : "qx.event.type.Data",
        "menuAlwaysVisible" : "qx.event.type.Data",
        "menuMoved" : "qx.event.type.Data",
        "newWindow" : "qx.event.type.Data",
        "shareSession" : "qx.event.type.Data",
        "statusAlwaysVisible" : "qx.event.type.Data",
        /// emitted when cursor window toggle is clicked
        "showCursorWindow" : "qx.event.type.Data"
    },

    members : {

        _buildToolBar : function ( cmds ){
            var i = 0;
            if( Object.prototype.toString.call( cmds.value ) === '[object Array]') {
                for ( i = 0; i < cmds.value.length; i++ ){
                    this._buildToolBar( cmds.value[i]);
                }
            }
            else if ( cmds.getType() === skel.widgets.Command.Command.TYPE_COMPOSITE ){
                var vals = cmds.getValue();
                for ( i = 0; i < vals.length; i++ ){
                    this._buildToolBar( vals[i]);
                }
            }
            else {
                this._makeTool( cmds );
            }
        },
        
        _invokeCmd : function( lookup, val, undo ){
            var cmdFactory = skel.widgets.Command.CommandFactory.getInstance();
            var cmd = cmdFactory.getCommand( lookup);
            cmd.doAction( val, this.m_activeWindowIds, undo );
        },
        
        _makeRadioGroup : function( cmd ){
            var radioGroup = new qx.ui.form.RadioGroup();
            radioGroup.setAllowEmptySelection(true);
            var values = cmd.getValue();
            var labelFunction = function( anObject, button){
                return function(){
                    anObject._invokeCmd( button.getLabel(), "", null );
                };
            };
            
            for ( var i = 0; i < values.length; i++ ){
                var label = values[i].getLabel();
                var button = new qx.ui.toolbar.RadioButton(label).set({
                    toolTipText: values[i].getToolTip()
                });
                button.addListener("execute", labelFunction(this, button), button );
                button.addListener("mouseup", labelFunction( this, button ), button );
                radioGroup.add(button);
                button.setFocusable(false);
                this.add(button);
            }
        },
        
        _makeToggle : function( cmd ){
            var label = cmd.getLabel();
            
            //Send values to the server
            var toggle = new skel.boundWidgets.Toggle( label, "");
            toggle.addListener( "toggleChanged", function( val ){
                this._invokeCmd( label, val.getData(), null );
            }, this);
            
            //Updates from the server
            cmd.addListener( "cmdValueChanged", function( evt ){
                var data = evt.getData();
                if ( this.getValue() != data.value ){
                    this.setValue( data.value );
                }
            }, toggle);
            this.add( toggle );
        },
        
        _makeTool : function( cmd ){
            
            if ( cmd.isVisibleToolbar()){
                var cmdType = cmd.getType();
                if ( cmdType === skel.widgets.Command.Command.TYPE_GROUP){
                    this._makeRadioGroup( cmd );
                }
                else if ( cmdType === skel.widgets.Command.Command.TYPE_BOOL){
                    this._makeToggle( cmd );
                }
                else if ( cmdType == "number"){
                    console.log( "Number not implemented");
                }
                else {
                    console.log( "Toolbar unrecognized type "+cmdType );
                }
            }
        },
        
        _resetButtons : function( cmds ){
            this.removeAll();
            this._buildToolBar( cmds );
            this.addSpacer();
        },

        /**
         * Initialize a tool bar containing 'quick' access buttons.
         */
        _init : function() {
            var cmdFactory = skel.widgets.Command.CommandFactory.getInstance();
            var cmds = cmdFactory.getCommandTree();
            this._resetButtons( cmds );
        },
        

        
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
        
        _initSubscriptions : function(){
            qx.event.message.Bus.subscribe( "commandsChanged", function( message ){
                this._init();
                    }, this );
            this.m_activeWindowIds = [];
            qx.event.message.Bus.subscribe(
                    "windowSelected",
                    function(message) {
                        var selectedWindow = message.getData();
                        var winId = selectedWindow.getIdentifier();
                        if ( this.m_activeWindowIds.indexOf( winId ) == -1){
                            this.m_activeWindowIds.push( winId );
                            
                        }
                    }, this);
            qx.event.message.Bus.subscribe(
                    "windowUnselected",
                    function(message) {
                        var unselectedWindow = message.getData();
                        var winId = unselectedWindow.getIdentifier();
                        var windowIndex = this.m_activeWindowIds.indexOf( winId );
                        if ( windowIndex >= 0){
                            this.m_activeWindowIds.splice( windowIndex, 1 );
                           
                        }
                    }, this);
        },
        
        m_activeWindowIds : null
    }

});
