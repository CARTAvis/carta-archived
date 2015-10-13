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
         * Return the page with the given name, if it exists; otherwise return null.
         * @param pageName {String} - an identifier for a page displaying a contour set.
         * @return {skel.widgets.Image.Contour.ContourSetPage} - the page with the given name or
         *      null if none exists.
         */
        _findPage : function( pageName ){
            var target = null;
            var pages = this.m_tabView.getChildren();
            for ( var i = 0; i < pages.length; i++ ){
                if ( pages[i].getLabel() == pageName ){
                    target = pages[i];
                    break;
                }
            }
            return target;
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_tabView = new qx.ui.tabview.TabView("left");
            //this.m_tabView.addListener( "changeSelection", this._pageChanged, this );
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this.m_generatorPage = new skel.widgets.Image.Contour.GeneratorPage();
            this._add( this.m_tabView );
            this.m_tabView.add( this.m_generatorPage );
        },
        

        _pageChanged : function(){
            //console.log( "Implement page changed");
        },
        
       
        
        /**
         * Update from the server when the contour controls have changed.
         * @param controls {Object} - information about the contour controls from the server.
         */
        setControls : function( controls ){
            this.m_generatorPage.setControls( controls );
        },
        
        /**
         * Update from the server when the contour sets have changed.
         * @param controls {Object} - information about the contour sets from the server.
         */
        setControlsData : function( controls ){
            //Go through the server side contour sets and add pages that are new.
            this.m_generatorPage.setControlsData( controls );
            var i = 0;
            for ( i = 0; i < controls.contourSets.length; i++ ){
                var page = this._findPage( controls.contourSets[i].name );
                if ( page === null ){
                    page = new skel.widgets.Image.Contour.ContourSetPage();
                    page.setId( this.m_id );
                    this.m_tabView.add( page );
                }
                page.setControls( controls.contourSets[i] );
            }
            //Go through the existing pages and remove pages that are no longer there.
            var pages = this.m_tabView.getChildren();
            for ( i = 0; i < pages.length; i++ ){
                var pageLabel = pages[i].getLabel();
                var pageFound = false;
                for ( var j = 0; j < controls.contourSets.length; j++ ){
                    if ( pageLabel == controls.contourSets[j].name ){
                        pageFound = true;
                        break;
                    }
                }
                if ( !pageFound && pages[i] != this.m_generatorPage ){
                    this.m_tabView.remove( pages[i] );
                }
            }
            
        },
        
        
        /**
         * Set the server side id of this control UI.
         * @param contourId {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( contourId ){
            this.m_id = contourId;
            var pages = this.m_tabView.getChildren();
            for ( var i = 0; i < pages.length; i++ ){
                pages[i].setId ( contourId );
            }
        },
        
        m_id : null,
        
        m_tabView : null,
        m_generatorPage : null
    }
});