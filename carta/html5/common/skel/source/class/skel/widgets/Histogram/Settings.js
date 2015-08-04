/**
 * Displays controls for customizing the histogram.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.Settings", {
    extend : qx.ui.core.Widget, 

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
                    if ( this.m_rangePage !== null ){
                        this.m_rangePage.histUpdate( hist );
                    }
                    if ( this.m_displayPage !== null ){
                        this.m_displayPage.histUpdate( hist );
                    }
                    if ( this.m_selectPage !== null ){
                        this.m_selectPage.histUpdate( hist );
                    }
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
                    if ( this.m_rangePage !== null ){
                        this.m_rangePage.histDataUpdate( hist );
                    }
                    if ( this.m_selectPage !== null ){
                        this.m_selectPage.histDataUpdate( hist );
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
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this._add( this.m_tabView );
            
            this.m_displayPage = new skel.widgets.Histogram.PageDisplay();
            this.m_rangePage = new skel.widgets.Histogram.PageRange();
            this.m_selectPage = new skel.widgets.Histogram.PageSelection();
            
            this.m_tabView.add( this.m_rangePage );
            this.m_tabView.add( this.m_displayPage );
            this.m_tabView.add( this.m_selectPage );
        },
        
        /**
         * Register to get updates on histogram settings from the server.
         */
        _registerHistogram : function(){
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._histogramChangedCB.bind(this));
            var dataPath = this.m_id + path.SEP + "data";
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
            this.m_displayPage.setId( id );
            this.m_rangePage.setId( id );
            this.m_selectPage.setId( id );
            this._registerHistogram();
        },
        
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        m_sharedVarData : null,
        
        m_tabView : null,
        
        m_rangePage : null,
        m_displayPage : null,
        m_selectPage : null
    }
});