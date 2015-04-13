/**
 * Displays a dialog for customizing commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Customize.CustomizeDialog", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this._init();
    },
    
    events : {
        "closeCustomizeDialog" : "qx.event.type.Data"
    },
    
    members : {

        /**
         * Initializes the UI.
         */
        _init : function() {
            this._setLayout(new qx.ui.layout.VBox(3));
            var scroller = new qx.ui.container.Scroll();
            var container = new qx.ui.container.Composite();
            container.setLayout(new qx.ui.layout.VBox(0));
            scroller.add(container);
            this._add(scroller, {
                flex : 1
            });
            
            this.m_tabView = new qx.ui.tabview.TabView("top");
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
            
            
            var butContainer = new qx.ui.container.Composite();
            butContainer.setLayout( new qx.ui.layout.HBox());
            butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            var closeButton = new qx.ui.form.Button( "Close");
            closeButton.addListener( "execute", function(){
                this.fireDataEvent("closeCustomizeDialog", "");
            }, this);
            butContainer.add( closeButton );
            this._add( butContainer);
            
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
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }
    }
});