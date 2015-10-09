/**
 * Displays controls for customizing histogram range settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.PageRange", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Range", "");
        this._init( );
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.HBox(2));
           
            this.m_rangeSettings = new skel.widgets.Histogram.HistogramRange();
            this.m_clipSettings = new skel.widgets.Histogram.HistogramClip();
            
            this.add( this.m_rangeSettings );
            this.add( this.m_clipSettings );
        },
        
        
        /**
         * Update range user preferences based on server-side values.
         * @param hist {Object} - server-side information.
         */
        histUpdate : function( hist ){
            if ( this.m_rangeSettings !== null ){
                this.m_rangeSettings.setBuffer( hist.useClipBuffer );
            }
        },
        
        /**
         * Update range data based on server-side values.
         * @param hist {Object} - server-side information.
         */
        histDataUpdate : function(hist){
            if ( this.m_clipSettings !== null ){
                this.m_clipSettings.setColorRange( hist.colorMin, hist.colorMax );
                this.m_clipSettings.setColorRangePercent( hist.colorMinPercent, hist.colorMaxPercent);
            }
            
            if ( this.m_rangeSettings !== null ){
                this.m_rangeSettings.setClipBounds( hist.clipMinClient, hist.clipMaxClient );
                this.m_rangeSettings.setClipPercents( hist.clipMinPercent, hist.clipMaxPercent);
                this.m_rangeSettings.setBufferAmount( hist.clipBuffer );
            }
        },
        
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_clipSettings.setId( id );
            this.m_rangeSettings.setId ( id );
        },
        
        m_id : null,
        
        m_clipSettings : null,
        m_rangeSettings : null

    }
});