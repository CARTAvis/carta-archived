/**
 * Displays controls for customizing the grid.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.ImageControls", {
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
    
    events : {
        "gridControlsChanged" : "qx.event.type.Data"
    },
   
    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {

            this._setLayout( new qx.ui.layout.VBox(1));
            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this.m_tabListenId = this.m_tabView.addListener( "changeSelection", this._sendTabIndex, this );
            this._add( this.m_tabView );
          
            this.m_pages = [];
            this.m_pages[0] = new skel.widgets.Image.Grid.GridControls();
            this.m_pages[0].addListener( "gridControlsChanged", function(ev){
                this.fireDataEvent( "gridControlsChanged", ev.getData() );
            }, this );
            this.m_tabView.add( this.m_pages[0] );
            
            this.m_pages[1] = new skel.widgets.Image.Contour.ContourControls();
            this.m_tabView.add( this.m_pages[1] );
            
            this.m_pages[2] = new skel.widgets.Image.Stack.StackControls();
            this.m_tabView.add( this.m_pages[2] );
            
            this.m_pages[3] = new skel.widgets.Image.Region.RegionControls();
            this.m_tabView.add( this.m_pages[3] );   
        },
        
        /**
         * Callback for when image preference state changes on the server.
         */
        _prefCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var prefs = JSON.parse( val );
                    var tabIndex = prefs.tabIndex;
                    this._selectTab( tabIndex );
                }
                catch( err ){
                    console.log( "Could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Register to receive preference updates from the server.
         */
        _register : function(){
            var path = skel.widgets.Path.getInstance();
            
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id );
            this.m_sharedVar.addCB( this._prefCB.bind( this));
            this._prefCB();
        },
        
        
        /**
         * Send a command to the server to get the grid control id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
           this.m_id = imageId;
           for ( var i = 0; i < this.m_pages.length; i++ ){
               this.m_pages[i].setId( imageId );
           }
           this._register();
        },
        
       
        m_sharedVar : null
    }
});