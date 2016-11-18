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

        // monitor mouse move
        this.addListener( "mousewheel", this._mouseWheelCB.bind(this));

        this.m_viewId = viewId;
        this.m_connector = mImport( "connector");
        var qualityValue = this.m_connector.supportsRasterViewQuality() ? 90 : 101;
        this.setQuality( qualityValue );
    },

    members: {
    	
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

        _mouseWheelCB : function(ev) {
            var box = this.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };
            //console.log( "vwid wheel", pt.x, pt.y, ev.getWheelDelta());
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.ZOOM;
            this.m_connector.sendCommand( cmd,
                "" + pt.x + " " + pt.y + " " + ev.getWheelDelta());
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
        m_inputHandlers : null

    }
} );
