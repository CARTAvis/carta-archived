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
            this.setAllowGrowX( true );
            this.setAllowGrowY( true );
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            
            this.m_gammaContainer = new qx.ui.groupbox.GroupBox( "");
            this.m_gammaContainer.setContentPadding( 0, 0, 0, 0);
            this.m_gammaContainer.setLayout( new qx.ui.layout.VBox(2));
            
            this.m_gammaGrid = new skel.widgets.Colormap.TwoDSlider();
            this.m_gammaGrid.addListener( skel.widgets.Path.CHANGE_VALUE, function(evt){
                var data = evt.getData();
               this._sendScaledChangedCmd( data.x, data.y );
            }, this );
            this.m_gammaContainer.add( this.m_gammaGrid, {flex:1});
            this._add( this.m_gammaContainer, {flex:1} );
        },
        
        /**
         * Notify the server that the scales have changed.
         * @param scale1 {Number} the first gamma scale.
         * @param scale2 {Number} the second gamma scale.
         */
        _sendScaledChangedCmd : function( scale1, scale2 ){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setScales";
            var params = "scale1:"+scale1+",scale2:"+scale2;
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        /**
         * Set the gamma value from the server.
         * @param value {Number} the gamma value.
         */
        setGamma : function( value ){
            this.m_gammaContainer.setLegend( "Gamma: "+value );
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
        m_gammaContainer : null
    }
});