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
            var widgetLayout = new qx.ui.layout.VBox(1);
            this._setLayout(widgetLayout);
            
            var zoomContainer = new qx.ui.groupbox.GroupBox("Zoom (mouse left drag)", "");
            zoomContainer.setContentPadding(1,1,1,1);
            zoomContainer.setLayout( new qx.ui.layout.VBox(1));
            this._add( zoomContainer );
            
            this.m_fullRange = new qx.ui.form.Button( "Full" );
            this.m_fullRange.setToolTipText( "Zoom out to full histogram range.");
            this.m_fullRange.addListener( "execute", function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramZoom.CMD_ZOOM_FULL;
                var params = "";
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            this.m_selectedRange = new qx.ui.form.Button( "Selected");
            this.m_selectedRange.setToolTipText( "Zoom to the graphically selected range.");
            this.m_selectedRange.addListener( "execute", function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramZoom.CMD_ZOOM_RANGE;
                var params = "";
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            zoomContainer.add( this.m_fullRange );
            zoomContainer.add( this.m_selectedRange );
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
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});