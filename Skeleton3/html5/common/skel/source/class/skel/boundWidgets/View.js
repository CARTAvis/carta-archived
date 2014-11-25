/**
 * Created by pfederl on 11/04/14.
 */

/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 09/04/13
 * Time: 3:09 PM
 * To change this template use File | Settings | File Templates.
 */

/* global qx, mImport */
/* jshint strict: false */

/**

 @ignore(mImport)

 ************************************************************************ */

qx.Class.define( "skel.boundWidgets.View", {

    extend: qx.ui.core.Widget,

    /**
     * @param viewName {String} the name of the view.
     */
    construct: function( viewName )
    {
        this.m_connector = mImport( "connector" );

        this.base( arguments );
        this.m_viewName = viewName;
        // reverting the following change because it does not make sense
        //var path = skel.widgets.Path.getInstance();
        //this.m_viewName = viewName + path.SEP + path.VIEW;

        var setZeroTimeout = mImport( "setZeroTimeout" );

        // listen for appear event, because the html is not generated until the widget
        // appears
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
            if( null == this.getContentElement().getDomElement() ) {
                return;
            }

            // defer calling update size by a little bit, because qooxdoo sent us the
            // resize probably before the actual html has been updated
            setZeroTimeout( this.m_iview.updateSize.bind( this.m_iview ) );

        }, this );

        //this.addListener( "mousemove", this._mouseMoveCB );
        //this.addListener( "mousedown", this._mouseDownCB );
        //this.addListener( "mouseup", this._mouseUpCB );
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
        /*
         _mouseMoveCB: function( event )
         {
         if( typeof this.m_iview.mouseMoveCB === "function" ) {
         this.m_iview.mouseMoveCB( event );
         }
         // Todo: write for server connector.
         },

         _mouseDownCB: function( event )
         {


         },

         _mouseUpCB: function( event )
         {

         },

         _sendCoords: function( pt )
         {

         },

         */

        /**
         * @type {String} unique name of the view
         */
        m_viewName : null,
        m_iview    : null,
        m_connector: null
    },

    destruct: function()
    {
    }

} );