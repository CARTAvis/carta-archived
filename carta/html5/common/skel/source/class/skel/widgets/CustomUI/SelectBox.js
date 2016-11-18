/**
 * A select box capable of sending value changes to the server and receiving
 * server updates.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.CustomUI.SelectBox", {
    extend : qx.ui.form.SelectBox, 

    /**
     * Constructor.
     * @param cmd {String} - the cmd to send to the server when a value changes.
     * @param paramId {String} - an identifier for the value to send to the server.
     */
    construct : function( cmd, paramId) {
        this.base(arguments);
        this.m_cmd = cmd;
        this.m_paramId = paramId;
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
        this.m_selectListener = this.addListener( "changeSelection", this._sendCmd, this );
    },
    
    events: {
        "selectChanged" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Return the index of the first selected item.
         * @return {Number} - the index of the first selected item.
         */
        getIndex : function(){
            var selections = this.getSelection();
            var selection = null;
            var index = -1;
            if ( selections.length > 0 ){
                selection = selections[0].getLabel();
                var selectables = this.getSelectables(true);
                for ( var i = 0; i < selectables.length; i++ ){
                    var selectValue = selectables[i].getLabel();
                    if ( selectValue == selection ){
                        index = i;
                        break;
                    }
                }
            }
            return index;
        },
        
        /**
         * Return the first value that the user selected.
         * @return {String} - the first user selected value or null if there
         *      is no such value.
         */
        getValue : function(){
            var selections = this.getSelection();
            var selection = null;
            if ( selections.length > 0 ){
                selection = selections[0].getLabel();
            }
            return selection;
        },
        
        /**
         * Sends a value change to the server.
         */
        _sendCmd : function(){
        	if ( this.m_connector !== null ){
        		var errorMan = skel.widgets.ErrorHandler.getInstance();
        		errorMan.clearErrors();
        		var selectValue = this.getValue();
        		if ( selectValue !== null && selectValue.length > 0 ){
        			if ( this.m_id !== null ){
                
        				var path = skel.widgets.Path.getInstance();
        				var cmd = this.m_id + path.SEP_COMMAND + this.m_cmd;
                
        				var params = this.m_paramId + ":"+selectValue;
        				this.m_connector.sendCommand( cmd, params, function(){});
        			}
        			this.fireDataEvent( "selectChanged", null );
        		}
        	}
        },
        
        /**
         * Update the list of select box items.
         * @param items {Array} the new select items to display.
         */
       setSelectItems : function ( items ){
           this.removeListenerById( this.m_selectListener );
           var oldValue = this.getValue();
           this.removeAll();
           var selection = [];
           for ( var i = 0; i < items.length; i++ ){
               var newValue = items[i]+"";
              
               var tempItem = new qx.ui.form.ListItem( newValue );
               if ( newValue == oldValue ){
                   selection.push( tempItem );
               }
               this.add( tempItem );
           }
          
           //Try to reset the old selection
           if ( oldValue !== null ){ 
               if ( items.length > 0 ){
                   this.setSelection( selection );
               }
           }
           //Select the first item
           else if ( items.length > 0 ){
               var selectables = this.getSelectables(true);
               if ( selectables.length > 0 ){
                   var selection = [];
                   selection.push( selectables[0])
                   this.setSelection( selection );
               }
           }
           this.m_selectListener =this.addListener( "changeSelection", 
                   this._sendCmd, this );
       },

       /**
        * Set the new value of the select box.
        * @param value {String} the new select box selected value.
        * @param notify {boolean} - true if others should be notified of the change; false otherwise.
        */
        setSelectValue : function( value, notify ){
            var selectables = this.getSelectables(true);
            var currValue = this.getValue();
            for ( var i = 0; i < selectables.length; i++ ){
                var mapItem = selectables[i];
                var selectValue = selectables[i].getLabel();
                if ( selectValue == value ){
                    if ( currValue != selectValue ){
                        if ( !notify ){
                            this.removeListenerById( this.m_selectListener );
                        }
                        var selections = [];
                        selections.push( selectables[i] );
                        this.setSelection( selections );
                        if ( !notify ){
                            this.m_selectListener =this.addListener( "changeSelection", 
                                this._sendCmd, this );
                        }
                    }
                    break;
                }
            }
        },
        
        
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_id : null,
        m_connector : null,
        m_cmd : null,
        m_paramId : null,
        m_selectListener : null
    }


});