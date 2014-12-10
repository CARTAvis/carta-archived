/**
 * Controls for the histogram zoom settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramZoom", {
    extend : qx.ui.core.Widget,

    construct : function( ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init( );
    },
    
    statics : {
        CMD_ZOOM_FULL : "zoomFull",
        CMD_ZOOM_RANGE : "zoomRange"
    },

    members : {

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);

            var zoomGroup = new qx.ui.groupbox.GroupBox( "Zoom Range");
            zoomGroup.setLayout( new qx.ui.layout.VBox(2));
            
            this._add( zoomGroup );
            this.m_fullRange = new qx.ui.form.Button( "Full" );
            this.m_fullRange.addListener( "execute", function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramZoom.CMD_ZOOM_FULL;
                var params = "";
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            this.m_selectedRange = new qx.ui.form.Button( "Selected");
            this.m_selectedRange.addListener( "execute", function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramZoom.CMD_ZOOM_RANGE;
                var params = "";
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            zoomGroup.add( this.m_fullRange );
            zoomGroup.add( this.m_selectedRange );
        },
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_id : null,
        m_connector : null,
        m_fullRange : null,
        m_selectedRange : null
    }
});