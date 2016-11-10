/**
 * Allows the user to choose a label format for one of the axes of the grid.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.Settings.GridLabelFormat", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     * @param side {String} - the side of the grid the label is on.
     * @param axis {Number} - an identifier for the type of axis.
     */
    construct : function(  side, axis ) {
        this.base(arguments);
        this.m_side = side;
        this.m_axis = axis;
        if ( typeof mImport !== "undefined"){
        	this.m_connector = mImport("connector");
        }
        this._init( );
        
        if ( this.m_connector !== null ){
        	var pathDict = skel.widgets.Path.getInstance();
        	this.m_sharedVarFormats = this.m_connector.getSharedVar(pathDict.LABEL_FORMATS);
        	this.m_sharedVarFormats.addCB(this._formatsChangedCB.bind(this));
        	this._formatsChangedCB();
        }
    },
   

    members : {
        /**
         * Callback for when the list of available formats change for the axis on
         * this particular side of the grid.
         */
        _formatsChangedCB : function(){
            if ( this.m_sharedVarFormats ){
                var val = this.m_sharedVarFormats.get();
                if ( val ){
                    try {
                        var formats = JSON.parse( val );
                        var count = formats.labelFormats.length;
                        var labelFormats = [];
                        for ( var i = 0; i < count; i++ ){
                            labelFormats.push( formats.labelFormats[i] );
                        }
                        
                        var supportedFormats = [];
                        var formatIndices = formats.axesFormats[this.m_axis];
                        var indexCount = formatIndices.length;
                        for ( i = 0; i < indexCount; i++ ){
                            supportedFormats.push( labelFormats[formatIndices[i]]);
                        }
                        this.m_select.setSelectItems( supportedFormats );
                    }
                    catch( err ){
                        console.log( "Grid label formats could not parse: "+val );
                        console.log( err );
                    }
                }
            }
        },
        
        /**
         * Initialize the UI.
         */
        _init : function(){
            this._setLayout( new qx.ui.layout.VBox() );
            var content = new qx.ui.container.Composite();
            content.setLayout( new qx.ui.layout.VBox(2) );
            this._add( new qx.ui.core.Spacer(), {flex:1});
            this._add( content );
            this._add( new qx.ui.core.Spacer(), {flex:1});
            
            this.m_select = new skel.widgets.CustomUI.SelectBox();
            this.m_selectListener = this.m_select.addListener( "selectChanged", 
                    this._sendFormatCmd, this );
            this.m_select.setToolTipText( "Select the format for grid labels on the "+this.m_side+" side of the grid");
            
            content.add( new qx.ui.core.Spacer(), {flex:1} );
            content.add( this.m_select );
            content.add( new qx.ui.core.Spacer(), {flex:1} );
        },
        
        /**
         * Sends the new format to the server.
         */
        _sendFormatCmd : function(){
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            errorMan.clearErrors();
            if ( this.m_id !== null && this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setGridLabelFormat";
                var selectValue = this.m_select.getValue();
                var colon = ':';
                var re = new RegExp( colon, 'g');
                selectValue = selectValue.replace( re, "-");
                var params = "format:"+selectValue+",side:"+this.m_side;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Update the list of select box items.
         * @param items {Array} the new select items to display.
         */
       setFormatItems : function ( items ){
           this.m_select.setSelectItems( items );
           
       },

       /**
        * Set the new value of the select box.
        * @param value {String} the new select box selected value.
        * @param notify {boolean} - true if others should be notified of the change; false otherwise.
        */
        setFormatValue : function( value){
            this.m_axis = value.axis;
            this.m_select.setSelectValue( value.format, true );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_axis : null,
        m_id : null,
        m_sharedVarFormats : null,
        m_connector : null,
        m_select : null,
        m_side : null,
        m_selectListener : null
    }
});