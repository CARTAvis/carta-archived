/**
 * Created by pfederl.
 *
 * New view widget capable of displaying vector graphics.
 */

qx.Class.define( "skel.boundWidgets.View.VGView", {

    extend: qx.ui.container.Composite,

    /**
     * Constructor
     */
    construct: function( viewName) {
        this.base( arguments);
        this.setLayout( new qx.ui.layout.Grow());
        this.m_viewWidget = new skel.boundWidgets.View.View( viewName );
        this.add( this.m_viewWidget);
        this.m_overlayWidget = new qx.ui.core.Widget();
        this.add( this.m_overlayWidget);
        // this.m_overlayWidget.setBackgroundColor( "rgba(255,0,0,0.2)");
    },

    members: {

        m_overlayWidget: null,
        m_viewWidget: null,

        /**
         * Get the overlay widget
         */
        overlayWidget: function()
        {
            return this.m_overlayWidget;
        },

        /**
         * Get the view widget
         */
        viewWidget: function()
        {
            return this.m_viewWidget;
        }
    }

});
