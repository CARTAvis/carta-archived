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

    /**
     * @param viewName {String} the name of the view.
     */
    construct: function( viewName )
    {
        this.m_connector = mImport( "connector" );
        
        this.base( arguments );
        this.m_viewName = viewName;
 

        var setZeroTimeout = mImport( "setZeroTimeout" );

        var appearListenerId = this.addListener( "appear", function( e )
                {
                    this.m_iview = this.m_connector.registerViewElement( this
                        .getContentElement().getDomElement(), this.m_viewName );
                    this.removeListenerById( appearListenerId );

                    this.m_iview.updateSize();
                }, this );
       
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

    events: {},

    properties: {},

    members: {

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
        m_connector: null
    },

    destruct: function()
    {
    }

} );