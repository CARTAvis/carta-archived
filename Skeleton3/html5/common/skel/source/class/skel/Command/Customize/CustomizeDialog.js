/**
 * Displays a dialog for customizing commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Customize.CustomizeDialog", {
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
            
            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setWidth( 500 );
            this.m_tabView.setHeight( 600 );
            this.m_menuView = new skel.Command.Customize.CustomizeTab( skel.Command.Command.MENU);
            this.m_toolView = new skel.Command.Customize.CustomizeTab( skel.Command.Command.TOOLBAR);
            this.m_menuView.addListener( "cmdVisibilityChanged", function(ev){
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandVisibilityMenuChanged", ""));
            }, this);
            
            this.m_toolView.addListener( "cmdVisibilityChanged", function(ev){
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandVisibilityToolChanged", ""));
            }, this);
            this.m_tabView.add( this.m_menuView );
            this.m_tabView.add( this.m_toolView );
            container.add( this.m_tabView );
            qx.event.message.Bus.subscribe("windowSelected",
                function(message) {
                    this._updateViews();
                }, this );
            this._updateViews();
        },
        
        /**
         * Selects a page of the tabbed view.
         * @param menuSelected {boolean} true if the menu page should be selected; false otherwise.
         */
        setMenuPage : function( menuSelected ){
            var pageArray = [];
            if ( menuSelected ){
                pageArray.push( this.m_menuView );
            }
            else {
                pageArray.push( this.m_toolView );
            }
            this.m_tabView.setSelection( pageArray );
        },
        
        /**
         * Updates the view of the commands.
         */
        _updateViews : function (){
            var cmdFactory = skel.Command.CommandFactory.getInstance();
            var cmdList = cmdFactory.getCommandTree();
            this.m_menuView.updateTree( cmdList );
            this.m_toolView.updateTree( cmdList );
        },
        
        
        m_connector : null,
        m_menuView : null,
        m_toolView : null,
        m_tabView : null
    }
    

});