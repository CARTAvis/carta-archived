/**
 * Displays controls for customizing the grid color.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.GridColor", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this.m_gridColor = new skel.widgets.Grid.ColorInfo();
        this.m_axesColor = new skel.widgets.Grid.ColorInfo();
        this.m_labelColor = new skel.widgets.Grid.ColorInfo();
        this._init();
    },

    members : {
        
        /**
         * Returns the parameters to send to the server when the color has changed.
         * @return {String} - the new color values.
         */
        _getColorParams : function(){
            var red = this.m_colorPicker.getRed();
            var green = this.m_colorPicker.getGreen();
            var blue = this.m_colorPicker.getBlue();
            var params = "red:"+red+",green:"+green+",blue:"+blue;
            return params;
        },

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.groupbox.GroupBox( "Colors");
            this.m_content.setContentPadding( 0, 0, 0, 0 );
            this._add( this.m_content );
            this.m_content.setLayout(new qx.ui.layout.HBox());
            this._initControls();
        },
        
        /**
         * Initialize the color controls.
         */
        _initControls : function(){
           this.m_partsList = new qx.ui.form.List();
           this.m_partsList.setSelectionMode( "one");
           this.m_partsList.setWidth( 100 );
           this.m_partsList.setHeight( 100 );
           this.m_gridItem = new qx.ui.form.ListItem( "Grid");
           this.m_axesItem = new qx.ui.form.ListItem( "Axes/Border");
           this.m_labelItem = new qx.ui.form.ListItem( "Labels");
           this.m_partsList.add( this.m_gridItem );
           this.m_partsList.add( this.m_axesItem );
           this.m_partsList.add( this.m_labelItem );
           this.m_partsList.setSelection( this.m_gridItem );
           this.m_colorPicker = new skel.widgets.CustomUI.ColorSelector();
           this.m_colorListenerId = this.m_colorPicker.addListener( "changeValue", this._sendCmd, this );
           this.m_partsList.addListener( "changeSelection", this._partSelected, this );
           this.m_content.add( this.m_partsList );
           this.m_content.add( this.m_colorPicker );
        },
        
        /**
         * Update the color picker when the user has selected a new item
         * for color configuration.
         */
        _partSelected : function(){
            var selectedItems = this.m_partsList.getSelection();
            if ( selectedItems.length > 0 ){
                if ( selectedItems[0] == this.m_gridItem ){
                    this._setColorInPicker( this.m_gridColor );
                }
                else if ( selectedItems[0] == this.m_axesItem ){
                    this._setColorInPicker( this.m_axesColor );
                }
                else if ( selectedItems[0] == this.m_labelItem ){
                    this._setColorInPicker( this.m_labelColor );
                }
                else {
                    console.log( "could not find a selected item");
                }
            }
        },
        
        /**
         * Send a commmand to the server when a color has changed.
         */
        _sendCmd : function(){
            var selectedItems = this.m_partsList.getSelection();
            if ( selectedItems.length > 0 ){
                if ( selectedItems[0] == this.m_gridItem ){
                    this._sendGridColorCmd();
                }
                else if ( selectedItems[0] == this.m_axesItem ){
                    this._sendAxesColorCmd();
                }
                else if ( selectedItems[0] == this.m_labelItem ){
                    this._sendLabelColorCmd();
                }
                else {
                    console.log( "could not find a selected item");
                }
            }
        },
        
        /**
         * Notify the server that the label color has changed.
         */
        _sendLabelColorCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setLabelColor";
            var params = this._getColorParams();
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        /**
         * Notify the server that the grid color has changed.
         */
        _sendGridColorCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setGridColor";
            var params = this._getColorParams();
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        /**
         * Notify the server that the axes color has changed.
         */
        _sendAxesColorCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setAxesColor";
            var params = this._getColorParams();
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        /**
         * Update the color picker with the passed in color.
         * @param colorInfo {skel.widgets.Grid.ColorInfo}.
         */
        _setColorInPicker : function( colorInfo ){
            this.m_colorPicker.setRed( colorInfo.getRed() );
            this.m_colorPicker.setGreen( colorInfo.getGreen() );
            this.m_colorPicker.setBlue( colorInfo.getBlue());
        },
        
        /**
         * Update the colors based on server-side values.
         * @param controls {Object} - server side grid color values.
         */
        setControls : function( controls ){
            this.m_colorPicker.removeListenerById( this.m_colorListenerId );
            this.m_gridColor.setRed( controls.grid.red);
            this.m_gridColor.setGreen( controls.grid.green);
            this.m_gridColor.setBlue( controls.grid.blue );
            this.m_axesColor.setRed( controls.axes.red );
            this.m_axesColor.setGreen( controls.axes.green );
            this.m_axesColor.setBlue( controls.axes.blue );
            this.m_labelColor.setRed( controls.labels.red );
            this.m_labelColor.setBlue( controls.labels.blue );
            this.m_labelColor.setGreen(controls.labels.green );
            this._partSelected();
            this.m_colorListenerId = this.m_colorPicker.addListener( "changeValue", this._sendCmd, this );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
        },
        m_gridColor : null,
        m_axesColor : null,
        m_labelColor : null,
        
        m_gridItem : null,
        m_axesItem : null,
        m_labelItem : null,
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_partsList : null,
        m_colorPicker : null,
        m_colorListenerId : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }


});