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
        this.m_connector = mImport("connector");
        this.m_comboListener = this.addListener( skel.widgets.Path.CHANGE_VALUE, this._sendCmd, this );
    },
    


    members : {
        
        /**
         * Sends a value change to the server.
         */
        _sendCmd : function(){
            if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + this.m_cmd;
                var coordSystem = this.getValue();
                var params = this.m_paramId + ":"+coordSystem;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Update the list of combo box items.
         * @param items {Array} the new combo box items to display.
         */
       setComboItems : function ( items ){
           this.removeAll();
           var oldValue = this.getValue();
           for ( var i = 0; i < items.length; i++ ){
               var newValue = items[i]+"";
               var tempItem = new qx.ui.form.ListItem( newValue );
               this.add( tempItem );
           }
           //Try to reset the old selection
           if ( oldValue !== null ){
               this.setValue( oldValue );
           }
           //Select the first item
           else if ( items.length > 0 ){
               var selectables = this.getChildrenContainer().getSelectables(true);
               if ( selectables.length > 0 ){
                   this.setValue( selectables[0].getLabel());
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
         * @param gridId {String} the server side id of the object that contains data for this control UI.
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