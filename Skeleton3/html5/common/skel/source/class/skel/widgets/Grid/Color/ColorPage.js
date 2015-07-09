/**
 * Displays controls for customizing the grid color.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.Color.ColorPage", {
    extend : qx.ui.tabview.Page,
    
    /**
     * Constructor.
     * @param sendCmd {String} the command for setting color on the server.
     */
    construct : function( sendCmd ) {
        this.base(arguments, "Color", "");
        this.m_connector = mImport("connector");
        this.m_cmd = sendCmd;
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
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout(new qx.ui.layout.VBox(1));
            this.m_content = new qx.ui.container.Composite();
            this.m_content.setPadding( 0, 0, 0, 0 );
            this._add( this.m_content, {flex:1} );
            this.m_content.setLayout(new qx.ui.layout.HBox(2));
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
            this._initControls();
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
        },
        
        /**
         * Initialize the color controls.
         */
        _initControls : function(){
           this.m_colorPicker = new skel.widgets.CustomUI.ColorSelector();
           this.m_colorPicker.addListener( 'appear', function(){
               this.m_appeared = true;
               this._updateColors();
           }, this );
           this.m_colorListenerId = this.m_colorPicker.addListener( "changeValue", this._sendCmd, this );
           this.m_content.add( this.m_colorPicker );
        },
        
        /**
         * Notify the server that the grid color has changed.
         */
        _sendCmd : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + this.m_cmd;
            var params = this._getColorParams();
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        
        
        /**
         * Update the colors based on server-side values.
         * @param controls {Object} - server side grid color values.
         */
        setControls : function( controlColor ){
            if ( typeof controlColor != "undefined"){
                this.m_red = controlColor.red;
                this.m_green = controlColor.green;
                this.m_blue = controlColor.blue;
                this._updateColors();
            }
        },
        
        /**
         * Set the server side id..
         * @param gridId {String} the server side id of the object that 
         *              contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
           
        },
        
        /**
         * Store the new colors in the picker.
         */
        _updateColors : function( ){
            //Needed because if the color picker has not appeared before trying
            //to set things the internal updateContext gets out of whack and it remains broken.
            if ( this.m_appeared ){
                this.m_colorPicker.removeListenerById( this.m_colorListenerId );
                if ( this.m_red !== null ){
                    this.m_colorPicker.setRed( this.m_red );
                }
                if ( this.m_green !== null ){
                    this.m_colorPicker.setGreen( this.m_green);
                }
                if ( this.m_blue !== null ){
                    this.m_colorPicker.setBlue( this.m_blue  );
                }
                this.m_colorListenerId = this.m_colorPicker.addListener( "changeValue", this._sendCmd, this );
            }
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_red : null,
        m_green : null,
        m_blue : null,
        m_appeared : false,
        
        m_colorPicker : null,
        m_colorListenerId : null
    }

});