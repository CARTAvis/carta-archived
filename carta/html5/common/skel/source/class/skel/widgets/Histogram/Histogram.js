/**
 * Displays a histogram and controls for customizing it.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.Histogram", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this._init();
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout(new qx.ui.layout.Grow());
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content );
            this.m_content.setLayout(new qx.ui.layout.VBox());
            
            this._initMain();
            this._initControls();
        },
       
        /**
         * Initializes the histogram settings.
         */
        _initControls : function(){
            
            this.m_settingsContainer = new skel.widgets.Histogram.Settings();
            
        },
        
        
        /**
         * Initializes the menu for setting the visibility of individual histogram
         * settings and the main graph.
         */
        _initMain : function(){
            this.m_mainComposite = new qx.ui.container.Composite();
            this.m_mainComposite.setLayout( new qx.ui.layout.VBox(2));
            this.m_mainComposite.set ({
                minWidth : this.m_MIN_DIM,
                minHeight : this.m_MIN_DIM,
                allowGrowX: true,
                allowGrowY: true
            });
            
            this.m_content.add( this.m_mainComposite, {flex:1});
        },
        
        
        /**
         * Initialize the histogram view.
         */
        _initView : function(){
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.DragView(this.m_id);
                this.m_view.setAllowGrowX( true );
                this.m_view.setAllowGrowY( true );
                this.m_view.setMinHeight(this.m_MIN_DIM);
                this.m_view.setMinWidth(this.m_MIN_DIM);
                if ( this.m_mainComposite.indexOf( this.m_view) < 0 ){
                    this.m_mainComposite.add( this.m_view, {flex:1} );
                }
            }
        },
        
        
        /**
         * Add or remove the control settings.
         */
        _layoutControls : function( visible ){
            if(visible){
                //Make sure the settings container is visible.
                if ( this.m_content.indexOf( this.m_settingsContainer ) < 0 ){
                    this.m_content.add( this.m_settingsContainer );
                }
            }
            else {
                if ( this.m_content.indexOf( this.m_settingsContainer ) >= 0 ){
                    this.m_content.remove( this.m_settingsContainer );
                }
            }
        },
        
        
        /**
         * Set the server side id of this histogram.
         * @param controlId {String} the server side id of the object that produced this histogram.
         */
        setId : function( controlId ){
            this.m_id = controlId;
            this.m_settingsContainer.setId( controlId );
            this._initView();
            
        },

        
        /**
         * Show or hide the bin count settings.
         * @param visible {boolean} if the settings should be shown; false otherwise.
         */
        showHideSettings : function( visible ){
            this._layoutControls( visible );
        },
        
        
        m_content : null,
        m_mainComposite : null,
        m_settingsContainer : null,
        
        m_MIN_DIM : 195,
        m_id : null,
        
        m_view : null
    }


});