/**
 * Displays controls for customizing histogram display properties.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.PageDisplay", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Display", "");
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
            
            this.m_binSettings = new skel.widgets.Histogram.HistogramBin();
            this.m_displaySettings = new skel.widgets.Histogram.HistogramDisplay();
            this.m_digitSettings = new skel.widgets.Histogram.HistogramDigits();
            
            this.add( this.m_binSettings );
            this.add( this.m_displaySettings );
            this.add( this.m_digitSettings );
        },
        

        
        /**
         * Update the UI based on server histogram preferences.
         * @param hist {Object} - server-side settings.
         */
        histUpdate : function( hist ){
            if ( this.m_binSettings !== null ){
                this.m_binSettings.setBinCountMax( hist.binCountMax );
                this.m_binSettings.setBinCount(hist.binCount );
                this.m_binSettings.setBinWidth(hist.binWidth );
            }
            
            if ( this.m_displaySettings !== null ){
                this.m_displaySettings.setStyle(hist.graphStyle);
                this.m_displaySettings.setLogCount( hist.logCount );
                this.m_displaySettings.setColored( hist.colored );
                this.m_digitSettings.setControls( hist );
            }
        },
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_binSettings.setId( id );
            this.m_displaySettings.setId ( id );
            this.m_digitSettings.setId( id );
        },
        
        m_id : null,
        
        m_binSettings : null,
        m_digitSettings : null,
        m_displaySettings : null

    }
});