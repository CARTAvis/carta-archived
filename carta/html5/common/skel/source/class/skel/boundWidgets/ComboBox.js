/**
 * A combo box capable of sending value changes to the server and receiving
 * server updates.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.boundWidgets.ComboBox", {
    extend : qx.ui.form.ComboBox, 

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
        this.m_comboListener = this.addListener( skel.widgets.Path.CHANGE_VALUE, this._sendCmd, this );
    },
    
    events: {
        "comboChanged" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Sends a value change to the server.
         */
        _sendCmd : function(){
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            errorMan.clearErrors();
            if ( this.m_id !== null && this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + this.m_cmd;
                var coordSystem = this.getValue();
                var params = this.m_paramId + ":"+coordSystem;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
            else {
                this.fireDataEvent( "comboChanged", null );
            }
        },
        
        /**
         * Return a list of items in the combo box.
         * @return {Array} - the items displayed in the combo box.
         */
        getComboItems : function(){
            var items = [];
            var selectables = this.getChildrenContainer().getSelectables();
            for ( var i = 0; i < selectables.length; i++ ){
                items[i] = selectables[i].getLabel();
            }
            return items;
        },
        
        /**
         * Return the number of items in the combo box.
         * @return {Number} - the combo item count.
         */
        getItemCount : function(){
            var selectables = this.getChildrenContainer().getSelectables();
            return selectables.length;
        },
        
        /**
         * Return the value at the given index in the combo box.
         * @param index {Number} - the index of the combo box item.
         * @return {String} - the value at the given position in the combo box or
         *      a blank if no such item exists.
         */
        getValueAt : function( index ){
            var itemCount = this.getItemCount();
            var value = "";
            if ( index >= 0 && index < itemCount ){
                var selectables = this.getChildrenContainer().getSelectables();
                value = selectables[index].getLabel();
            }
            return value;
        },
        
        /**
         * Returns true if the lists are not the same; false otherwise.
         * @param oldItems {Array} - one list of items.
         * @param newItems {Array} - a second list of items.
         * @return {boolean} - true if the items are the same; false, otherwise.
         */
        isItemsChanged : function( oldItems, newItems ){
            var changed = false;
            if ( oldItems.length != newItems.length ){
                changed = true;
            }
            else {
                for ( var i = 0; i < oldItems.length; i++ ){
                    if ( oldItems[i] != newItems[i] ){
                        changed = true;
                        break;
                    }
                }
            }
            return changed;
        },
        
        /**
         * Update the list of combo box items.
         * @param items {Array} the new combo box items to display.
         */
       setComboItems : function ( items ){
           var oldItems = this.getComboItems();
           if ( this.isItemsChanged( oldItems, items ) ){
               this.removeAll();
               var oldValue = this.getValue();
               for ( var i = 0; i < items.length; i++ ){
                   var newValue = items[i]+"";
                   var tempItem = new qx.ui.form.ListItem( newValue );
                   this.add( tempItem );
               }
               //Try to reset the old selection
               if ( oldValue !== null ){ 
                   if ( items.length > 0 ){
                       this.setValue( oldValue );
                   }
               }
               //Select the first item
               else if ( items.length > 0 ){
                   var selectables = this.getChildrenContainer().getSelectables(true);
                   if ( selectables.length > 0 ){
                       this.setValue( selectables[0].getLabel());
                   }
               }
           }
       },

       /**
        * Set the new value of the combo box.
        * @param value {String} the new combo box selected value.
        */
        setComboValue : function( value ){
            var selectables = this.getChildrenContainer().getSelectables();
            var currValue = this.getValue();
            for ( var i = 0; i < selectables.length; i++ ){
                var mapItem = selectables[i];
                var newValue = selectables[i].getLabel();
                if ( newValue == value ){
                    if ( currValue != newValue ){
                        this.removeListenerById( this.m_comboListener );
                        this.setValue( newValue );
                        this.m_comboListener =this.addListener( skel.widgets.Path.CHANGE_VALUE, 
                                this._sendCmd, this );
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
        m_comboListener : null
    }


});