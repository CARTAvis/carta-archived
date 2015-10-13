/**
 * Displays controls for customizing histogram data selection.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.PageSelection", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Selection", "");
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
           
            this.m_cubeSettings = new skel.widgets.Histogram.HistogramCube();
            this.m_twoDSettings = new skel.widgets.Histogram.Histogram2D();
            
            this.add( this.m_cubeSettings );
            this.add( this.m_twoDSettings );
        },
        

        
        
        /**
         * Update user selection preferences based on server-side values.
         * @param hist {Object} - server-side information.
         */
        histUpdate : function( hist ){
            if ( this.m_cubeSettings !== null ){
                this.m_cubeSettings.setPlaneMode( hist.planeMode );
                this.m_cubeSettings.setUnit( hist.rangeUnit );
            }
           
            if ( this.m_twoDSettings !== null ){
                this.m_twoDSettings.setFootPrint( hist.twoDFootPrint );
            }
        },
        
        /**
         * Update user selection data based on server-side values.
         * @param hist {Object} - server-side information.
         */
        histDataUpdate : function( hist ){
            if ( this.m_cubeSettings !== null ){
                this.m_cubeSettings.setPlaneRangeEnabled( hist.image3D );
                this.m_cubeSettings.setPlaneChannelMax( hist.planeChannelMax );
                this.m_cubeSettings.setPlaneChannel( hist.planeChannel );
                this.m_cubeSettings.setPlaneBounds( hist.planeMin, hist.planeMax );
            }
        },
        
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_cubeSettings.setId( id );
            this.m_twoDSettings.setId ( id );
        },
        
        m_id : null,
        
        m_cubeSettings : null,
        m_twoDSettings : null
        
    }
});