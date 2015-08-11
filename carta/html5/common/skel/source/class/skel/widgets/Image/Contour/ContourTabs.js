/**
 * A tabbed display containing a page for generating contours and a page for
 * each contour set.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Contour.ContourTabs", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments );
        this._init( );
    },

    members : {
        
        /**
         * Remove the pages which display sets of contours.
         */
        _clearPages : function(){
            for ( var i = 0; i < this.m_setPages.length; i++ ){
                this.m_tabView.remove( this.m_setPages[i]);
            }
            this.m_setPages = [];
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_tabView = new qx.ui.tabview.TabView("left");
            this.m_tabView.addListener( "changeSelection", this._pageChanged, this );
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this.m_generatorPage = new skel.widgets.Image.Contour.GeneratorPage();
            this._add( this.m_tabView );
            this.m_tabView.add( this.m_generatorPage );
        },
        

        _pageChanged : function(){
            console.log( "Implement page changed");
        },
        
        /**
         * Update from the server when the contour controls have changed.
         * @param controls {Object} - information about the contour controls from the server.
         */
        setControls : function( controls ){
            this.m_generatorPage.setControls( controls );
            
            this._clearPages();
            for ( var i = 0; i < controls.contourSets.length; i++ ){
                this.m_setPages[i] = new skel.widgets.Image.Contour.ContourSetPage();
                this.m_setPages[i].setControls( controls.contourSets[i] );
                this.m_setPages[i].setId( this.m_id );
                /*this.m_setPages[i].addListener( "contourSetChanged", function(evt){
                    var data = evt.getData();
                    var index = data.contourSetIndex();
                    
                    //Ask the contour set for all of its levels.  Send a call to the server
                    // indicating the new contour set 
                    
                }, this );*/
                this.m_tabView.add( this.m_setPages[i] );
            }
        },
        
        
        /**
         * Set the server side id of this control UI.
         * @param contourId {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( contourId ){
            this.m_id = contourId;
            this.m_generatorPage.setId ( contourId );
            for ( var i = 0; i < this.m_setPages.length; i++ ){
                this.m_setPages[i].setId ( contourId );
            }
        },
        
        m_id : null,
        
        m_tabView : null,
        m_setPages : [],
        m_generatorPage : null
    }
});