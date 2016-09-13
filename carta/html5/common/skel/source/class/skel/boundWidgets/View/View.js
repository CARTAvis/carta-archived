/* *
 * Created by pfederl on 11/04/14.
 *
 */

/* global qx, mImport */
/* jshint strict: false */

/**
 *
 * The View class renders a remote image (provided by the server). It lets the
 * server know if the view has been resized on the client side. It also has some
 * functionality to transform coordinates between client/server.
 *
 * @ignore(mImport)
 *
 * ************************************************************************ */

qx.Class.define( "skel.boundWidgets.View.View", {

    extend: qx.ui.core.Widget,

    events:
    {
        // fired when the view is updated
        "viewRefreshed" : "qx.event.type.Data"
    },

    /**
     * @param viewName {String} the name of the view.
     */
    construct: function( viewName )
    {
        this.m_connector = mImport( "connector" );
        
        this.base( arguments );
        this.m_viewName = viewName;
        
        var setZeroTimeout = mImport( "setZeroTimeout" );

        this.addListenerOnce( "appear", this._appearCB.bind(this));
        this.addListener( "resize", function( /*e*/ )
        {
            // only continue if the dom element has been created
            if (null === this.getContentElement().getDomElement()) {
                return;
            }

            // defer calling update size by a little bit, because qooxdoo sent us the
            // resize probably before the actual html has been updated
            setZeroTimeout( this.m_iview.updateSize.bind( this.m_iview ) );

        }, this );

    },

    properties: {},

    members: {

        // set quality of the view
        setQuality : function( quality) {
            this.m_quality = quality;
            if( this.m_iview) {
                this.m_iview.setQuality( this.m_quality);
            }
        },

        // return the underlying iview
        getIView : function() {
            return this.m_iview;
        },

        // return the name of the view
        viewName : function() {
            return this.m_viewName;
        },

        // callback for appear event
        _appearCB: function()
        {
            this.m_iview = this.m_connector.registerViewElement(
            this.getContentElement().getDomElement(), this.m_viewName );
    
            this.m_iview.updateSize();
            this.m_iview.addViewCallback( this._iviewRefreshCB.bind( this ) );
            this.setQuality( this.m_quality);
        },

        // callback for iView refresh
        _iviewRefreshCB : function() {
            this.fireDataEvent( "viewRefreshed");
        },

        // overridden
        _createContentElement: function()
        {
            return new qx.html.Element( "div", {
                overflowX: "hidden",
                overflowY: "hidden",
                outline  : 0
            } );
        },

        /**
         * returns mouse event's local position (with respect to this widget)
         * @param event {MouseEvent}
         * @private
         */
        _localPos: function( event )
        {
            var box = this.getContentLocation( "box" );
            return {
                x: event.getDocumentLeft() - box.left,
                y: event.getDocumentTop() - box.top
            };
        },


        /**
         * @type {String} unique name of the view
         */
        m_viewName : null,
        /**
         * @type {IView} pointer to connector's iview
         */
        m_iview    : null,
        /**
         * @type {Connector} cached instance of the connector
         */
        m_connector: null,
        
        m_quality: null
    },

    destruct: function()
    {
    }

} );