/**
 * Displays a dialog for customizing commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CustomizeDialog", {
    extend : skel.widgets.Window.MoveResizeWindow,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments, true);
        this._init();
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function() {
            this.setWidth( 100 );
            this.setHeight( 300 );
            
      
            this.setLayout(new qx.ui.layout.VBox(0));
            var scroller = new qx.ui.container.Scroll();
            var container = new qx.ui.container.Composite();
            container.setLayout(new qx.ui.layout.VBox(0));
            scroller.add(container);
            this.add(scroller, {
                flex : 1
            });
            
            var tabView = new qx.ui.tabview.TabView();
            tabView.setWidth( 500 );
            tabView.setHeight( 600 );
            this.m_menuView = new skel.widgets.Command.CustomizeTab( skel.widgets.Command.Command.MENU);
            this.m_toolView = new skel.widgets.Command.CustomizeTab( skel.widgets.Command.Command.TOOLBAR);
            /*this.m_menuView.addListener( "cmdVisibilityChanged", function(ev){
                this._updateViews( );
            }, this);
            this.m_toolView.addListener( "cmdVisibilityChanged", function(ev){
                this._updateViews( );
            }, this);*/
            tabView.add( this.m_menuView );
            tabView.add( this.m_toolView );
            container.add( tabView );
            
            this._updateViews();
        },
        
        /**
         * Updates the view of the commands.
         */
        _updateViews : function (){
            var cmdFactory = skel.widgets.Command.CommandFactory.getInstance();
            var cmdList = cmdFactory.getCommandTree();
            this.m_menuView.updateTree( cmdList );
            this.m_toolView.updateTree( cmdList );
        },
        
        
        m_connector : null,
        m_menuView : null,
        m_toolView : null

    }
    

});