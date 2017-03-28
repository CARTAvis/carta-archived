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
        console.log("grimmer view- PanZoomView");
        this.base( arguments, viewId );

        this.m_viewWidget.m_updateViewCallback = this.viewSizeHandler.bind(this);

        // monitor mouse move
        // this.addListener( "mousewheel", this._mouseWheelCB.bind(this));

        this.m_viewId = viewId;
        this.m_connector = mImport( "connector");
        var qualityValue = this.m_connector.supportsRasterViewQuality() ? 90 : 101;
        this.setQuality( qualityValue );
    },

    members: {

        viewSizeHandler: function(width, height) {
            console.log("grimmer test size:", width, ";h:", height);
            if (this.m_updateViewCallback) {
                this.m_updateViewCallback(width, height);
            }
        },

        // callback for iView refresh
        // _iviewRefreshCB : function() {
        //     console.log("grimmer aspect _iviewRefreshCB_parent2");
        //     this.fireDataEvent( "viewRefreshed");
        // },



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

        sendPanZoomLevel : function(pt, level) {
            // var box = this.overlayWidget().getContentLocation( "box" );
            // var pt = {
            //     x: ev.getDocumentLeft() - box.left,
            //     y: ev.getDocumentTop() - box.top
            // };
            //console.log( "vwid wheel", pt.x, pt.y, ev.getWheelDelta());
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + "setPanAndZoomLevel";//path.ZOOM;

            // this.m_connector.sendCommand( cmd,  newZoom);
            this.m_connector.sendCommand( cmd,
                "" + pt.x + " " + pt.y + " " + level);
        },

        sendZoomLevel: function(level) {
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + "setZoomLevel";
            console.log("grimmer aspect, set zoom level:",cmd,":", level);
            this.m_connector.sendCommand( cmd, level);
        },

        /**
         * Send an input event to the server side.
         * @param e {object}
         */
        sendInputEvent: function( e ){

            var params = JSON.stringify( e );
            //console.log( "Sending input event"+params );
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.INPUT_EVENT;
            console.log("grimmer aspect:",cmd,";",params);
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

        // m_currentZoomLevel: 1, //fitToWindow時要存. 手動放大放小時也存
        // m_effectZoomLevel:  1, //???
        m_viewId : null,
        m_inputHandlers : null,
        m_updateViewCallback: null

    }
} );
