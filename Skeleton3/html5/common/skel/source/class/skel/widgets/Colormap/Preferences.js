/**
 * Displays preferences for the colormap and allows users to set them.
 */

/**

 ************************************************************************ */

qx.Class.define("skel.widgets.Colormap.Preferences", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( win ) {
        this.base(arguments);
        this.m_win = win;
        this._init();
    },

    events : {
        "closeColormapPreferences" : "qx.event.type.Data"
    },

    members : {
       
        /**
         * Initialization and set-up of the dialog.
         */
        _init: function(){
            this._setLayout( new qx.ui.layout.VBox(2));
            
            //Title
            var titleContainer = new qx.ui.container.Composite();
            titleContainer.setLayout( new qx.ui.layout.HBox());
            titleContainer.add( new qx.ui.core.Spacer(10,10), {flex:1});
            var title = new qx.ui.basic.Label( "Colormap Preferences");
            title.setFont( "titleFont");
            titleContainer.add( title );
            titleContainer.add( new qx.ui.core.Spacer(10,10), {flex:1});
            this._add( titleContainer );
            
            //Significant Digits
            var butContainer = new qx.ui.container.Composite();
            butContainer.setLayout( new qx.ui.layout.HBox());
            var signLabel = new qx.ui.basic.Label("Significant Digits:");
            var significantDigits = this.m_win.getSignificantDigits();
            this.m_signSpin = new qx.ui.form.Spinner(1, significantDigits, 10 );
            butContainer.add( signLabel );
            butContainer.add( this.m_signSpin );
            this._add(butContainer );
            
            //Close and apply buttons
            var closeContainer = new qx.ui.container.Composite();
            closeContainer.setLayout( new qx.ui.layout.HBox());
            this.m_applyButton = new qx.ui.form.Button( "Apply");
            this.m_applyButton.addListener( "execute", function(){
                this.m_win.setSignificantDigits( this.m_signSpin.getValue());
            }, this );
            this.m_closeButton = new qx.ui.form.Button( "Close");
            this.m_closeButton.addListener( "execute", function(){
                this.fireDataEvent( "closeColormapPreferences", null );
            }, this );
            closeContainer.add( new qx.ui.core.Spacer(10,10), {flex:1});
            closeContainer.add( this.m_applyButton );
            closeContainer.add( this.m_closeButton );
            this._add( closeContainer );
        },
        
        m_win : null,
        m_applyButton : null,
        m_closeButton: null,
        m_signSpin : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }
    }

});
