/**
 * Created by pfederl on 05/01/15.
 */

/*global qx, mImport, skel */

/**
 @ignore( mImport)
 */

qx.Class.define( "skel.boundWidgets.View.PanZoomView", {

    extend: skel.boundWidgets.View.ViewWithInputDivSuffixed,

    /**
     * Constructor
     */
    construct: function( viewId )
    {
        this.base( arguments, viewId );

        this.m_viewWidget.m_updateViewCallback = this.viewSizeHandler.bind(this);

        // monitor mouse move, move to DisplayWindowImage.
        // this.addListener( "mousewheel", this._mouseWheelCB.bind(this));

        this.m_viewId = viewId;
        this.m_connector = mImport( "connector");
        var qualityValue = this.m_connector.supportsRasterViewQuality() ? 90 : 101;
        this.setQuality( qualityValue );
    },

    members: {

        viewSizeHandler: function(width, height) {
            if (this.m_updateViewCallback) {
                this.m_updateViewCallback(width, height);
            }
        },

        /**
         * Install an input handler.
         * @param handlerType {class}
         *
         * For example: installHandler( skel.hacks.inputHandlers.Tap)
         */
        installHandler: function( handlerType )
        {
            if( ! this.m_inputHandlers ) {
                this.m_inputHandlers = {};
            }
            if( this.m_inputHandlers[handlerType] !== undefined ) {
                console.warn( "Double install of handler" );
                return;
            }
            var handler = new handlerType( this );
            this.m_inputHandlers[handlerType] = handler;
        },

        sendPanZoom : function(pt, wheelFactor) {
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.ZOOM;

            this.m_connector.sendCommand( cmd,
                "" + pt.x + " " + pt.y + " " + wheelFactor);
        },

        // new command for mouse wheel zooom event, for 1 image, 
        sendPanZoomLevel : function(pt, level, id) {

            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + "setPanAndZoomLevel";
            this.m_connector.sendCommand( cmd,
                "" + pt.x + " " + pt.y + " " + level+" "+ id);
        },

        // new command for fitToWinowSize and setup minimal zoom level functions.
        sendZoomLevel: function(level, id) {
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + "setZoomLevel";
            this.m_connector.sendCommand( cmd, ""+level+" "+ id);
        },

        /**
         * Send an input event to the server side. E.g. mouse hover action
         * @param e {object}
         */
        sendInputEvent: function( e ){

            var params = JSON.stringify( e );
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.INPUT_EVENT;
            this.m_connector.sendCommand( cmd, params );
        },

        /**
         * Install an input handler.
         * @param handlerType {class}
         */
        uninstallHandler: function( handlerType )
        {
            if( ! this.m_inputHandlers ) {
                this.m_inputHandlers = {};
            }
            if( this.m_inputHandlers[handlerType] === undefined ) {
                console.warn( "Cannot uninstall handler" );
                return;
            }
            this.m_inputHandlers[handlerType].deactivate();
            delete this.m_inputHandlers[handlerType];
        },

        m_viewId : null,
        m_inputHandlers : null,
        m_updateViewCallback: null

    }
} );
