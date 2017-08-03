/**
 * Utility class for determining the location of widgets.
 */

/* global qx */

qx.Class.define("skel.widgets.Util", {
    type : "static",
    statics : {
        
        /**
         * Add a context menu to the widget that allows it to be removed from the menu/toolbar
         * and shows the general customize dialog.
         * @param widget {qx.ui.core.Widget} a UI widget.
         * @param cmd {skel.Command.Command} the cmd the widget executes.
         * @param toolbar {boolean} true if the widget is to appear on the toolbar; false otherwise.
         */
        addContext : function( widget, cmd, toolbar ){
            var contextMenu = new qx.ui.menu.Menu();
            var removeButton = new qx.ui.menu.Button( "Remove");
            removeButton.addListener( "execute", function(){
                if ( toolbar ){
                    cmd.setVisibleToolbar( false );
                }
                else {
                    cmd.setVisibleMenu( false );
                }
            }, this );
            contextMenu.add( removeButton );
            var customizeButton = new qx.ui.menu.Button("Customize...");
            var showDialog = skel.Command.Customize.CommandShowCustomizeDialog.getInstance();
            customizeButton.addListener("execute", function() {
                showDialog.doAction( !toolbar, null );
            }, this);
            contextMenu.add( customizeButton );
            widget.setContextMenu( contextMenu);
        },
        
        /**
         * Returns the 'left' position of the widget.
         * 
         * @param widget
         *                {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getLeft : function(widget) {
            return widget.getBounds().left;
        },

        /**
         * Returns the 'right' position of the widget.
         * 
         * @param widget {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getRight : function(widget) {
            var widgetBounds = widget.getBounds();
            return widgetBounds.left + widgetBounds.width;
        },

        /**
         * Returns the 'top' position of the widget.
         * 
         * @param widget {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getTop : function(widget) {
            return widget.getBounds().top;
        },

        /**
         * Returns the 'bottom' position of the widget.
         * 
         * @param widget {qx.ui.core.Widget} the widget whose position is to be
         *                determined.
         */
        getBottom : function(widget) {
            var widgetBounds = widget.getBounds();
            return widgetBounds.top + widgetBounds.height;
        },

        /**
         * Returns an array of size 2 representing the point that is located at
         * that center of the widget.
         * 
         * @param widget {qx.ui.core.Widget} the widget whose center is to be
         *                determined.
         */
        getCenter : function(widget) {
            var widgetBounds = widget.getBounds();
            var box = widget.getContentLocation("box");
            var left = 0;
            var top = 0;
            if ( box ){
                left = box.left;
                top = box.top;
            }
            var xCoord = left;
            var yCoord = top;
            if ( widgetBounds ){
                xCoord = left + Math.round(widgetBounds.width / 2);
                yCoord = top + Math.round(widgetBounds.height / 2);
            }
            return [ xCoord, yCoord ];
        },

        /**
         * Returns mouse event's local position (with respect to the event
         * target)
         *
         * Fixing getTarget() to getCurrentTarget(), but I am not sure how reliable this is.
         * I think the right way to do this is to get the "box" of the element on which you
         * received the event and subract it from the mouse x/y. Or you could pass the
         * box to this function.
         */
        localPos : function( target, event) {
            var box = target.getContentLocation("box");
           
            var left = event.getDocumentLeft();
            var top = event.getDocumentTop();
            if ( box ){
                left = left - box.left;
                top = top - box.top;
            }
            
            return  {
                x : left,
                y : top
            };
        },
        
        /**
         * Construct a button for the given command and callback.
         * @param cmd {skel.Command.Command}
         * @param cb {Function} a callback for the command.
         * @param tool {boolean} true if it should be a tool bar button; false for a menu button.
         * @param value {Object} the command's value for its action.
         */
        makeButton : function( cmd, cb, tool, value){
            var label = cmd.getLabel();
            var button = new qx.ui.menu.Button( label );
            if ( tool ){
                button = new qx.ui.toolbar.Button( label );
            }
            button.addListener( "execute", function(){
                this.doAction( value, cb );
            }, cmd );
            if ( tool ){
                skel.widgets.Util.addContext( button, cmd, tool );
            }
            return button;
        },
        
        
        /**
         * Make a checkable menu item for the cmd.
         * @param cmd {skel.Command.Command}
         * @param cb {Function} a callback for the command.
         * @param tool {boolean} true if it should be a tool bar button; false for a menu button.
         */
        makeCheck : function( cmd, cb, tool ){
            var checkBox = new qx.ui.menu.CheckBox();
            if ( tool ){
                checkBox = new qx.ui.form.CheckBox();
            }
            checkBox.setCommand( cmd );
            var labelFunction = function( cmd, checkBox){
                return function(){
                    cmd.doAction( checkBox.getValue(), null);
                };
            };
            checkBox.addListener( "changeValue", labelFunction( cmd,checkBox), checkBox);
            if ( tool ){
                
                skel.widgets.Util.addContext( checkBox, cmd, tool );
            }
            return checkBox;
        },
        
        /**
         * Return a menu consisting of the children of the command.
         * @param cmds {skel.Command.Command}.
         */
        makeMenu : function( cmds ){
            var cmdMenu = new qx.ui.menu.Menu();
            if ( cmds.getType() === skel.Command.Command.TYPE_GROUP ){
                var radioItems = skel.widgets.Util.makeRadioGroup( cmds );
                for ( var j = 0; j < radioItems.length; j++ ){
                    cmdMenu.add( radioItems[j]);
                }
            }
            else {
                var vals = cmds.getValue();
                var emptyCB = function(){};
                for ( var i = 0; i < vals.length; i++ ){
                    var cmdType = vals[i].getType();
                    if ( cmdType === skel.Command.Command.TYPE_COMPOSITE ){
                        var menuButton = new qx.ui.menu.Button( vals[i].getLabel());
                        var enabled = vals[i].isEnabled();
                        menuButton.setEnabled( vals[i].isEnabled() );
                        cmdMenu.add( menuButton );
                        var subMenu = skel.widgets.Util.makeMenu( vals[i]);
                        menuButton.setMenu( subMenu );
                    }
                    else if ( cmdType === skel.Command.Command.TYPE_GROUP ){
                        var radios = skel.widgets.Util.makeRadioGroup( vals[i]);
                        for ( var k = 0; k < radios.length; k++ ){
                            cmdMenu.add( radios[k]);
                        }
                    }
                    else if ( cmdType === skel.Command.Command.TYPE_BOOL ){
                        var checkItem = skel.widgets.Util.makeCheck( vals[i], emptyCB, false );
                        cmdMenu.add( checkItem );
                    }
                    else if ( cmdType == skel.Command.Command.TYPE_BUTTON ){
                        var button = skel.widgets.Util.makeButton( vals[i], emptyCB );
                        cmdMenu.add( button );
                    }
                    else {
                        console.log( "Menu unsupported command type="+ cmdType );
                    }
                }
            }
            return cmdMenu;
        },
        
        /**
         * Returns a group of radio menu items representing the children of the command.
         * @param cmd {skel.Command.CommandGroup} a command with the property that only one of
         *      its children can be active at a time.
         */
        makeRadioGroup : function( cmd, tool ){
            var radios = [];
            var radioGroup = new qx.ui.form.RadioGroup();
            radioGroup.setAllowEmptySelection(true);
            var values = cmd.getValue();
            var labelFunction = function( cmd, button){
                return function(){
                    if ( button.getValue()){
                        cmd.doAction( true, null);
                    }
                };
            };
            
            for ( var i = 0; i < values.length; i++ ){
                var button = null;
                if ( !tool && values[i].isVisibleMenu() ){
                    button = new qx.ui.menu.RadioButton();
                }
                else if ( tool && values[i].isVisibleToolbar() ){
                    button = new qx.ui.toolbar.RadioButton();
                }
                if ( button !== null ){
                    radios.push( button );
                    button.setCommand( values[i] );
                    button.addListener("changeValue", labelFunction(values[i], button), button);
                    radioGroup.add(button);
                    button.setFocusable(false);
                    if ( tool ){
                        skel.widgets.Util.addContext( button, values[i], tool );
                    }
                }
            }
            return radios;
        },
        
        /**
         * Converts a string containing 'true' or 'false' to a boolean.
         * @param valStr {String} the string to translate.
         * @return {boolean} true if the string contains 'true'; false otherwise.
         */
        toBool : function ( valStr ){
            var val = false;
            if ( valStr === "true"){
                val = true;
            }
            return val;
        }

    }

});