/**
 * Displays controls for customizing the profile.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.Settings", {
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
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );

            this._setLayout( new qx.ui.layout.VBox(1));

            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this._add( this.m_tabView );
        },

        
        /**
         * Store the server-side id of the server profile settings object.
         * @param id {String} the server-side id of the profile settings object.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_id : null,
        m_connector : null,
        
        m_tabView : null
    }
});