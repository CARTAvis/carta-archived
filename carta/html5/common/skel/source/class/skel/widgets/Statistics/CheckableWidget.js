/**
 * Model for an object that can be checked, selected, and moved (Label, boolean).
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Statistics.CheckableWidget", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._setLayout( new qx.ui.layout.HBox(2) );
        this.m_label = new qx.ui.basic.Label("");
        this.m_check = new qx.ui.form.CheckBox();
        this.m_check.addListener( "changeValue", this._sendVisibleCmd, this );
        
        this._add( this.m_check );
        this._add( this.m_label );
        this._add( new qx.ui.core.Spacer(2), {flex:1} );
        
        this.setDraggable( true );
        this.setToolTipText( "Drag to reorder statistics." );
        
        this.addListener( "mousedown", function(){
            this.setSelected( true );
        }, this );
        
        this.addListener( "mouseup", function(){
            this.setSelected( false );
        }, this );
        
        this.addListener( "mouseover", function(){
            this.m_mouseOver = true;
        }, this );
        
        this.addListener( "mouseout", function(){
            this.m_mouseOver = false;
        }, this );
        
        this.addListener( "dragstart", function(e){
            e.addAction( "move");
            //e.addType( this.m_LOC );
            var data = {
                title : this.getLabel(),
                row : this.getRow(),
                col : this.getCol()
            }
            this.fireDataEvent( "dragStart", data );
        }, this);
        
        this.addListener( "drag", function(e){
            var left = e.getDocumentLeft();
            var top = e.getDocumentTop();
            var data = {
                posX : left,
                posY : top
            }
            this.fireDataEvent( "dragging", data );
        }, this );
        
        this.addListener( "dragend", function(e){
            this.fireDataEvent( "dragEnd", null );
        }, this );
    },
    
    events: {
        "dragStart" : "qx.event.type.Data",
        "dragging" : "qx.event.type.Data",
        "dragEnd" : "qx.event.type.Data"
    },
    
    properties : {
        checkEnabled : {init : true, apply : "_applyEnabled" },
        col : { init : 0 },
        label : { init : "", apply : "_applyLabel"  },
        row : { init : 0 },
        selected : {init :false, apply : "_applySelected"},
        value : { init :true, apply : "_applyValue" },
        appearance : {
            refine : true,
            init : "fakeButton"
        }
    },
    
    members : {
        
        /**
         * Update the enabled status of the UI.
         */
        _applyEnabled : function(){
            this.m_check.setEnabled( this.getCheckEnabled() );
        },
        
        /**
         * Update the text on the UI.
         */
       _applyLabel : function(){
           this.m_label.setValue( this.getLabel() );
           this.m_check.setToolTipText( "Show/hide the "+this.getLabel()+" statistic.");
           skel.widgets.TestID.addTestId( this.m_check, this.getLabel() + "StatVisible");
       },
       
       /**
        * Update the selected status of the UI.
        */
       _applySelected : function(){
           if ( this.getCheckEnabled() ){
               var selected = this.getSelected();
               if ( selected ){
                   this.setDecorator( "button-pressed");
               }
               else {
                   this.setDecorator( "button");
               }
           }
       },
       
       /**
        * Update the checked status of the UI.
        */
       _applyValue : function(){
           this.m_check.setValue( this.getValue() );
       },
       
       /**
        * Returns true if the mouse is hovered over this widget; false otherwise.
        * @return {boolean} - true if the mouse is hovered over this widget; false otherwise.
        */
       isMouseOver : function(){
           return this.m_mouseOver;
       },
       
       /**
        * Send a command to the server to set a stat visible/invisible.
        */
       _sendVisibleCmd : function(){
           if ( this.m_id !== null ){
               var showStat = this.m_check.getValue();
               var path = skel.widgets.Path.getInstance();
               var cmd = this.m_id + path.SEP_COMMAND + "setStatVisible";
               var params = "visible:"+showStat+", name:"+this.getLabel()+", type:"+this.m_statType;
               this.m_connector.sendCommand( cmd, params, null);
           }
       },
       
       /**
        * Set the server-side id.
        * @param id {String} - the server-side id of the statistic object.
        */
       setId : function( id ){
           this.m_id = id;
       },
       
       /**
        * Set the type of statistic.
        * @param type {String} - an identifier for the type of statistic such as
        *       region or imagee.
        */
       setStatType : function( type ){
           this.m_statType = type.toLowerCase();
       },
       
       m_connector : null,
       m_id : null,
       m_label : null,
       m_check : null,
       //m_LOC : "location",
       m_mouseOver : false,
       m_statType : null
    }
});