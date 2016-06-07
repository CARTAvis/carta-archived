/**
 * Controls for the color map model selection.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorModel", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init( );
    },

    members : {

        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.HBox(1);
            this._setLayout(widgetLayout);
            
            this.m_gammaContainer = new qx.ui.container.Composite();
            this.m_gammaContainer.setLayout( new qx.ui.layout.VBox(2));
            
            
            var gammaTextContainer = new qx.ui.container.Composite();
            gammaTextContainer.setLayout( new qx.ui.layout.HBox(2) );
            var gammaLabel = new qx.ui.basic.Label( "Gamma:");
            this.m_gammaText = new skel.widgets.CustomUI.NumericTextField( 0, null );
            this.m_gammaListenId = this.m_gammaText.addListener( "textChanged",
                    this._sendGammaCmd, this );
            this.m_gammaText.setIntegerOnly( false );
            this.m_gammaText.setToolTipText( "Set gamma value.");
            gammaTextContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            gammaTextContainer.add( gammaLabel );
            gammaTextContainer.add( this.m_gammaText );
            gammaTextContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            this.m_gammaContainer.add( gammaTextContainer );
            
            
            this.m_gammaGrid = new skel.widgets.Colormap.TwoDSlider();
            this.m_gridListenId = this.m_gammaGrid.addListener( skel.widgets.Path.CHANGE_VALUE, function(evt){
                var data = evt.getData();
               this._sendScaledChangedCmd( data.x, data.y );
            }, this );
            this.m_gammaContainer.add( this.m_gammaGrid, {flex:1});
            
            this._add( this.m_gammaContainer, {flex:1} );
        },
        
        /**
         * Notify the server that gamma has changed.
         */
        _sendGammaCmd : function(){
            if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var value = this.m_gammaText.getValue();
                if ( value >= 0 ){
                    var cmd = this.m_id + path.SEP_COMMAND + "setGamma";
                    var params = "gamma:"+value;
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
            }
        },
        
        /**
         * Notify the server that the scales have changed.
         * @param scale1 {Number} the first gamma scale.
         * @param scale2 {Number} the second gamma scale.
         */
        _sendScaledChangedCmd : function( scale1, scale2 ){
            if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setScales";
                var params = "scale1:"+scale1+",scale2:"+scale2;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Set the gamma value from the server.
         * @param value {Number} the gamma value.
         */
        setGamma : function( value ){
            if ( this.m_gammaListenId !== null ){
                this.m_gammaText.removeListenerById( this.m_gammaListenId );
            }
            this.m_gammaText.setValue( value );
            this.m_gammaListenId = this.m_gammaText.addListener( "textChanged",
                    this._sendGammaCmd, this );
        },
        
        /**
         * Set the (x,y) grid coordinates based on server values.
         * @param x {Number} - the x-location on the grid.
         * @param y {Number} - the y-location on the grid.
         */
        setGammaPosition : function( x, y ){
            if ( this.m_gridListenId !== null ){
                this.m_gammaGrid.removeListenerById( this.m_gridListenId );
            }
            this.m_gammaGrid.setValue( x, y );
            this.m_gridListenId = this.m_gammaGrid.addListener( skel.widgets.Path.CHANGE_VALUE, function(evt){
                var data = evt.getData();
               this._sendScaledChangedCmd( data.x, data.y );
            }, this );
        },
        
        /**
         * Set the server side id of the color map.
         * @param id {String} the unique server side id of this color map.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_id : null,
        m_connector : null,
        m_gammaGrid : null,
        m_gammaText : null,
        m_gammaListenId : null,
        m_gridListenId : null,
        m_gammaContainer : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});