/**
 * Displays controls for customizing the histogram.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.Settings", {
    extend : qx.ui.core.Widget, 
    include : skel.widgets.MTabMixin,

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },
    

    members : {
        
        /**
         * Callback for a change in histogram preference settings.
         */
        _histogramChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var hist = JSON.parse( val );
                    if ( this.m_pages[this.m_INDEX_RANGE] !== null ){
                        this.m_pages[this.m_INDEX_RANGE].histUpdate( hist );
                    }
                    if ( this.m_pages[this.m_INDEX_DISPLAY] !== null ){
                        this.m_pages[this.m_INDEX_DISPLAY].histUpdate( hist );
                    }
                    if ( this.m_pages[this.m_INDEX_SELECT] !== null ){
                        this.m_pages[this.m_INDEX_SELECT].histUpdate( hist );
                    }
                    var tabIndex = hist.tabIndex;
                    this._selectTab( tabIndex );
                }
                catch ( err ){
                    console.log( "Problem updating hist: "+val );
                    console.log( "Error: "+err);
                }
            }
        },
        
        /**
         * Callback for a change in histogram data dependent settings.
         */
        _histogramDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var hist = JSON.parse( val );
                    if ( this.m_pages[this.m_INDEX_RANGE] !== null ){
                        this.m_pages[this.m_INDEX_RANGE].histDataUpdate( hist );
                    }
                    if ( this.m_pages[this.m_INDEX_SELECT] !== null ){
                        this.m_pages[this.m_INDEX_SELECT].histDataUpdate( hist );
                    }
                }
                catch( err ){
                    console.log( "Could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );

            this._setLayout( new qx.ui.layout.VBox(1));

            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabListenId = this.m_tabView.addListener( "changeSelection", this._sendTabIndex, this );
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this._add( this.m_tabView );
            
            this.m_pages = [];
            this.m_pages[this.m_INDEX_DISPLAY] = new skel.widgets.Histogram.PageDisplay();
            this.m_pages[this.m_INDEX_RANGE] = new skel.widgets.Histogram.PageRange();
            this.m_pages[this.m_INDEX_SELECT] = new skel.widgets.Histogram.PageSelection();
            
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_tabView.add( this.m_pages[i] );
            }
        },
        
        /**
         * Register to get updates on histogram settings from the server.
         */
        _registerHistogram : function(){
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._histogramChangedCB.bind(this));
            var dataPath = this.m_id + path.SEP + path.DATA;
            this.m_sharedVarData = this.m_connector.getSharedVar( dataPath );
            this.m_sharedVarData.addCB( this._histogramDataCB.bind( this));
            this._histogramChangedCB();
            this._histogramDataCB();
            
        },

        
        /**
         * Send a command to the server to get the data update id.
         * @param id {String} the server side id of the histogram object.
         */
        setId : function( id ){
            this.m_id = id;
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_pages[i].setId( id );
            }
            this._registerHistogram();
        },
        
        m_sharedVar : null,
        m_sharedVarData : null,
        
        m_INDEX_DISPLAY : 0,
        m_INDEX_RANGE : 1,
        m_INDEX_SELECT : 2
    }
});