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

/*global qx, mImport */

/**

 @ignore(mImport)
 @ignore(fv.console.log)
 @ignore(fv.hub.*)
 @ignore(fv.GLOBAL_DEBUG)

 ************************************************************************ */

qx.Class.define("skel.boundWidgets.View",
    {
        extend: qx.ui.core.Widget,

        /**
         * @param viewName {String} the name of the view.
         */
        construct: function (viewName) {

            this.m_connector = mImport("connector");

            this.base(arguments);
            this.m_viewName = viewName;

            var setZeroTimeout = mImport( "setZeroTimeout");

//            this.getContentElement().setAttributes(
//                {
//                    id: "grrrrrsome11231213"
////                    , "class": "pureweb"
//                });

            // listen for appear event, because the html is not generated until the widget
            // appears
            var appearListenerId = this.addListener("appear", function (e) {

//                fv.hub.emit("ui.pureWebView.created", {
//                    id: this.m_id,
//                    viewName: this.m_viewName,
//                    format: this.m_format
//                });
//
//                var div = this.getContentElement().getDomElement();
//                var ww = this.getWidth();
//                var hh = this.getHeight();
//                fv.hub.emit("ui.pureWebView.resized",
//                    { width: this.getWidth(),
//                        height: this.getHeight(),
//                        viewName: this.m_viewName
//                    });

                this.m_iview = this.m_connector.registerViewElement(
                    this.getContentElement().getDomElement(), this.m_viewName );
                this.removeListenerById(appearListenerId);

                this.m_iview.updateSize();

                console.log( "view appeared", this.m_iview);
                window.gview = this.m_iview;



            }, this);

            this.addListener("resize", function (/*e*/) {
                console.log( "resize?");
                // only fire resize event if the actual dom element has been created
                if (null == this.getContentElement().getDomElement()) { return; }

//                setTimeout( this.m_iview.updateSize.bind(this.m_iview), 10);
                setZeroTimeout( this.m_iview.updateSize.bind(this.m_iview));
//                qx.ui.core.queue.Widget.flush();
//                console.log( "yes", this.getContentElement().getDomElement().getBoundingClientRect().width);
//                this.m_iview.updateSize();

            }, this);

            /*
             this.addListener( "appear", function() {
             fv.console.log( "appear-" + this.m_id);
             }, this);

             this.addListener( "disappear", function() {
             fv.console.log( "disappear-" +this.m_id);
             }, this);
             */
        },

        events: {
        },

        properties: {
        },

        members: {

            // overridden
            _createContentElement: function () {
                return new qx.html.Element("div", {
                    overflowX: "hidden",
                    overflowY: "hidden"
                });
            },

            m_viewName: null,
            m_iview: null,
            m_connector: null

        },

        destruct: function () {
        }

    });